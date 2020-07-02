#pragma once
#include "api/halley_api_internal.h"

namespace Halley {
	class DummyAudioAPI : public AudioOutputAPIInternal {
	public:
		Vector<std::unique_ptr<const AudioDevice>> getAudioDevices() override;
		AudioSpec openAudioDevice(const AudioSpec& requestedFormat, const AudioDevice* device, AudioCallback prepareAudioCallback) override;
		void closeAudioDevice() override;
		void startPlayback() override;
		void stopPlayback() override;
		void init() override;
		void deInit() override;
		bool needsMoreAudio() override;
		bool needsAudioThread() const override;
		void onAudioAvailable() override;
	};
}
