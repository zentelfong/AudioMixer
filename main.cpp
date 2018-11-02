#include "mixer/audio_mixer.h"
#include "mixer/avx_helper.h"

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

	std::vector<MixerSource> sources;
	sources.push_back(MixerSource(MixerSource::TYPE_FILE, 1, 1.0, "test1.mp3", 0, 0));
	sources.push_back(MixerSource(MixerSource::TYPE_FILE, 2, 0.5, "test2.mp3", 0, 0));
	MixerConfig config(sources,44100,2,10);
	std::shared_ptr<AudioMixerApi> mixer = AudioMixerApi::Create(config);

	char buf[AudioMixerApi::MAX_BUF_SIZE];
	FILE* file = fopen("out.pcm", "wb");

	int len = mixer->Mix(buf);
	while (len>0)
	{
		fwrite(buf, 1, len, file);
		len = mixer->Mix(buf);
	}
	fclose(file);
}
