#include "audio_mixer/audio_mixer_impl.h"

#ifdef WIN32
#pragma comment(lib,"winmm.lib")
#endif

void main()
{
	auto mixer = webrtc::AudioMixerImpl::Create();



}