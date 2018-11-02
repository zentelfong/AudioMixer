/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "audio_mixer/frame_combiner.h"

#include <algorithm>
#include <array>
#include <functional>

#include "rtc_base/array_view.h"
#include "audio/audio_frame_operations.h"
#include "audio/audio_util.h"
#include "audio_mixer/audio_frame_manipulator.h"
#include "audio_mixer/audio_mixer_impl.h"
#include "audio/audio_frame_view.h"
#include "rtc_base/arraysize.h"
#include "rtc_base/checks.h"

namespace webrtc {
namespace {

// Stereo, 48 kHz, 10 ms.
constexpr int kMaximumAmountOfChannels = 2;
constexpr int kMaximumChannelSize = 48 * AudioMixerImpl::kFrameDurationInMs;

using OneChannelBuffer = std::array<float, kMaximumChannelSize>;

void SetAudioFrameFields(const std::vector<AudioFrame*>& mix_list,
                         size_t number_of_channels,
                         int sample_rate,
                         size_t number_of_streams,
                         AudioFrame* audio_frame_for_mixing) {
  const size_t samples_per_channel = static_cast<size_t>(
      (sample_rate * webrtc::AudioMixerImpl::kFrameDurationInMs) / 1000);

  // TODO(minyue): Issue bugs.webrtc.org/3390.
  // Audio frame timestamp. The 'timestamp_' field is set to dummy
  // value '0', because it is only supported in the one channel case and
  // is then updated in the helper functions.
  audio_frame_for_mixing->UpdateFrame(
      0, nullptr, samples_per_channel, sample_rate, AudioFrame::kUndefined,
      AudioFrame::kVadUnknown, number_of_channels);

  if (mix_list.empty()) {
    audio_frame_for_mixing->elapsed_time_ms_ = -1;
  } else if (mix_list.size() == 1) {
    audio_frame_for_mixing->timestamp_ = mix_list[0]->timestamp_;
    audio_frame_for_mixing->elapsed_time_ms_ = mix_list[0]->elapsed_time_ms_;
  }
}

void MixFewFramesWithNoLimiter(const std::vector<AudioFrame*>& mix_list,
                               AudioFrame* audio_frame_for_mixing) {
  if (mix_list.empty()) {
    audio_frame_for_mixing->Mute();
    return;
  }
  RTC_DCHECK_LE(mix_list.size(), 1);
  std::copy(mix_list[0]->data(),
            mix_list[0]->data() +
                mix_list[0]->num_channels_ * mix_list[0]->samples_per_channel_,
            audio_frame_for_mixing->mutable_data());
}

std::array<OneChannelBuffer, kMaximumAmountOfChannels> MixToFloatFrame(
    const std::vector<AudioFrame*>& mix_list,
    size_t samples_per_channel,
    size_t number_of_channels) {
  // Convert to FloatS16 and mix.
  using OneChannelBuffer = std::array<float, kMaximumChannelSize>;
  std::array<OneChannelBuffer, kMaximumAmountOfChannels> mixing_buffer{};

  for (size_t i = 0; i < mix_list.size(); ++i) {
    const AudioFrame* const frame = mix_list[i];
    for (size_t j = 0; j < number_of_channels; ++j) {
      for (size_t k = 0; k < samples_per_channel; ++k) {
        mixing_buffer[j][k] += frame->data()[number_of_channels * k + j];
      }
    }
  }
  return mixing_buffer;
}

// Both interleaves and rounds.
void InterleaveToAudioFrame(AudioFrameView<const float> mixing_buffer_view,
                            AudioFrame* audio_frame_for_mixing) {
  const size_t number_of_channels = mixing_buffer_view.num_channels();
  const size_t samples_per_channel = mixing_buffer_view.samples_per_channel();
  // Put data in the result frame.
  for (size_t i = 0; i < number_of_channels; ++i) {
    for (size_t j = 0; j < samples_per_channel; ++j) {
      audio_frame_for_mixing->mutable_data()[number_of_channels * j + i] =
          FloatS16ToS16(mixing_buffer_view.channel(i)[j]);
    }
  }
}
}  // namespace

FrameCombiner::FrameCombiner(){
}

FrameCombiner::~FrameCombiner() = default;

void FrameCombiner::Combine(const std::vector<AudioFrame*>& mix_list,
                            size_t number_of_channels,
                            int sample_rate,
                            size_t number_of_streams,
                            AudioFrame* audio_frame_for_mixing) {
  RTC_DCHECK(audio_frame_for_mixing);

  SetAudioFrameFields(mix_list, number_of_channels, sample_rate,
                      number_of_streams, audio_frame_for_mixing);

  const size_t samples_per_channel = static_cast<size_t>(
      (sample_rate * webrtc::AudioMixerImpl::kFrameDurationInMs) / 1000);

  for (const auto* frame : mix_list) {
    RTC_DCHECK_EQ(samples_per_channel, frame->samples_per_channel_);
    RTC_DCHECK_EQ(sample_rate, frame->sample_rate_hz_);
  }

  // The 'num_channels_' field of frames in 'mix_list' could be
  // different from 'number_of_channels'.
  for (auto* frame : mix_list) {
    RemixFrame(number_of_channels, frame);
  }

  if (number_of_streams <= 1) {
    MixFewFramesWithNoLimiter(mix_list, audio_frame_for_mixing);
    return;
  }

  std::array<OneChannelBuffer, kMaximumAmountOfChannels> mixing_buffer =
      MixToFloatFrame(mix_list, samples_per_channel, number_of_channels);

  // Put float data in an AudioFrameView.
  std::array<float*, kMaximumAmountOfChannels> channel_pointers{};
  for (size_t i = 0; i < number_of_channels; ++i) {
    channel_pointers[i] = &mixing_buffer[i][0];
  }
  AudioFrameView<float> mixing_buffer_view(
      &channel_pointers[0], number_of_channels, samples_per_channel);

  InterleaveToAudioFrame(mixing_buffer_view, audio_frame_for_mixing);
}


}  // namespace webrtc
