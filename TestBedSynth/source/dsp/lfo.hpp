// (c) 2023 Takamitsu Endo
//
// This file is part of Uhhyou Plugins.
//
// Uhhyou Plugins is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Uhhyou Plugins is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Uhhyou Plugins.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include "../../../common/dsp/smoother.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

namespace SomeDSP {

template<typename Sample> struct EMAHighpass {
  Sample v1 = 0;

  void reset(Sample value = 0) { v1 = value; }

  Sample process(Sample input, Sample kp)
  {
    v1 += kp * (input - v1);
    return input - v1;
  }
};

template<typename Sample, size_t tableSize> class EasyLfo {
private:
  Sample phase = 0;

public:
  DoubleEMAFilter<Sample> smoother;

  void reset()
  {
    phase = 0;
    smoother.reset();
  }

  Sample process(
    Sample phaseDelta, Sample smootherKp, const std::array<Sample, tableSize> &table)
  {
    phase += phaseDelta;
    phase -= std::floor(phase);
    return smoother.processKp(table[size_t(Sample(tableSize) * phase)], smootherKp);
  }
};

template<typename Sample> class ADSREnvelope {
public:
  enum class State { attack, decay, release, terminated };

  State state = State::terminated;
  size_t attackLength = 0;
  size_t counter = 0;
  Sample atkKp = 1;
  Sample decKp = 1;
  Sample relKp = 1;
  DoubleEMAFilter<Sample> smoother;

  Sample value() { return smoother.v2; }

  bool isAttacking() { return state == State::attack; }
  bool isTerminated() { return state == State::terminated; }

  void reset()
  {
    state = State::terminated;
    smoother.reset();
  }

  void noteOn(size_t attackSamples)
  {
    state = State::attack;
    attackLength = attackSamples;
    counter = 0;
    smoother.reset();
  }

  void noteOff() { state = State::release; }

  void prepare(Sample attackKp, Sample decayKp, Sample releaseKp)
  {
    atkKp = attackKp;
    decKp = decayKp;
    relKp = releaseKp;
  }

  Sample process(Sample sustainAmplitude)
  {
    if (state == State::attack) {
      if (++counter >= attackLength) state = State::decay;
      const auto ramp = Sample(counter) / Sample(attackLength / 2);
      return smoother.processKp(std::min(ramp, Sample(1)), atkKp);
    } else if (state == State::decay) {
      return smoother.processKp(sustainAmplitude, decKp);
    } else if (state == State::release) {
      constexpr Sample eps = Sample(std::numeric_limits<float>::epsilon());
      if (smoother.v2 <= eps) {
        state = State::terminated;
        smoother.reset();
      }
      return smoother.processKp(Sample(0), relKp);
    }
    return 0;
  }
};

} // namespace SomeDSP
