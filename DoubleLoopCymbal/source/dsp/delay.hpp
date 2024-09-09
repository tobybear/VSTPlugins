// (c) 2024 Takamitsu Endo
//
// This file is part of DoubleLoopCymbal.
//
// DoubleLoopCymbal is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DoubleLoopCymbal is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with DoubleLoopCymbal.  If not, see <https://www.gnu.org/licenses/>.

#include "../../../lib/pcg-cpp/pcg_random.hpp"
#include "../../lib/LambertW/LambertW.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <numeric>
#include <random>
#include <vector>

namespace SomeDSP {

template<typename Sample> class ExpDecay {
public:
  Sample value = 0;
  Sample alpha = 0;

  void setTime(Sample decayTimeInSamples, bool sustain = false)
  {
    constexpr auto eps = Sample(std::numeric_limits<float>::epsilon());
    alpha = sustain ? Sample(1) : std::pow(eps, Sample(1) / decayTimeInSamples);
  }

  void reset() { value = 0; }
  void trigger(Sample gain = Sample(1)) { value = gain; }
  Sample process() { return value *= alpha; }
};

template<typename Sample> class ExpDSREnvelope {
public:
  enum class State { decay, release };

private:
  Sample value = 0;
  Sample alphaD = 0;
  Sample alphaR = 0;
  Sample offset = 0;
  State state = State::release;

public:
  void setTime(Sample decayTimeInSamples, Sample releaseTimeInSamples)
  {
    constexpr auto eps = Sample(std::numeric_limits<float>::epsilon());
    alphaD = std::pow(eps, Sample(1) / decayTimeInSamples);
    alphaR = std::pow(eps, Sample(1) / releaseTimeInSamples);
  }

  void reset()
  {
    value = 0;
    alphaD = 0;
    alphaR = 0;
    offset = 0;
    state = State::release;
  }

  void trigger(Sample sustainLevel)
  {
    state = State::decay;
    value = Sample(1) - sustainLevel;
    offset = sustainLevel;
  }

  void release()
  {
    state = State::release;
    offset = 0;
  }

  Sample process()
  {
    if (state == State::decay) {
      value *= alphaD;
      return offset + value;
    }
    return value *= alphaR;
  }
};

template<typename Sample> class TransitionReleaseSmoother {
private:
  Sample v0 = 0;
  Sample decay = 0;

public:
  // decaySamples = sampleRate * seconds.
  void setup(Sample decaySamples)
  {
    decay = std::pow(std::numeric_limits<Sample>::epsilon(), Sample(1) / decaySamples);
  }

  void reset() { v0 = 0; }

  void prepare(Sample value, Sample decaySamples)
  {
    v0 += value;
    decay = std::pow(std::numeric_limits<Sample>::epsilon(), Sample(1) / decaySamples);
  }

  Sample process() { return v0 *= decay; }
};

template<typename Sample> class ExpADEnvelope {
private:
  static constexpr Sample epsilon = std::numeric_limits<Sample>::epsilon();
  Sample targetGain = 0;
  Sample velocity = 0;
  Sample gain = Sample(1);
  Sample smoo = Sample(1);
  Sample valueA = 0;
  Sample alphaA = 0;
  Sample valueD = 0;
  Sample alphaD = 0;

public:
  bool isTerminated() { return valueD <= Sample(1e-3); }

  void setup(Sample smoothingKp) { smoo = smoothingKp; }

  void reset()
  {
    targetGain = 0;
    gain = Sample(1);
    valueA = 0;
    alphaA = 0;
    valueD = 0;
    alphaD = 0;
  }

  enum class NormalizationType { peak, energy };

  void update(
    Sample sampleRate,
    Sample peakSeconds,
    Sample releaseSeconds,
    Sample peakGain,
    NormalizationType normalization = NormalizationType::energy)
  {
    const auto decaySeconds = releaseSeconds - std::log(epsilon) * peakSeconds;
    const auto d_ = std::log(epsilon) / decaySeconds;
    const auto x_ = d_ * peakSeconds;
    const auto a_ = Sample(utl::LambertW(-1, x_ * std::exp(x_))) / peakSeconds - d_;

    const auto attackSeconds = -std::log(epsilon) / std::log(-a_);
    alphaA = std::exp(a_ / sampleRate);
    alphaD = std::exp(d_ / sampleRate);

    if (normalization == NormalizationType::energy) {
      // `area` is obtained by solving `integrate((1-%e^(-a*t))*%e^(-d*t), t, 0, +inf);`.
      const auto area = -a_ / (d_ * (d_ + a_));
      targetGain = Sample(1e-1) * peakGain / area;
    } else { // `normalization == NormalizationType::peak`.
      targetGain
        = peakGain / (-std::expm1(a_ * peakSeconds) * std::exp(d_ * peakSeconds));
    }
  }

  void trigger(
    Sample sampleRate,
    Sample peakSeconds,
    Sample releaseSeconds,
    Sample peakGain,
    Sample velocity_)
  {
    velocity = velocity_;
    valueA = Sample(1);
    valueD = Sample(1);
    update(sampleRate, peakSeconds, releaseSeconds, peakGain);
  }

  Sample process()
  {
    gain += smoo * (targetGain - gain);
    valueA *= alphaA;
    valueD *= alphaD;
    return velocity * gain * (Sample(1) - (valueA)) * valueD;
  }
};

template<typename T> inline T lagrange3Interp(T y0, T y1, T y2, T y3, T t)
{
  auto u = T(1) + t;
  auto d0 = y0 - y1;
  auto d1 = d0 - (y1 - y2);
  auto d2 = d1 - ((y1 - y2) - (y2 - y3));
  return y0 - u * (d0 + (T(1) - u) / T(2) * (d1 + (T(2) - u) / T(3) * d2));
}

template<typename Sample> class Delay {
private:
  int wptr = 0;
  std::vector<Sample> buf{Sample(0), Sample(0)};

public:
  void setup(Sample maxTimeSamples)
  {
    buf.resize(std::max(size_t(4), size_t(maxTimeSamples) + 4));
    reset();
  }

  void reset() { std::fill(buf.begin(), buf.end(), Sample(0)); }

  void applyGain(Sample gain)
  {
    for (auto &x : buf) x *= gain;
  }

  Sample process(Sample input, Sample timeInSamples)
  {
    const int size = int(buf.size());
    const int clamped
      = std::clamp(timeInSamples - Sample(1), Sample(1), Sample(size - 4));
    const int timeInt = int(clamped);
    const Sample rFraction = clamped - Sample(timeInt);

    // Write to buffer.
    if (++wptr >= size) wptr = 0;
    buf[wptr] = input;

    // Read from buffer.
    auto rptr0 = wptr - timeInt;
    auto rptr1 = rptr0 - 1;
    auto rptr2 = rptr0 - 2;
    auto rptr3 = rptr0 - 3;
    if (rptr0 < 0) rptr0 += size;
    if (rptr1 < 0) rptr1 += size;
    if (rptr2 < 0) rptr2 += size;
    if (rptr3 < 0) rptr3 += size;
    return lagrange3Interp(buf[rptr0], buf[rptr1], buf[rptr2], buf[rptr3], rFraction);
  }
};

template<typename Sample> class Highpass2 {
private:
  // static constexpr Sample g = Sample(0.02);               // ~= tan(pi * 300 / 48000).
  static constexpr Sample k = Sample(0.7071067811865476); // 2 / sqrt(2).

  Sample ic1eq = 0;
  Sample ic2eq = 0;

public:
  void reset()
  {
    ic1eq = 0;
    ic2eq = 0;
  }

  Sample process(Sample input, Sample cutoffNormalized)
  {
    const auto g = std::tan(
      std::numbers::pi_v<Sample>
      * std::clamp(cutoffNormalized, Sample(0.00001), Sample(0.49998)));
    const auto v1 = (ic1eq + g * (input - ic2eq)) / (1 + g * (g + k));
    const auto v2 = ic2eq + g * v1;
    ic1eq = Sample(2) * v1 - ic1eq;
    ic2eq = Sample(2) * v2 - ic2eq;
    return input - k * v1 - v2;
  }
};

template<typename Sample> class EmaHighShelf {
private:
  Sample value = 0;

public:
  void reset() { value = 0; }

  Sample process(Sample input, Sample kp, Sample shelvingGain)
  {
    value += kp * (input - value);
    return std::lerp(value, input, shelvingGain);
  }
};

template<typename Sample> class EmaLowShelf {
private:
  Sample value = 0;

public:
  void reset() { value = 0; }

  Sample process(Sample input, Sample kp, Sample shelvingGain)
  {
    value += kp * (input - value);
    return std::lerp(input - value, input, shelvingGain);
  }
};

template<typename Sample> class AdaptiveNotchCPZ {
public:
  static constexpr Sample mu = Sample(2) / Sample(1024);
  Sample alpha = Sample(-2);

  Sample v1 = 0;
  Sample v2 = 0;

  void reset()
  {
    alpha = Sample(-2); // 0 Hz as initial guess.

    v1 = 0;
    v2 = 0;
  }

  Sample process(Sample input, Sample narrowness)
  {
    const auto a1 = narrowness * alpha;
    const auto a2 = narrowness * narrowness;
    auto gain = alpha >= 0 ? (Sample(1) + a1 + a2) / (Sample(2) + alpha)
                           : (Sample(1) - a1 + a2) / (Sample(2) - alpha);

    constexpr auto clip = Sample(1) / std::numeric_limits<Sample>::epsilon();
    const auto x0 = std::clamp(input, -clip, clip);
    auto v0 = x0 - a1 * v1 - a2 * v2;
    const auto y0 = v0 + alpha * v1 + v2;
    const auto s0
      = (Sample(1) - narrowness) * v0 - narrowness * (Sample(1) - narrowness) * v2;
    constexpr auto bound = Sample(2);
    alpha = std::clamp(alpha - y0 * s0 * mu, -bound, bound);

    v2 = v1;
    v1 = v0;

    return y0 * gain;
  }
};

template<typename Sample, size_t nAllpass, size_t nAdaptiveNotch> class SerialAllpass {
private:
  std::array<Sample, nAllpass> buffer{};
  std::array<Delay<Sample>, nAllpass> delay;
  std::array<EmaHighShelf<Sample>, nAllpass> lowpass;
  std::array<EmaLowShelf<Sample>, nAllpass> highpass;

public:
  std::array<AdaptiveNotchCPZ<Sample>, nAdaptiveNotch> notch;
  static constexpr size_t size = nAllpass;
  std::array<Sample, nAllpass> timeInSamples{};

  void setup(Sample maxTimeSamples)
  {
    for (auto &x : delay) x.setup(maxTimeSamples);
  }

  void reset()
  {
    buffer.fill({});
    for (auto &x : delay) x.reset();
    for (auto &x : lowpass) x.reset();
    for (auto &x : highpass) x.reset();
    for (auto &x : notch) x.reset();
  }

  void applyGain(Sample gain)
  {
    for (auto &x : delay) x.applyGain(gain);
  }

  Sample sum(Sample altSignMix)
  {
    Sample sumAlt = Sample(0);
    Sample sign = Sample(1);
    for (const auto &x : buffer) {
      sumAlt += x * sign;
      sign = -sign;
    }
    Sample sumDirect = std::accumulate(buffer.begin(), buffer.end(), Sample(0));
    return std::lerp(sumDirect, sumAlt, altSignMix) / (Sample(2) * nAllpass);
  }

  Sample process(
    Sample input,
    Sample highShelfCut,
    Sample highShelfGain,
    Sample lowShelfCut,
    Sample lowShelfGain,
    Sample gain,
    Sample pitchRatio,
    Sample timeModAmount,
    size_t nNotch,
    Sample notchMix,
    Sample notchNarrowness)
  {
    for (size_t idx = 0; idx < nAllpass; ++idx) {
      constexpr auto sign = 1;
      auto x0 = lowpass[idx].process(sign * input, highShelfCut, highShelfGain);
      // auto x0 = sign * input;
      x0 = highpass[idx].process(x0, lowShelfCut, lowShelfGain);
      x0 -= gain * buffer[idx];
      input = buffer[idx] + gain * x0;
      buffer[idx] = delay[idx].process(
        x0, timeInSamples[idx] / pitchRatio - timeModAmount * std::abs(x0));
    }

    // input = lowpass[0].process(input, highShelfCut, highShelfGain);

    for (size_t idx = 0; idx < nNotch; ++idx) {
      input += notchMix * (notch[idx].process(input, notchNarrowness) - input);
    }

    return input;
  }
};

template<typename Sample> class HalfClosedNoise {
private:
  Sample phase = 0;
  Sample gain = Sample(1);
  Sample decay = 0;

  Highpass2<Sample> highpass;

public:
  void reset()
  {
    phase = 0;
    gain = Sample(1);

    highpass.reset();
  }

  void setDecay(Sample timeInSample)
  {
    constexpr Sample eps = std::numeric_limits<Sample>::epsilon();
    decay = timeInSample < Sample(1) ? 0 : std::pow(eps, Sample(1) / timeInSample);
  }

  // `density` is inverse of average samples between impulses.
  // `randomGain` is in [0, 1].
  Sample process(Sample density, Sample randomGain, Sample highpassNormalized, pcg64 &rng)
  {
    std::uniform_real_distribution<Sample> jitter(Sample(0), Sample(1));
    phase += jitter(rng) * density;
    if (phase >= Sample(1)) {
      phase -= std::floor(phase);

      std::normal_distribution<Sample> distGain(Sample(0), Sample(1) / Sample(3));
      gain = Sample(1) + randomGain * (distGain(rng) - Sample(1));
    } else {
      gain *= decay;
    }

    std::uniform_real_distribution<Sample> distNoise(-Sample(1), Sample(1));
    const auto noise = distNoise(rng);
    return highpass.process(noise * noise * noise * gain, highpassNormalized);
  }
};

} // namespace SomeDSP
