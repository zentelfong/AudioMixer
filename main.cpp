#include "mixer/audio_mixer.h"
#include "mixer/avx_helper.h"
#include "audio/dr_wav.h"

#ifdef WIN32
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avdevice.lib")
#pragma comment(lib,"avfilter.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"swresample.lib")
#endif

using namespace audio_mixer;

void main()
{
	avcodec_register_all();
	av_register_all();


	drwav_data_format format;
	format.container = drwav_container_riff;
	format.format = DR_WAVE_FORMAT_PCM;
	format.channels = 2;
	format.sampleRate = 44100;
	format.bitsPerSample = 16;
	drwav* pWav = drwav_open_file_write("mixed.wav", &format);


	std::vector<MixerSource> sources;
	sources.push_back(MixerSource(MixerSource::TYPE_FILE, 1, 1.0, "test1.wav", 0, 0));
	sources.push_back(MixerSource(MixerSource::TYPE_FILE, 2, 0.4, "test2.mp3", 0, 0));
	MixerConfig config(sources,44100,2,10);
	std::shared_ptr<AudioMixerApi> mixer = AudioMixerApi::Create(config);

	char buf[AudioMixerApi::MAX_BUF_SIZE];

	int len = mixer->Mix(buf);
	while (len>0)
	{
		drwav_write(pWav, len/2, buf);
		len = mixer->Mix(buf);
	}
	drwav_close(pWav);
}
