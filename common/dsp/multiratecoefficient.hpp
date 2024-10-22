// SPDX-License-Identifier: GPL-3.0-only
// Copyright Takamitsu Endo (ryukau@gmail.com)

#pragma once

#include <array>

namespace SomeDSP {

/**
Lowpass filter coefficient specialized for 64x oversampling.
Sos stands for second order sections.

```python
import numpy
from scipy import signal

samplerate = 2 * 48000
uprate = samplerate * 32
sos = signal.butter(16, samplerate / 4, output="sos", fs=uprate)
```
*/
template<typename Sample> struct Sos64FoldFirstStage {
  constexpr static size_t upfold = 64;
  static constexpr size_t fold = 32;

  constexpr static std::array<std::array<Sample, 5>, 8> co{{
    {Sample(1.354163914584143e-26), Sample(2.708327829168286e-26),
     Sample(1.354163914584143e-26), Sample(-1.9045872504279573),
     Sample(0.9068841759295282)},
    {Sample(1.0), Sample(2.0), Sample(1.0), Sample(-1.908001035290007),
     Sample(0.9103020778040721)},
    {Sample(1.0), Sample(2.0), Sample(1.0), Sample(-1.9147330871451047),
     Sample(0.9170422484899456)},
    {Sample(1.0), Sample(2.0), Sample(1.0), Sample(-1.9245914935233015),
     Sample(0.9269125440714382)},
    {Sample(1.0), Sample(2.0), Sample(1.0), Sample(-1.9372866598709455),
     Sample(0.9396230207448886)},
    {Sample(1.0), Sample(2.0), Sample(1.0), Sample(-1.9524305274354947),
     Sample(0.9547851517602688)},
    {Sample(1.0), Sample(2.0), Sample(1.0), Sample(-1.9695376181976627),
     Sample(0.9719128736135145)},
    {Sample(1.0), Sample(2.0), Sample(1.0), Sample(-1.9880295377862067),
     Sample(0.9904270943918131)},
  }};
};

/**
Lowpass filter coefficient specialized for 16x oversampling.
Sos stands for second order sections.

```python
import numpy
from scipy import signal

samplerate = 48000
uprate = samplerate * 16 / 2
sos = signal.butter(16, samplerate / 1.8, output="sos", fs=uprate)
```
*/
template<typename Sample> struct Sos16FoldFirstStage {
  constexpr static size_t upfold = 16;
  constexpr static size_t fold = 8;

  constexpr static std::array<std::array<Sample, 5>, 8> co{{
    {Sample(3.5903469155931847e-12), Sample(7.1806938311863695e-12),
     Sample(3.5903469155931847e-12), Sample(-1.2759657610561284),
     Sample(0.40787244610150275)},
    {Sample(1.0), Sample(2.0), Sample(1.0), Sample(-1.2906502176887378),
     Sample(0.42407495130188644)},
    {Sample(1.0), Sample(2.0), Sample(1.0), Sample(-1.320459244427636),
     Sample(0.456965573191349)},
    {Sample(1.0), Sample(2.0), Sample(1.0), Sample(-1.3662708320207162),
     Sample(0.5075130673741699)},
    {Sample(1.0), Sample(2.0), Sample(1.0), Sample(-1.429387848302023),
     Sample(0.5771549894497601)},
    {Sample(1.0), Sample(2.0), Sample(1.0), Sample(-1.5114943545116066),
     Sample(0.6677494954045713)},
    {Sample(1.0), Sample(2.0), Sample(1.0), Sample(-1.6145439579130596),
     Sample(0.7814521523555764)},
    {Sample(1.0), Sample(2.0), Sample(1.0), Sample(-1.7405167001403739),
     Sample(0.9204476945203488)},
  }};
};

/**
Lowpass filter coefficient specialized for 8x oversampling.
Sos stands for second order sections.

```python
import numpy
from scipy import signal

samplerate = 48000
uprate = samplerate * 8 / 2
sos = signal.butter(10, samplerate / 1.9, output="sos", fs=uprate)
```
*/
template<typename Sample> struct Sos8FoldFirstStage {
  constexpr static size_t upfold = 8;
  constexpr static size_t fold = 4;

  constexpr static std::array<std::array<Sample, 5>, 5> co{{
    {Sample(1.6921576928941614e-05), Sample(3.384315385788323e-05),
     Sample(1.6921576928941614e-05), Sample(-0.7844963643040177),
     Sample(0.1583016535071512)},
    {Sample(1.0), Sample(2.0), Sample(1.0), Sample(-0.818202758191658),
     Sample(0.20806883350989597)},
    {Sample(1.0), Sample(2.0), Sample(1.0), Sample(-0.8910219912408522),
     Sample(0.31558576014706946)},
    {Sample(1.0), Sample(2.0), Sample(1.0), Sample(-1.0154057703544481),
     Sample(0.49923726393009143)},
    {Sample(1.0), Sample(2.0), Sample(1.0), Sample(-1.2147539782254588),
     Sample(0.7935730560475367)},
  }};
};

template<typename T> struct HalfBandCoefficient {
  static constexpr std::array<T, 9> h0_a{
    T(0.0765690656031399), T(0.264282270318935),  T(0.47939467893641907),
    T(0.661681722389424),  T(0.7924031566294969), T(0.8776927911111817),
    T(0.9308500986629166), T(0.9640156636878193), T(0.9862978287283355),
  };
  static constexpr std::array<T, 10> h1_a{
    T(0.019911761024506557), T(0.16170648261075027), T(0.37320978687920564),
    T(0.5766558985008232),   T(0.7334355636406803),  T(0.8399227128761151),
    T(0.9074601780285125),   T(0.9492937701934973),  T(0.9760539731706528),
    T(0.9955323321150525),
  };
};

/**
Polyphase FIR coefficients for 16 fold upsampler.

```python
import numpy as np
import scipy.signal as signal
nTaps = 32
nPhase = 16
fir = signal.remez(nTaps * nPhase - 1, [0, 18000, 28000, 8 * 48000], [1, 0], [1, 10],
                    fs=16 * 48000,
                    maxiter=256)
fir = np.hstack((fir, [0]))
poly = nPhase * fir.reshape((nTaps, nPhase)).T[::-1]  # Upsampler
poly = [p[::-1] for p in poly]
```
*/
template<typename Sample> struct Fir16FoldUpSample {
  constexpr static size_t bufferSize = 32;
  constexpr static size_t intDelay = 15;
  constexpr static size_t upfold = 16;

  constexpr static std::array<std::array<Sample, bufferSize>, upfold> coefficient{{
    {
      Sample(0.000000000000000),      Sample(4.710049084045554e-06),
      Sample(-8.91767480313468e-05),  Sample(0.0003318665036976312),
      Sample(-0.0009029355190602759), Sample(0.0020103130118938567),
      Sample(-0.0039108232199485116), Sample(0.006846006452087558),
      Sample(-0.010994067895928607),  Sample(0.016394932272087707),
      Sample(-0.022899510563565026),  Sample(0.030139906074162965),
      Sample(-0.03754763652461209),   Sample(0.04441586552368129),
      Sample(-0.05000320079278829),   Sample(0.0536563581385643),
      Sample(0.9450727576747361),     Sample(0.0536563581385643),
      Sample(-0.05000320079278829),   Sample(0.04441586552368129),
      Sample(-0.03754763652461209),   Sample(0.030139906074162965),
      Sample(-0.022899510563565026),  Sample(0.016394932272087707),
      Sample(-0.010994067895928607),  Sample(0.006846006452087558),
      Sample(-0.0039108232199485116), Sample(0.0020103130118938567),
      Sample(-0.0009029355190602759), Sample(0.0003318665036976312),
      Sample(-8.91767480313468e-05),  Sample(4.710049084045554e-06),
    },
    {
      Sample(1.3479673515322202e-05),  Sample(-6.653674007204556e-06),
      Sample(-5.84634321592889e-05),   Sample(0.00026964783145397704),
      Sample(-0.0008022588019898115),  Sample(0.0018843883768186523),
      Sample(-0.0038153560241077082),  Sample(0.00691037004319183),
      Sample(-0.011459536884283111),   Sample(0.017660810041711397),
      Sample(-0.025582816588426923),   Sample(0.03517255991060761),
      Sample(-0.04638792220871797),    Sample(0.05964357277159895),
      Sample(-0.07753958159867064),    Sample(0.11708115241647543),
      Sample(0.9395895134500334),      Sample(-0.0037966492050887394),
      Sample(-0.022699964595321732),   Sample(0.0285134132009406),
      Sample(-0.02786788710334668),    Sample(0.024325373332777903),
      Sample(-0.01957014646036561),    Sample(0.014637502328099185),
      Sample(-0.010180768986647017),   Sample(0.006552635892362446),
      Sample(-0.0038666724001800218),  Sample(0.002058135209493373),
      Sample(-0.0009641081958538801),  Sample(0.0003764277936461807),
      Sample(-0.00011317194812522876), Sample(1.4049313350164142e-05),
    },
    {
      Sample(9.703922934794865e-06),   Sample(-1.9890198537748112e-05),
      Sample(-2.1383395038703052e-05), Sample(0.00019025938799197717),
      Sample(-0.0006622032572619759),  Sample(0.0016786776428574547),
      Sample(-0.003573646795731787),   Sample(0.00672839027491833),
      Sample(-0.011539519582625996),   Sample(0.0183622904540556),
      Sample(-0.027489998096875804),   Sample(0.039203780525624676),
      Sample(-0.05403100851892882),    Sample(0.07362137405852807),
      Sample(-0.10438155546098346),    Sample(0.18551434376573492),
      Sample(0.9232561387862673),      Sample(-0.054487465007320435),
      Sample(0.0035108812320078318),   Sample(0.012502761908825356),
      Sample(-0.01771718325712137),    Sample(0.01796460617158813),
      Sample(-0.015740529859447194),   Sample(0.012474509419633528),
      Sample(-0.009067221953740245),   Sample(0.00605449217580668),
      Sample(-0.003693954931272076),   Sample(0.002032162296534101),
      Sample(-0.0009870688640381503),  Sample(0.0004035285897779825),
      Sample(-0.00013040247649419103), Sample(2.133090888007697e-05),
    },
    {
      Sample(1.2763936533040183e-05),  Sample(-3.474199150373041e-05),
      Sample(2.140709424007283e-05),   Sample(9.49180551186065e-05),
      Sample(-0.00048443203467528074), Sample(0.0013943899741018255),
      Sample(-0.003184069405785382),   Sample(0.006290717688516446),
      Sample(-0.011208255473759107),   Sample(0.01844304744889957),
      Sample(-0.028512242333653397),   Sample(0.042038744258082315),
      Sample(-0.06014302984956069),    Sample(0.08578448117448852),
      Sample(-0.1295568905547041),     Sample(0.2578388742557367),
      Sample(0.8964187216080176),      Sample(-0.09781300376626244),
      Sample(0.02785910408164383),     Sample(-0.00307597160129911),
      Sample(-0.007461964682316679),   Sample(0.011300546124782195),
      Sample(-0.011566417664852165),   Sample(0.010001420565947525),
      Sample(-0.0077087147406803246),  Sample(0.005381433118070637),
      Sample(-0.0034074610587552516),  Sample(0.0019389630887573213),
      Sample(-0.0009743540932177976),  Sample(0.0004139832051857727),
      Sample(-0.00014106125576820053), Sample(2.6600342651664725e-05),
    },
    {
      Sample(1.607947200193808e-05),   Sample(-5.0827311082251037e-05),
      Sample(6.891961867747201e-05),   Sample(-1.4371602956711715e-05),
      Sample(-0.0002722217177740313),  Sample(0.0010358153845066098),
      Sample(-0.0026504110673602433),  Sample(0.005596853827197398),
      Sample(-0.010453529597007261),   Sample(0.017866229148661015),
      Sample(-0.028567008943939227),   Sample(0.04351554690228815),
      Sample(-0.064426888927973),      Sample(0.09559808139598455),
      Sample(-0.1520746007365989),     Sample(0.3328078777833169),
      Sample(0.8596442052557635),      Sample(-0.13336654402797954),
      Sample(0.04968078004461383),     Sample(-0.017725215650396725),
      Sample(0.002545582095007783),    Sample(0.004574804173132053),
      Sample(-0.007207664902493617),   Sample(0.0073193777259110675),
      Sample(-0.006165881629247218),   Sample(0.004567548873821048),
      Sample(-0.0030249461748891896),  Sample(0.0017869784165739058),
      Sample(-0.0009295513963859288),  Sample(0.00040912695452287334),
      Sample(-0.00014559182148270598), Sample(2.9983659311536116e-05),
    },
    {
      Sample(1.951819551734623e-05),  Sample(-6.766962245519853e-05),
      Sample(0.00011985100122193777), Sample(-0.00013485862543473318),
      Sample(-3.047720515685937e-05), Sample(0.0006104022458092286),
      Sample(-0.001982110165895166),  Sample(0.004655733540001486),
      Sample(-0.009277911666460817),  Sample(0.016616853117648735),
      Sample(-0.027602354221803443),  Sample(0.04351260778784133),
      Sample(-0.0666345089436824),    Sample(0.10257693614017332),
      Sample(-0.1709524804335472),    Sample(0.4090724884044057),
      Sample(0.813705875381671),      Sample(-0.16094135179759603),
      Sample(0.06843178056277632),    Sample(-0.03100394330656437),
      Sample(0.01197866312769322),    Sample(-0.0019805202789144947),
      Sample(-0.0028225133836233983), Sample(0.00453138960164905),
      Sample(-0.004502284978844274),  Sample(0.00364969957798612),
      Sample(-0.0025662981598160947), Sample(0.0015860613377086748),
      Sample(-0.0008570751580271142), Sample(0.0003907108219219258),
      Sample(-0.000144572025311346),  Sample(3.165559755790354e-05),
    },
    {
      Sample(2.292027423928119e-05),  Sample(-8.465915842685608e-05),
      Sample(0.00017258765442065211), Sample(-0.0002630472283497376),
      Sample(0.00023431075928766928), Sample(0.00012871926088374428),
      Sample(-0.0011943019636486416), Sample(0.0034859905935713358),
      Sample(-0.00769951717580503),   Sample(0.01470353025812795),
      Sample(-0.025600360746098044),  Sample(0.041955008246504215),
      Sample(-0.06657797929998237),   Sample(0.1063043461184521),
      Sample(-0.18524547762799762),   Sample(0.48521289578202287),
      Sample(0.759563625581294),      Sample(-0.18052889044597611),
      Sample(0.08369719898118388),    Sample(-0.042539184456460516),
      Sample(0.020545654418939463),   Sample(-0.008150310454753227),
      Sample(0.0014377816246666129),  Sample(0.0017386595750233202),
      Sample(-0.0027820259743428642), Sample(0.002666053429565046),
      Sample(-0.0020526860419967153), Sample(0.0013470268720799255),
      Sample(-0.0007619404606755493), Sample(0.0003608018837120524),
      Sample(-0.0001387749559101323), Sample(3.184514794042204e-05),
    },
    {
      Sample(2.608615624166069e-05),   Sample(-0.00010108397935197923),
      Sample(0.00022524339213407982),  Sample(-0.00039477665215476487),
      Sample(0.0005141854613472223),   Sample(-0.0003957045458875576),
      Sample(-0.00030763689109161615), Sample(0.0021158727942215106),
      Sample(-0.005752239740468992),   Sample(0.012159411908367439),
      Sample(-0.022579514226450468),   Sample(0.038819470973703155),
      Sample(-0.06413916878131061),    Sample(0.10644983846168542),
      Sample(-0.19407409197259143),    Sample(0.5597718257957642),
      Sample(0.6983395497678168),      Sample(-0.19231180108293305),
      Sample(0.09519657968861324),     Sample(-0.052034807030786126),
      Sample(0.027998245212399724),    Sample(-0.013742975919210006),
      Sample(0.005433823805004525),    Sample(-0.0009628364544560138),
      Sample(-0.001067478223047236),   Sample(0.0016546565841673655),
      Sample(-0.0015057230584022971),  Sample(0.0010811877546244523),
      Sample(-0.0006495193031586813),  Sample(0.0003216749071594291),
      Sample(-0.0001290049718072652),  Sample(3.080222524738068e-05),
    },
    {
      Sample(2.879679065150001e-05),   Sample(-0.00011613795493211648),
      Sample(0.00027569918575506526),  Sample(-0.0005253224928600103),
      Sample(0.0007999047715219641),   Sample(-0.0009466701760656395),
      Sample(0.0006521095174147799),   Sample(0.0005827960461315251),
      Sample(-0.0034854039362733225),  Sample(0.009042295451563999),
      Sample(-0.01859587826215821),    Sample(0.034137752240822515),
      Sample(-0.05927739504866232),    Sample(0.10278495906091381),
      Sample(-0.19665196099551444),    Sample(0.631289561605983),
      Sample(0.631289561605983),       Sample(-0.19665196099551444),
      Sample(0.10278495906091381),     Sample(-0.05927739504866232),
      Sample(0.034137752240822515),    Sample(-0.01859587826215821),
      Sample(0.009042295451563999),    Sample(-0.0034854039362733225),
      Sample(0.0005827960461315251),   Sample(0.0006521095174147799),
      Sample(-0.0009466701760656395),  Sample(0.0007999047715219641),
      Sample(-0.0005253224928600103),  Sample(0.00027569918575506526),
      Sample(-0.00011613795493211648), Sample(2.879679065150001e-05),
    },
    {
      Sample(3.080222524738068e-05),   Sample(-0.0001290049718072652),
      Sample(0.0003216749071594291),   Sample(-0.0006495193031586813),
      Sample(0.0010811877546244523),   Sample(-0.0015057230584022971),
      Sample(0.0016546565841673655),   Sample(-0.001067478223047236),
      Sample(-0.0009628364544560138),  Sample(0.005433823805004525),
      Sample(-0.013742975919210006),   Sample(0.027998245212399724),
      Sample(-0.052034807030786126),   Sample(0.09519657968861324),
      Sample(-0.19231180108293305),    Sample(0.6983395497678168),
      Sample(0.5597718257957642),      Sample(-0.19407409197259143),
      Sample(0.10644983846168542),     Sample(-0.06413916878131061),
      Sample(0.038819470973703155),    Sample(-0.022579514226450468),
      Sample(0.012159411908367439),    Sample(-0.005752239740468992),
      Sample(0.0021158727942215106),   Sample(-0.00030763689109161615),
      Sample(-0.0003957045458875576),  Sample(0.0005141854613472223),
      Sample(-0.00039477665215476487), Sample(0.00022524339213407982),
      Sample(-0.00010108397935197923), Sample(2.608615624166069e-05),
    },
    {
      Sample(3.184514794042204e-05),  Sample(-0.0001387749559101323),
      Sample(0.0003608018837120524),  Sample(-0.0007619404606755493),
      Sample(0.0013470268720799255),  Sample(-0.0020526860419967153),
      Sample(0.002666053429565046),   Sample(-0.0027820259743428642),
      Sample(0.0017386595750233202),  Sample(0.0014377816246666129),
      Sample(-0.008150310454753227),  Sample(0.020545654418939463),
      Sample(-0.042539184456460516),  Sample(0.08369719898118388),
      Sample(-0.18052889044597611),   Sample(0.759563625581294),
      Sample(0.48521289578202287),    Sample(-0.18524547762799762),
      Sample(0.1063043461184521),     Sample(-0.06657797929998237),
      Sample(0.041955008246504215),   Sample(-0.025600360746098044),
      Sample(0.01470353025812795),    Sample(-0.00769951717580503),
      Sample(0.0034859905935713358),  Sample(-0.0011943019636486416),
      Sample(0.00012871926088374428), Sample(0.00023431075928766928),
      Sample(-0.0002630472283497376), Sample(0.00017258765442065211),
      Sample(-8.465915842685608e-05), Sample(2.292027423928119e-05),
    },
    {
      Sample(3.165559755790354e-05),   Sample(-0.000144572025311346),
      Sample(0.0003907108219219258),   Sample(-0.0008570751580271142),
      Sample(0.0015860613377086748),   Sample(-0.0025662981598160947),
      Sample(0.00364969957798612),     Sample(-0.004502284978844274),
      Sample(0.00453138960164905),     Sample(-0.0028225133836233983),
      Sample(-0.0019805202789144947),  Sample(0.01197866312769322),
      Sample(-0.03100394330656437),    Sample(0.06843178056277632),
      Sample(-0.16094135179759603),    Sample(0.813705875381671),
      Sample(0.4090724884044057),      Sample(-0.1709524804335472),
      Sample(0.10257693614017332),     Sample(-0.0666345089436824),
      Sample(0.04351260778784133),     Sample(-0.027602354221803443),
      Sample(0.016616853117648735),    Sample(-0.009277911666460817),
      Sample(0.004655733540001486),    Sample(-0.001982110165895166),
      Sample(0.0006104022458092286),   Sample(-3.047720515685937e-05),
      Sample(-0.00013485862543473318), Sample(0.00011985100122193777),
      Sample(-6.766962245519853e-05),  Sample(1.951819551734623e-05),
    },
    {
      Sample(2.9983659311536116e-05),  Sample(-0.00014559182148270598),
      Sample(0.00040912695452287334),  Sample(-0.0009295513963859288),
      Sample(0.0017869784165739058),   Sample(-0.0030249461748891896),
      Sample(0.004567548873821048),    Sample(-0.006165881629247218),
      Sample(0.0073193777259110675),   Sample(-0.007207664902493617),
      Sample(0.004574804173132053),    Sample(0.002545582095007783),
      Sample(-0.017725215650396725),   Sample(0.04968078004461383),
      Sample(-0.13336654402797954),    Sample(0.8596442052557635),
      Sample(0.3328078777833169),      Sample(-0.1520746007365989),
      Sample(0.09559808139598455),     Sample(-0.064426888927973),
      Sample(0.04351554690228815),     Sample(-0.028567008943939227),
      Sample(0.017866229148661015),    Sample(-0.010453529597007261),
      Sample(0.005596853827197398),    Sample(-0.0026504110673602433),
      Sample(0.0010358153845066098),   Sample(-0.0002722217177740313),
      Sample(-1.4371602956711715e-05), Sample(6.891961867747201e-05),
      Sample(-5.0827311082251037e-05), Sample(1.607947200193808e-05),
    },
    {
      Sample(2.6600342651664725e-05), Sample(-0.00014106125576820053),
      Sample(0.0004139832051857727),  Sample(-0.0009743540932177976),
      Sample(0.0019389630887573213),  Sample(-0.0034074610587552516),
      Sample(0.005381433118070637),   Sample(-0.0077087147406803246),
      Sample(0.010001420565947525),   Sample(-0.011566417664852165),
      Sample(0.011300546124782195),   Sample(-0.007461964682316679),
      Sample(-0.00307597160129911),   Sample(0.02785910408164383),
      Sample(-0.09781300376626244),   Sample(0.8964187216080176),
      Sample(0.2578388742557367),     Sample(-0.1295568905547041),
      Sample(0.08578448117448852),    Sample(-0.06014302984956069),
      Sample(0.042038744258082315),   Sample(-0.028512242333653397),
      Sample(0.01844304744889957),    Sample(-0.011208255473759107),
      Sample(0.006290717688516446),   Sample(-0.003184069405785382),
      Sample(0.0013943899741018255),  Sample(-0.00048443203467528074),
      Sample(9.49180551186065e-05),   Sample(2.140709424007283e-05),
      Sample(-3.474199150373041e-05), Sample(1.2763936533040183e-05),
    },
    {
      Sample(2.133090888007697e-05),   Sample(-0.00013040247649419103),
      Sample(0.0004035285897779825),   Sample(-0.0009870688640381503),
      Sample(0.002032162296534101),    Sample(-0.003693954931272076),
      Sample(0.00605449217580668),     Sample(-0.009067221953740245),
      Sample(0.012474509419633528),    Sample(-0.015740529859447194),
      Sample(0.01796460617158813),     Sample(-0.01771718325712137),
      Sample(0.012502761908825356),    Sample(0.0035108812320078318),
      Sample(-0.054487465007320435),   Sample(0.9232561387862673),
      Sample(0.18551434376573492),     Sample(-0.10438155546098346),
      Sample(0.07362137405852807),     Sample(-0.05403100851892882),
      Sample(0.039203780525624676),    Sample(-0.027489998096875804),
      Sample(0.0183622904540556),      Sample(-0.011539519582625996),
      Sample(0.00672839027491833),     Sample(-0.003573646795731787),
      Sample(0.0016786776428574547),   Sample(-0.0006622032572619759),
      Sample(0.00019025938799197717),  Sample(-2.1383395038703052e-05),
      Sample(-1.9890198537748112e-05), Sample(9.703922934794865e-06),
    },
    {
      Sample(1.4049313350164142e-05), Sample(-0.00011317194812522876),
      Sample(0.0003764277936461807),  Sample(-0.0009641081958538801),
      Sample(0.002058135209493373),   Sample(-0.0038666724001800218),
      Sample(0.006552635892362446),   Sample(-0.010180768986647017),
      Sample(0.014637502328099185),   Sample(-0.01957014646036561),
      Sample(0.024325373332777903),   Sample(-0.02786788710334668),
      Sample(0.0285134132009406),     Sample(-0.022699964595321732),
      Sample(-0.0037966492050887394), Sample(0.9395895134500334),
      Sample(0.11708115241647543),    Sample(-0.07753958159867064),
      Sample(0.05964357277159895),    Sample(-0.04638792220871797),
      Sample(0.03517255991060761),    Sample(-0.025582816588426923),
      Sample(0.017660810041711397),   Sample(-0.011459536884283111),
      Sample(0.00691037004319183),    Sample(-0.0038153560241077082),
      Sample(0.0018843883768186523),  Sample(-0.0008022588019898115),
      Sample(0.00026964783145397704), Sample(-5.84634321592889e-05),
      Sample(-6.653674007204556e-06), Sample(1.3479673515322202e-05),
    },
  }};
};

} // namespace SomeDSP
