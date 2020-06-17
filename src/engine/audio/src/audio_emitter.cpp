#include "audio_emitter.h"
#include <utility>
#include "audio_mixer.h"
#include "audio_emitter_behaviour.h"
#include "audio_source.h"
#include "halley/support/logger.h"

using namespace Halley;

AudioEmitter::AudioEmitter(std::shared_ptr<AudioSource> source, AudioPosition sourcePos, float gain, int group) 
	: source(std::move(source))
	, sourcePos(std::move(sourcePos))
	, group(group)
	, gain(gain)
{}

AudioEmitter::~AudioEmitter() = default;

void AudioEmitter::setId(size_t i)
{
	id = i;
}

size_t AudioEmitter::getId() const
{
	return id;
}

void AudioEmitter::start()
{
	Expects(isReady());
	Expects(!playing);

	playing = true;
	nChannels = source->getNumberOfChannels();

	if (nChannels > 1) {
		sourcePos = AudioPosition::makeFixed();
	}
}

void AudioEmitter::stop()
{
	playing = false;
	done = true;
}

bool AudioEmitter::isPlaying() const
{
	return playing;
}

bool AudioEmitter::isReady() const
{
	return source->isReady();
}

bool AudioEmitter::isDone() const
{
	return done;
}

void AudioEmitter::setBehaviour(std::shared_ptr<AudioEmitterBehaviour> value)
{
	behaviour = std::move(value);
	elapsedTime = 0;
	behaviour->onAttach(*this);
}

int AudioEmitter::getGroup() const
{
	return group;
}

void AudioEmitter::setGain(float g)
{
	gain = g;
}

void AudioEmitter::setAudioSourcePosition(Vector3f position)
{
	if (nChannels == 1) {
		sourcePos.setPosition(position);
	}
}

void AudioEmitter::setAudioSourcePosition(AudioPosition s)
{
	if (nChannels == 1) {
		sourcePos = std::move(s);
	}
}

float AudioEmitter::getGain() const
{
	return gain;
}

size_t AudioEmitter::getNumberOfChannels() const
{
	return nChannels;
}

void AudioEmitter::update(gsl::span<const AudioChannelData> channels, const AudioListenerData& listener, float groupGain)
{
	Expects(playing);

	if (behaviour) {
		bool keep = behaviour->update(elapsedTime, *this);
		if (!keep) {
			behaviour.reset();
		}
		elapsedTime = 0;
	}

	prevChannelMix = channelMix;
	sourcePos.setMix(nChannels, channels, channelMix, gain * groupGain, listener);
	
	if (isFirstUpdate) {
		prevChannelMix = channelMix;
		isFirstUpdate = false;
	}
}

void AudioEmitter::mixTo(size_t numSamples, gsl::span<AudioBuffer*> dst, AudioMixer& mixer, AudioBufferPool& pool)
{
	Expects(dst.size() > 0);
	Expects(numSamples % 16 == 0);

	const size_t numPacks = numSamples / 16;
	Expects(dst[0]->packs.size() >= numPacks);
	const size_t nSrcChannels = getNumberOfChannels();
	const auto nDstChannels = size_t(dst.size());

	// Figure out the total mix in the previous update, and now. If it's zero, then there's nothing to listen here.
	float totalMix = 0.0f;
	const size_t nMixes = nSrcChannels * nDstChannels;
	Expects (nMixes < 16);
	for (size_t i = 0; i < nMixes; ++i) {
		totalMix += prevChannelMix[i] + channelMix[i];
	}

	// Read data from source
	std::array<gsl::span<AudioSamplePack>, AudioConfig::maxChannels> audioData;
	std::array<gsl::span<AudioConfig::SampleFormat>, AudioConfig::maxChannels> audioSampleData;
	std::array<AudioBufferRef, AudioConfig::maxChannels> bufferRefs;
	for (size_t srcChannel = 0; srcChannel < nSrcChannels; ++srcChannel) {
		bufferRefs[srcChannel] = pool.getBuffer(numSamples);
		audioData[srcChannel] = bufferRefs[srcChannel].getSpan().subspan(0, numPacks);
		audioSampleData[srcChannel] = audioData[srcChannel].data()->samples;
	}
	bool isPlaying = source->getAudioData(numSamples, audioSampleData);

	// If we're audible, render
	if (totalMix >= 0.0001f) {
		// Render each emitter channel
		for (size_t srcChannel = 0; srcChannel < nSrcChannels; ++srcChannel) {
			// Read to buffer
			for (size_t dstChannel = 0; dstChannel < nDstChannels; ++dstChannel) {
				// Compute mix
				const size_t mixIndex = (srcChannel * nChannels) + dstChannel;
				const float gain0 = prevChannelMix[mixIndex];
				const float gain1 = channelMix[mixIndex];

				// Render to destination
				if (gain0 + gain1 > 0.0001f) {
					mixer.mixAudio(audioData[srcChannel], dst[dstChannel]->packs, gain0, gain1);
				}
			}
		}
	}

	advancePlayback(numSamples);
	if (!isPlaying) {
		stop();
	}
}

void AudioEmitter::advancePlayback(size_t samples)
{
	elapsedTime += float(samples) / AudioConfig::sampleRate;
}
