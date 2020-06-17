#include "audio_engine.h"
#include "audio_mixer.h"
#include <thread>
#include <chrono>
#include "audio_source_clip.h"
#include "audio_filter_resample.h"
#include "halley/support/debug.h"
#include "halley/core/resources/resources.h"
#include "audio_event.h"

using namespace Halley;

AudioEngine::AudioEngine()
	: mixer(AudioMixer::makeMixer())
	, pool(std::make_unique<AudioBufferPool>())
	, running(true)
	, needsBuffer(true)
{
	rng.setSeed(Random::getGlobal().getRawInt());
}

AudioEngine::~AudioEngine()
{
}

void AudioEngine::postEvent(size_t id, std::shared_ptr<const AudioEvent> event, const AudioPosition& position)
{
	event->run(*this, id, position);
}

void AudioEngine::play(size_t id, std::shared_ptr<const IAudioClip> clip, AudioPosition position, float volume, bool loop)
{
	addEmitter(id, std::make_unique<AudioEmitter>(std::make_shared<AudioSourceClip>(clip, loop, 0), position, volume, getGroupId("")));
}

void AudioEngine::setListener(AudioListenerData l)
{
	listener = l;
}

void AudioEngine::setOutputChannels(std::vector<AudioChannelData> channelData)
{
	if (channels.size() == channelData.size()) {
		channels = std::move(channelData);
	}
}

void AudioEngine::run()
{
	//const size_t bufSize = spec.numChannels * sizeof(AudioConfig::SampleFormat) * spec.bufferSize;

	// Generate one buffer
	if (running && out->needsMoreAudio()) {
		generateBuffer();
	}

	// OK, we've supplied it with enough buffers; if that was enough, then, sleep as long as no more buffers are needed
	while (running && !out->needsMoreAudio()) {
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(100us);
	}
	
	// When we get here, it means that buffers are needed again (either one wasn't enough, or we waited long enough),
	// but first return so we the AudioFacade can update the incoming sound data
}

void AudioEngine::addEmitter(size_t id, std::unique_ptr<AudioEmitter>&& src)
{
	emitters.emplace_back(std::move(src));
	emitters.back()->setId(id);
	idToSource[id].push_back(emitters.back().get());
}

const std::vector<AudioEmitter*>& AudioEngine::getSources(size_t id)
{
	auto src = idToSource.find(id);
	if (src != idToSource.end()) {
		return src->second;
	} else {
		return dummyIdSource;
	}
}

std::vector<size_t> AudioEngine::getPlayingSounds()
{
	std::vector<size_t> result(idToSource.size());
	size_t i = 0;
	for (const auto& kv: idToSource) {
		result[i++] = kv.first;
	}
	return result;
}

void AudioEngine::start(AudioSpec s, AudioOutputAPI& o)
{
	spec = s;
	out = &o;

	channels.resize(spec.numChannels);
	channels[0].pan = -1.0f;
	channels[1].pan = 1.0f;

	if (spec.sampleRate != 48000) {
		outResampler = std::make_unique<AudioResampler>(48000, spec.sampleRate, spec.numChannels, Debug::isDebug() ? 0.0f : 0.5f);
	}
}

void AudioEngine::resume()
{
	running = true;
	needsBuffer = false;
}

void AudioEngine::pause()
{
	running = false;
	needsBuffer = false;
}

void AudioEngine::generateBuffer()
{
	const size_t samplesToRead = alignUp(spec.bufferSize * 48000 / spec.sampleRate, 16);
	const size_t packsToRead = samplesToRead / 16;
	const size_t numChannels = spec.numChannels;
	
	auto channelBuffersRef = pool->getBuffers(numChannels, samplesToRead);
	auto channelBuffers = channelBuffersRef.getBuffers();
	mixEmitters(samplesToRead, numChannels, channelBuffers);
	removeFinishedEmitters();

	auto bufferRef = pool->getBuffer(samplesToRead * numChannels);
	auto buffer = bufferRef.getSpan().subspan(0, packsToRead * numChannels);
	mixer->interleaveChannels(buffer, channelBuffers);
	mixer->compressRange(buffer);

	// Resample to output sample rate, if necessary
	if (outResampler) {
		auto resampledBuffer = pool->getBuffer(samplesToRead * numChannels * spec.sampleRate / 48000 + 16);
		auto result = outResampler->resampleInterleaved(bufferRef.getSampleSpan().subspan(0, samplesToRead * numChannels), resampledBuffer.getSampleSpan());
		if (result.nRead != samplesToRead) {
			throw Exception("Failed to read all input sample data", HalleyExceptions::AudioEngine);
		}
		out->queueAudio(resampledBuffer.getSampleSpan().subspan(0, result.nWritten * numChannels));
	} else {
		out->queueAudio(bufferRef.getSampleSpan());
	}
}

Random& AudioEngine::getRNG()
{
	return rng;
}

AudioBufferPool& AudioEngine::getPool() const
{
	return *pool;
}

void AudioEngine::setMasterGain(float gain)
{
	masterGain = gain;
}

void AudioEngine::setGroupGain(const String& name, float gain)
{
	groupGains[getGroupId(name)] = gain;
}

void AudioEngine::mixEmitters(size_t numSamples, size_t nChannels, gsl::span<AudioBuffer*> buffers)
{
	// Clear buffers
	for (size_t i = 0; i < nChannels; ++i) {
		clearBuffer(buffers[i]->packs);
	}

	// Mix every emitter
	for (auto& e: emitters) {
		// Start playing if necessary
		if (!e->isPlaying() && !e->isDone() && e->isReady()) {
			e->start();
		}

		// Mix it in!
		if (e->isPlaying()) {
			e->update(channels, listener, masterGain * getGroupGain(e->getGroup()));
			e->mixTo(numSamples, buffers, *mixer, *pool);
		}
	}
}

void AudioEngine::removeFinishedEmitters()
{
	for (auto& e: emitters) {
		if (e->isDone()) {
			auto iter = idToSource.find(e->getId());
			if (iter != idToSource.end()) {
				auto& ems = iter->second;
				if (ems.size() > 1) {
					ems.erase(std::remove_if(ems.begin(), ems.end(), [] (const AudioEmitter* e) { return e->isDone(); }), ems.end());
				} else {
					idToSource.erase(iter);
				}
			}
		}
	}
	emitters.erase(std::remove_if(emitters.begin(), emitters.end(), [&] (const std::unique_ptr<AudioEmitter>& src) { return src->isDone(); }), emitters.end());
}

void AudioEngine::clearBuffer(gsl::span<AudioSamplePack> dst)
{
	for (auto& pack : dst) {
		for (size_t i = 0; i < AudioSamplePack::NumSamples; ++i) {
			pack.samples[i] = 0.0f;
		}
	}
}

int AudioEngine::getGroupId(const String& group)
{
	auto iter = std::find(groupNames.begin(), groupNames.end(), group);
	if (iter != groupNames.end()) {
		return iter - groupNames.begin();
	} else {
		groupNames.push_back(group);
		groupGains.push_back(1.0f);
		return int(groupNames.size()) - 1;
	}
}

float AudioEngine::getGroupGain(int id) const
{
	return groupGains[id];
}
