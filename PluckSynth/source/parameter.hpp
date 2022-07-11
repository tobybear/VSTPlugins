// (c) 2022 Takamitsu Endo
//
// This file is part of PluckSynth.
//
// PluckSynth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// PluckSynth is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with PluckSynth.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../../common/dsp/constants.hpp"
#include "../../common/parameterInterface.hpp"

#ifdef TEST_DSP
  #include "../../test/value.hpp"
#else
  #include "../../common/value.hpp"
#endif

constexpr size_t maximumVoice = 16;
constexpr size_t oscOvertoneSize = 32;
constexpr size_t fdnMatrixSize = 8;
constexpr size_t nLfoWavetable = 64;

namespace Steinberg {
namespace Synth {

namespace ParameterID {
enum ID {
  bypass,

  octave,
  semitone,
  milli,
  equalTemperament,
  pitchA4Hz,
  pitchBend,

  gain,
  gateRelease,

  oscOvertone0,
  impulseGain = oscOvertone0 + oscOvertoneSize,
  oscGain,
  oscAttack,
  oscDecay,
  oscOctave,
  oscFinePitch,
  oscSpectrumDenominatorSlope,
  oscSpectrumRotationSlope,
  oscSpectrumRotationOffset,
  oscSpectrumInterval,
  oscSpectrumHighpass,
  oscSpectrumBlur,

  refreshWavetable,

  fdnEnable,
  fdnMatrixIdentityAmount,
  fdnFeedback,
  fdnOvertoneAdd,
  fdnOvertoneMul,
  fdnOvertoneOffset,
  // fdnOvertoneModulo, // TODO
  // fdnOvertoneRandomness, // TODO
  fdnInterpRate,
  fdnInterpLowpassSecond,
  fdnSeed,
  fdnFixedSeed,

  lowpassCutoffSemi,
  lowpassQ,
  lowpassKeyFollow,
  highpassCutoffSemi,
  highpassQ,
  highpassKeyFollow,

  nUnison,
  unisonDetune,
  unisonPan,

  lfoWavetable0,
  lfoInterpolation = lfoWavetable0 + nLfoWavetable,
  lfoTempoSync,
  lfoTempoUpper,
  lfoTempoLower,
  lfoRate,
  lfoRetrigger,

  lfoToOscPitchAmount,
  lfoToFdnPitchAmount,
  lfoToOscPitchAlignment,
  lfoToFdnPitchAlignment,

  ID_ENUM_LENGTH,
};
} // namespace ParameterID

struct Scales {
  static SomeDSP::UIntScale<double> boolScale;
  static SomeDSP::LinearScale<double> defaultScale;
  static SomeDSP::UIntScale<double> seed;

  static SomeDSP::UIntScale<double> octave;
  static SomeDSP::UIntScale<double> semitone;
  static SomeDSP::UIntScale<double> milli;
  static SomeDSP::UIntScale<double> equalTemperament;
  static SomeDSP::UIntScale<double> pitchA4Hz;

  static SomeDSP::DecibelScale<double> gain;
  static SomeDSP::DecibelScale<double> gateRelease;

  static SomeDSP::LinearScale<double> oscOvertone;
  static SomeDSP::DecibelScale<double> oscAttack;
  static SomeDSP::DecibelScale<double> oscDecay;
  static SomeDSP::UIntScale<double> oscOctave;
  static SomeDSP::LinearScale<double> oscFinePitch;
  static SomeDSP::DecibelScale<double> oscSpectrumDenominatorSlope;
  static SomeDSP::LinearScale<double> oscSpectrumRotationSlope;
  static SomeDSP::UIntScale<double> oscSpectrumInterval;
  static SomeDSP::UIntScale<double> oscSpectrumHighpass;
  static SomeDSP::DecibelScale<double> oscSpectrumBlur;

  static SomeDSP::DecibelScale<double> impulseGain;

  static SomeDSP::DecibelScale<double> fdnMatrixIdentityAmount;
  static SomeDSP::LinearScale<double> fdnOvertoneAdd;
  static SomeDSP::LinearScale<double> fdnOvertoneMul;
  static SomeDSP::LinearScale<double> fdnOvertoneOffset;
  static SomeDSP::DecibelScale<double> fdnInterpRate;
  static SomeDSP::DecibelScale<double> fdnInterpLowpassSecond;

  static SomeDSP::LinearScale<double> filterCutoffSemi;
  static SomeDSP::LinearScale<double> filterQ;

