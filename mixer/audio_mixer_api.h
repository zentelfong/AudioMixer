#pragma once
#include <cstdint>
#include <memory>
#include "mixer_source.h"
#include "mixer_config.h"

namespace audio_mixer {

class AudioMixerApi {
public:
    virtual ~AudioMixerApi() {}

	enum {
		MAX_BUF_SIZE = 7680,
		SAMPLE_SIZE = 2
	};

    static std::shared_ptr<AudioMixerApi> Create(const MixerConfig & config);

    virtual void UpdateVolume(int32_t ssrc, float volume) = 0;

    virtual bool AddSource(const MixerSource & source) = 0;

    virtual bool RemoveSource(int32_t ssrc) = 0;

	virtual int32_t Mix(void* output_buffer) = 0;

	virtual int32_t AddRecordedDataAndMix(const void* data, int32_t size, 
		void* output_buffer) = 0;
};

}  // namespace audio_mixer
