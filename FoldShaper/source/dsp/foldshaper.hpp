// SPDX-License-Identifier: GPL-3.0-only
// Copyright Takamitsu Endo (ryukau@gmail.com)

#pragma once

#include "../../../common/dsp/multirate.hpp"

#include <algorithm>
#include <cmath>

namespace SomeDSP {

template<typename Sample> inline Sample safeClip(Sample input)
{
  return std::isfinite(input) ? std::clamp<Sample>(input, Sample(-1024), Sample(1024))
                              : 0;
}

template<typename Sample> class FoldShaper {
public:
  Sample gain = 1;
  Sample multiply = 1; // Must be greater than 0.
  bool hardclip = true;

  FirUpSampler<Sample, Fir16FoldUpSample<Sample>> upSampler;
  DecimationLowpass<Sample, Sos16FoldFirstStage<Sample>> lowpass;
  HalfBandIIR<Sample, HalfBandCoefficient<Sample>> halfbandIir;

  size_t latency() { return Fir16FoldUpSample<Sample>::intDelay; }

  void reset()
  {
    upSampler.reset();
    lowpass.reset();
    halfbandIir.reset();
  }

  Sample process(Sample x0)
  {
    if (hardclip) x0 = std::clamp(x0, Sample(-1), Sample(1));
    Sample absed = std::fabs(x0 * gain);
    Sample floored = std::floor(absed);
    Sample mul = std::pow(multiply, floored);

    Sample output;
    if (int(floored) % 2 == 1) {
      output = std::copysign(Sample(1), x0) - std::copysign(mul * (absed - floored), x0);
    } else if (floored >= Sample(1)) {
      output = std::copysign(mul * (absed - floored) + (Sample(1) - mul / multiply), x0);
    } else {
      output = std::copysign(mul * (absed - floored) + (Sample(1) - mul), x0);
    }
    return safeClip(output);
  }

  Sample process16(Sample x0)
  {
    upSampler.process(x0);

    std::array<Sample, 2> halfBandInput;
    for (size_t i = 0; i < 8; ++i) lowpass.push(process(upSampler.output[i]));
    halfBandInput[0] = lowpass.output();
    for (size_t i = 8; i < 16; ++i) lowpass.push(process(upSampler.output[i]));
    halfBandInput[1] = lowpass.output();

    return halfbandIir.process(halfBandInput);
  }
};

} // namespace SomeDSP
