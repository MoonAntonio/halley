#include "halley/audio/resampler.h"
#include "../../../../contrib/speex/speex_resampler.h"

using namespace Halley;

static std::unique_ptr<SpeexResamplerState, void(*)(SpeexResamplerState*)> makeResampler(int from, int to, int channels, float quality)
{
	int err;
	auto * rawResampler = speex_resampler_init(unsigned(channels), unsigned(from), unsigned(to), std::max(0, std::min(int(quality * 10 + 0.5f), 10)), &err);
	return std::unique_ptr<SpeexResamplerState, void(*)(SpeexResamplerState*)>(rawResampler, [] (SpeexResamplerState* s) { speex_resampler_destroy(s); });
}

AudioResampler::AudioResampler(int from, int to, int nChannels, float quality)
	: resampler(makeResampler(from, to, nChannels, quality))
	, nChannels(size_t(nChannels))
	, from(from)
	, to(to)
{
}

AudioResampler::~AudioResampler() = default;

AudioResamplerResult AudioResampler::resample(gsl::span<const float> src, gsl::span<float> dst, size_t channel)
{
	unsigned inLen = unsigned(src.size() / nChannels);
	unsigned outLen = unsigned(dst.size() / nChannels);
	speex_resampler_process_float(resampler.get(), unsigned(channel), src.data(), &inLen, dst.data(), &outLen);
	AudioResamplerResult result;
	result.nRead = inLen;
	result.nWritten = outLen;
	return result;
}

AudioResamplerResult AudioResampler::resampleInterleaved(gsl::span<const float> src, gsl::span<float> dst)
{
	unsigned inLen = unsigned(src.size() / nChannels);
	unsigned outLen = unsigned(dst.size() / nChannels);
	speex_resampler_process_interleaved_float(resampler.get(), src.data(), &inLen, dst.data(), &outLen);
	AudioResamplerResult result;
	result.nRead = inLen;
	result.nWritten = outLen;
	return result;
}

AudioResamplerResult AudioResampler::resampleInterleaved(gsl::span<const short> src, gsl::span<short> dst)
{
	unsigned inLen = unsigned(src.size() / nChannels);
	unsigned outLen = unsigned(dst.size() / nChannels);
	speex_resampler_process_interleaved_int(resampler.get(), src.data(), &inLen, dst.data(), &outLen);
	AudioResamplerResult result;
	result.nRead = inLen;
	result.nWritten = outLen;
	return result;
}

AudioResamplerResult AudioResampler::resampleNoninterleaved(gsl::span<const float> src, gsl::span<float> dst, const size_t numChannels)
{
	AudioResamplerResult result;
	result.nRead = 0;
	result.nWritten = 0;

	for (size_t i = 0; i < numChannels; ++i) {
		unsigned inLen = unsigned(src.size() / nChannels);
		unsigned outLen = unsigned(dst.size() / nChannels);
		speex_resampler_process_float(resampler.get(), unsigned(i), src.subspan(result.nRead).data(), &inLen, dst.subspan(result.nWritten).data(), &outLen);
		result.nRead += inLen;
		result.nWritten += outLen;
	}

	return result;
}

size_t AudioResampler::numOutputSamples(size_t numInputSamples) const
{
	return numInputSamples * to / from;
}