  static SomeDSP::UIntScale<double> nUnison;
  static SomeDSP::DecibelScale<double> unisonDetune;
  static SomeDSP::LinearScale<double> unisonPan;

  static SomeDSP::LinearScale<double> lfoWavetable;
  static SomeDSP::UIntScale<double> lfoInterpolation;
  static SomeDSP::UIntScale<double> lfoTempoUpper;
  static SomeDSP::UIntScale<double> lfoTempoLower;
  static SomeDSP::DecibelScale<double> lfoRate;
  static SomeDSP::LinearScale<double> lfoToPitchAmount;
  static SomeDSP::UIntScale<double> lfoToPitchAlignment;
};

struct GlobalParameter : public ParameterInterface {
  std::vector<std::unique_ptr<ValueInterface>> value;

  GlobalParameter()
  {
    value.resize(ParameterID::ID_ENUM_LENGTH);

    using Info = Vst::ParameterInfo;
    using ID = ParameterID::ID;
    using LinearValue = DoubleValue<SomeDSP::LinearScale<double>>;
    using LogValue = DoubleValue<SomeDSP::LogScale<double>>;
    using DecibelValue = DoubleValue<SomeDSP::DecibelScale<double>>;
    using SemitoneValue = DoubleValue<SomeDSP::SemitoneScale<double>>;

    value[ID::bypass] = std::make_unique<UIntValue>(
      false, Scales::boolScale, "bypass", Info::kCanAutomate | Info::kIsBypass);

    value[ID::octave]
      = std::make_unique<UIntValue>(12, Scales::octave, "octave", Info::kCanAutomate);
    value[ID::semitone] = std::make_unique<UIntValue>(
      120, Scales::semitone, "semitone", Info::kCanAutomate);
    value[ID::milli]
      = std::make_unique<UIntValue>(1000, Scales::milli, "milli", Info::kCanAutomate);
    value[ID::equalTemperament] = std::make_unique<UIntValue>(
      11, Scales::equalTemperament, "equalTemperament", Info::kCanAutomate);
    value[ID::pitchA4Hz] = std::make_unique<UIntValue>(
      340, Scales::pitchA4Hz, "pitchA4Hz", Info::kCanAutomate);
    value[ID::pitchBend] = std::make_unique<LinearValue>(
      0.5, Scales::defaultScale, "pitchBend", Info::kCanAutomate);

    value[ID::gain] = std::make_unique<DecibelValue>(
      Scales::gain.invmapDB(0.0), Scales::gain, "gain", Info::kCanAutomate);
    value[ID::gateRelease] = std::make_unique<DecibelValue>(
      Scales::gateRelease.invmap(0.01), Scales::gateRelease, "gateRelease",
      Info::kCanAutomate);

    std::string oscOvertoneLabel("oscOvertone");
    for (size_t idx = 0; idx < oscOvertoneSize; ++idx) {
      auto indexStr = std::to_string(idx);
      value[ID::oscOvertone0 + idx] = std::make_unique<LinearValue>(
        Scales::oscOvertone.invmap(idx == 0 ? 1.0 : 0.0), Scales::oscOvertone,
        (oscOvertoneLabel + indexStr).c_str(), Info::kCanAutomate);
    }
    value[ID::impulseGain] = std::make_unique<DecibelValue>(
      0.0, Scales::impulseGain, "impulseGain", Info::kCanAutomate);
    value[ID::oscGain] = std::make_unique<DecibelValue>(
      Scales::impulseGain.invmapDB(0.0), Scales::impulseGain, "oscGain",
      Info::kCanAutomate);
    value[ID::oscAttack] = std::make_unique<DecibelValue>(
      Scales::oscAttack.invmap(0.001), Scales::oscAttack, "oscAttack",
      Info::kCanAutomate);
    value[ID::oscDecay] = std::make_unique<DecibelValue>(
      Scales::oscDecay.invmap(1.0), Scales::oscDecay, "oscDecay", Info::kCanAutomate);
    value[ID::oscOctave] = std::make_unique<UIntValue>(
      12, Scales::oscOctave, "oscOctave", Info::kCanAutomate);
    value[ID::oscFinePitch] = std::make_unique<LinearValue>(
      Scales::oscFinePitch.invmap(0.0), Scales::oscFinePitch, "oscFinePitch",
      Info::kCanAutomate);
    value[ID::oscSpectrumDenominatorSlope] = std::make_unique<DecibelValue>(
      Scales::oscSpectrumDenominatorSlope.invmap(1.0),
      Scales::oscSpectrumDenominatorSlope, "oscSpectrumDenominatorSlope",
      Info::kCanAutomate);
    value[ID::oscSpectrumRotationSlope] = std::make_unique<LinearValue>(
      Scales::oscSpectrumRotationSlope.invmap(0.0), Scales::oscSpectrumRotationSlope,
      "oscSpectrumRotationSlope", Info::kCanAutomate);
    value[ID::oscSpectrumRotationOffset] = std::make_unique<LinearValue>(
      0.25, Scales::defaultScale, "oscSpectrumRotationOffset", Info::kCanAutomate);
    value[ID::oscSpectrumInterval] = std::make_unique<UIntValue>(
      0, Scales::oscSpectrumInterval, "oscSpectrumInterval", Info::kCanAutomate);
    value[ID::oscSpectrumHighpass] = std::make_unique<UIntValue>(
      0, Scales::oscSpectrumHighpass, "oscSpectrumHighpass", Info::kCanAutomate);
    value[ID::oscSpectrumBlur] = std::make_unique<DecibelValue>(
      Scales::oscSpectrumBlur.invmap(1.0), Scales::oscSpectrumBlur, "oscSpectrumBlur",
      Info::kCanAutomate);

    value[ID::refreshWavetable] = std::make_unique<UIntValue>(
      0, Scales::boolScale, "refreshWavetable", Info::kCanAutomate);

    value[ID::fdnEnable] = std::make_unique<UIntValue>(
      true, Scales::boolScale, "fdnEnable", Info::kCanAutomate);
    value[ID::fdnMatrixIdentityAmount] = std::make_unique<DecibelValue>(
      Scales::fdnMatrixIdentityAmount.invmap(0.1), Scales::fdnMatrixIdentityAmount,
      "fdnMatrixIdentityAmount", Info::kCanAutomate);
    value[ID::fdnFeedback] = std::make_unique<LinearValue>(
      1.0, Scales::defaultScale, "fdnFeedback", Info::kCanAutomate);
    value[ID::fdnOvertoneAdd] = std::make_unique<LinearValue>(
      Scales::fdnOvertoneAdd.invmap(1.0), Scales::fdnOvertoneAdd, "fdnOvertoneAdd",
      Info::kCanAutomate);
    value[ID::fdnOvertoneMul] = std::make_unique<LinearValue>(
      Scales::fdnOvertoneMul.invmap(1.0), Scales::fdnOvertoneMul, "fdnOvertoneMul",
      Info::kCanAutomate);
    value[ID::fdnOvertoneOffset] = std::make_unique<LinearValue>(
      Scales::fdnOvertoneOffset.invmap(0.0), Scales::fdnOvertoneOffset,
      "fdnOvertoneOffset", Info::kCanAutomate);
    value[ID::fdnInterpRate] = std::make_unique<DecibelValue>(
      Scales::fdnInterpRate.invmapDB(0.0), Scales::fdnInterpRate, "fdnInterpRate",
      Info::kCanAutomate);
    value[ID::fdnInterpLowpassSecond] = std::make_unique<DecibelValue>(
      Scales::fdnInterpLowpassSecond.invmap(0.005), Scales::fdnInterpLowpassSecond,
      "fdnInterpLowpassSecond", Info::kCanAutomate);
    value[ID::fdnSeed]
      = std::make_unique<UIntValue>(0, Scales::seed, "fdnSeed", Info::kCanAutomate);
    value[ID::fdnFixedSeed] = std::make_unique<UIntValue>(
      false, Scales::boolScale, "fdnFixedSeed", Info::kCanAutomate);

    value[ID::lowpassCutoffSemi] = std::make_unique<LinearValue>(
      1.0, Scales::filterCutoffSemi, "lowpassCutoffSemi", Info::kCanAutomate);
    value[ID::lowpassQ] = std::make_unique<LinearValue>(
      1.0, Scales::filterQ, "lowpassQ", Info::kCanAutomate);
    value[ID::lowpassKeyFollow] = std::make_unique<UIntValue>(
      true, Scales::boolScale, "lowpassKeyFollow", Info::kCanAutomate);

    value[ID::highpassCutoffSemi] = std::make_unique<LinearValue>(
      Scales::filterCutoffSemi.invmap(-12.0), Scales::filterCutoffSemi,
      "highpassCutoffSemi", Info::kCanAutomate);
    value[ID::highpassQ] = std::make_unique<LinearValue>(
      1.0, Scales::filterQ, "highpassQ", Info::kCanAutomate);
    value[ID::highpassKeyFollow] = std::make_unique<UIntValue>(
      true, Scales::boolScale, "highpassKeyFollow", Info::kCanAutomate);

    value[ID::nUnison]
      = std::make_unique<UIntValue>(0, Scales::nUnison, "nUnison", Info::kCanAutomate);
    value[ID::unisonDetune] = std::make_unique<DecibelValue>(
      Scales::unisonDetune.invmap(std::pow(10.0, 1.0 / 1200.0)), Scales::unisonDetune,
      "unisonDetune", Info::kCanAutomate);
    value[ID::unisonPan] = std::make_unique<LinearValue>(
      1.0, Scales::unisonPan, "unisonPan", Info::kCanAutomate);

    std::string lfoWavetableLabel("lfoWavetable");
    for (size_t idx = 0; idx < nLfoWavetable; ++idx) {
      auto indexStr = std::to_string(idx);
      value[ID::lfoWavetable0 + idx] = std::make_unique<LinearValue>(
        Scales::lfoWavetable.invmap(sin(SomeDSP::twopi * idx / double(nLfoWavetable))),
        Scales::lfoWavetable, (lfoWavetableLabel + indexStr).c_str(), Info::kCanAutomate);
    }
    value[ID::lfoTempoSync] = std::make_unique<UIntValue>(
      0, Scales::boolScale, "lfoTempoSync", Info::kCanAutomate);
    value[ID::lfoInterpolation] = std::make_unique<UIntValue>(
      2, Scales::lfoInterpolation, "lfoInterpolation", Info::kCanAutomate);
    value[ID::lfoTempoUpper] = std::make_unique<UIntValue>(
      0, Scales::lfoTempoUpper, "lfoTempoUpper", Info::kCanAutomate);
    value[ID::lfoTempoLower] = std::make_unique<UIntValue>(
      0, Scales::lfoTempoLower, "lfoTempoLower", Info::kCanAutomate);
    value[ID::lfoRate] = std::make_unique<DecibelValue>(
      Scales::lfoRate.invmap(1.0), Scales::lfoRate, "lfoRate", Info::kCanAutomate);
    value[ID::lfoRetrigger] = std::make_unique<UIntValue>(
      true, Scales::boolScale, "lfoRetrigger", Info::kCanAutomate);

    value[ID::lfoToOscPitchAmount] = std::make_unique<LinearValue>(
      Scales::lfoToPitchAmount.invmap(0.0), Scales::lfoToPitchAmount,
      "lfoToOscPitchAmount", Info::kCanAutomate);
    value[ID::lfoToFdnPitchAmount] = std::make_unique<LinearValue>(
      Scales::lfoToPitchAmount.invmap(0.0), Scales::lfoToPitchAmount,
      "lfoToFdnPitchAmount", Info::kCanAutomate);

    value[ID::lfoToOscPitchAlignment] = std::make_unique<UIntValue>(
      0, Scales::lfoToPitchAlignment, "lfoToOscPitchAlignment", Info::kCanAutomate);
    value[ID::lfoToFdnPitchAlignment] = std::make_unique<UIntValue>(
      0, Scales::lfoToPitchAlignment, "lfoToFdnPitchAlignment", Info::kCanAutomate);

    for (size_t id = 0; id < value.size(); ++id) value[id]->setId(Vst::ParamID(id));
  }

#ifdef TEST_DSP
  // Not used in DSP test.
  double getDefaultNormalized(int32_t) { return 0.0; }

#else
  tresult setState(IBStream *stream)
  {
    IBStreamer streamer(stream, kLittleEndian);
    for (auto &val : value)
      if (val->setState(streamer)) return kResultFalse;
    return kResultOk;
  }

  tresult getState(IBStream *stream)
  {
    IBStreamer streamer(stream, kLittleEndian);
    for (auto &val : value)
      if (val->getState(streamer)) return kResultFalse;
    return kResultOk;
  }

  tresult addParameter(Vst::ParameterContainer &parameters)
  {
    for (auto &val : value)
      if (val->addParameter(parameters)) return kResultFalse;
    return kResultOk;
  }

  double getDefaultNormalized(int32_t tag) override
  {
    if (size_t(abs(tag)) >= value.size()) return 0.0;
    return value[tag]->getDefaultNormalized();
  }
#endif
};

} // namespace Synth
} // namespace Steinberg
