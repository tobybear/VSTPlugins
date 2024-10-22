// SPDX-License-Identifier: GPL-3.0-only
// Copyright Takamitsu Endo (ryukau@gmail.com)

#define SET_PARAMETERS dsp->setParameters();
#define HAS_INPUT 1
#define NO_DSP_INTERFACE

#include "../../test/synthtester.hpp"
#include "../source/dsp/dspcore.hpp"

// CMake provides this macro, but just in case.
#ifndef UHHYOU_PLUGIN_NAME
  #define UHHYOU_PLUGIN_NAME "DoubleLoopCymbal"
#endif

#define OUT_DIR_PATH "snd/" UHHYOU_PLUGIN_NAME

int main()
{
  SynthTester<DSPCore> tester(UHHYOU_PLUGIN_NAME, OUT_DIR_PATH, 1);
  return tester.isFinished ? EXIT_SUCCESS : EXIT_FAILURE;
}
