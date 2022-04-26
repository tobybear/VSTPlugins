// (c) 2022 Takamitsu Endo
//
// This file is part of BasicLimiter.
//
// BasicLimiter is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// BasicLimiter is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with BasicLimiter.  If not, see <https://www.gnu.org/licenses/>.

#include <algorithm>
#include <array>

namespace SomeDSP {

template<typename Sample, typename Fir> class FirDownSampler {
  std::array<std::array<Sample, Fir::bufferSize>, Fir::upfold> buf{{}};

public:
  void reset() { buf.fill({}); }

  Sample process(const std::array<Sample, Fir::upfold> &input)
  {
    for (size_t i = 0; i < Fir::upfold; ++i) {
      std::rotate(buf[i].rbegin(), buf[i].rbegin() + 1, buf[i].rend());
      buf[i][0] = input[i];
    }

    Sample output = 0;
    for (size_t i = 0; i < Fir::coefficient.size(); ++i) {
      auto &&phase = Fir::coefficient[i];
      for (size_t n = 0; n < phase.size(); ++n) output += buf[i][n] * phase[n];
    }
    return output;
  }
};

/**
FIR polyphase lowpass coefficients for 8 fold downsampling.

```python
fir = signal.firwin(511, 20000, window=("dpss", 4), fs=8 * 48000)
fir = np.hstack((fir, [0]))

poly = fir.reshape((nTaps, nPhase)).T
```
*/
template<typename Sample> struct DownSamplerFir8Fold {
  constexpr static size_t bufferSize = 64;
  constexpr static size_t intDelay = 31;
  constexpr static size_t upfold = 8;

  constexpr static std::array<std::array<Sample, bufferSize>, upfold> coefficient{{
    {
      Sample(6.052993576901968e-08),   Sample(-2.2040588788328898e-07),
      Sample(2.673215074693805e-07),   Sample(3.726960324506497e-07),
      Sample(-2.554344119302319e-06),  Sample(6.827942942080198e-06),
      Sample(-1.2344441767502497e-05), Sample(1.570082969070882e-05),
      Sample(-1.0684120928898419e-05), Sample(-9.965772315161148e-06),
      Sample(5.022950277413057e-05),   Sample(-0.00010479403797941494),
      Sample(0.0001540664466431831),   Sample(-0.00016421928710076095),
      Sample(9.584088089955429e-05),   Sample(7.809908318707076e-05),
      Sample(-0.0003491568690193118),  Sample(0.0006545421209730521),
      Sample(-0.0008745158038705124),  Sample(0.0008558474762892359),
      Sample(-0.0004630359683227712),  Sample(-0.0003530871513915984),
      Sample(0.0014913994280287821),   Sample(-0.002668797764213702),
      Sample(0.0034435511483746515),   Sample(-0.003300355261343235),
      Sample(0.0017801223138234925),   Sample(0.001386736341110275),
      Sample(-0.006205159923063164),   Sample(0.01249218896567382),
      Sample(-0.020393652543245745),   Sample(0.03403721446135357),
      Sample(0.1023081301951627),      Sample(0.006849572124696738),
      Sample(-0.012027078670244534),   Sample(0.011393123241246113),
      Sample(-0.008570379940087735),   Sample(0.005009687265150047),
      Sample(-0.0016775952119604824),  Sample(-0.0008093955534936323),
      Sample(0.002190855411747915),    Sample(-0.0025263757592754992),
      Sample(0.002098994319355854),    Sample(-0.0012859918833576583),
      Sample(0.00043689283237609506),  Sample(0.00020910931315707768),
      Sample(-0.000552217605876423),   Sample(0.0006130786660399914),
      Sample(-0.00048492552502598236), Sample(0.0002800135793917379),
      Sample(-8.881111121958298e-05),  Sample(-3.931233183395993e-05),
      Sample(9.508184134536903e-05),   Sample(-9.567180522842486e-05),
      Sample(6.778820289319847e-05),   Sample(-3.459717670656732e-05),
      Sample(9.545545190937156e-06),   Sample(3.6048125414053386e-06),
      Sample(-7.257424837117654e-06),  Sample(5.884908260819308e-06),
      Sample(-3.2133061416839596e-06), Sample(1.182932677959194e-06),
      Sample(-2.1123568640007462e-07), Sample(-4.1668387372940177e-08),
    },
    {
      Sample(7.89498758312858e-08),    Sample(-3.1393559265007737e-07),
      Sample(5.667940663935022e-07),   Sample(-2.7396793193353e-07),
      Sample(-1.608896302099959e-06),  Sample(6.152610495780398e-06),
      Sample(-1.3327931319394115e-05), Sample(2.0491010318401015e-05),
      Sample(-2.1389383294283738e-05), Sample(7.020138442668566e-06),
      Sample(3.058792658515678e-05),   Sample(-9.183857261448366e-05),
      Sample(0.00016246773675813366),  Sample(-0.00021000453302503008),
      Sample(0.00018848260207290324),  Sample(-5.415444086615754e-05),
      Sample(-0.00020965921100064318), Sample(0.000566468774820931),
      Sample(-0.0009119154854177456),  Sample(0.001083583812169485),
      Sample(-0.0009026277174319366),  Sample(0.00024297160896100437),
      Sample(0.0008898354018394862),   Sample(-0.002298101538873049),
      Sample(0.0035784988489498204),   Sample(-0.004172464757351632),
      Sample(0.0034741319488297267),   Sample(-0.0009589409161455977),
      Sample(-0.003742227603179449),   Sample(0.010986644189606999),
      Sample(-0.022145871600269082),   Sample(0.04885412094655425),
      Sample(0.09685238915065805),     Sample(-0.004117387165728274),
      Sample(-0.006571829079469747),   Sample(0.009135810261281659),
      Sample(-0.008357541256706793),   Sample(0.0059639816456563376),
      Sample(-0.003085353228303186),   Sample(0.0005265554309749465),
      Sample(0.0012370879101196541),   Sample(-0.0020593421984969338),
      Sample(0.002063705414969643),    Sample(-0.0015360427007947487),
      Sample(0.0008035744899196746),   Sample(-0.00013572985175042937),
      Sample(-0.0003105487007475381),  Sample(0.0004969705911670242),
      Sample(-0.00047350853844833695), Sample(0.00033176925064889236),
      Sample(-0.00016184572909596397), Sample(2.525201317150932e-05),
      Sample(5.284873603931061e-05),   Sample(-7.654469440853126e-05),
      Sample(6.522921207695318e-05),   Sample(-4.032144353068764e-05),
      Sample(1.707309827999544e-05),   Sample(-2.2663745930560334e-06),
      Sample(-3.934216901171562e-06),  Sample(4.570143874535786e-06),
      Sample(-2.980794607557022e-06),  Sample(1.3150859798753382e-06),
      Sample(-3.5371941808763157e-07), Sample(2.3491813038709306e-08),
    },
    {
      Sample(8.983215412102375e-08),   Sample(-3.906718286232915e-07),
      Sample(8.641847852120751e-07),   Sample(-1.0181403622928186e-06),
      Sample(-2.980285358450533e-07),  Sample(4.629504164466036e-06),
      Sample(-1.2867337522180504e-05), Sample(2.349056255022532e-05),
      Sample(-3.086576047805624e-05),  Sample(2.5033792116603554e-05),
      Sample(5.482754898154145e-06),   Sample(-6.72485213059645e-05),
      Sample(0.00015326963912330185),  Sample(-0.00023597912900819143),
      Sample(0.00026725894638544016),  Sample(-0.00019013799330855436),
      Sample(-3.706386386833305e-05),  Sample(0.00040969334769947775),
      Sample(-0.0008508247702832494),  Sample(0.0012056848235303377),
      Sample(-0.0012688393458007467),  Sample(0.0008467223563671459),
      Sample(0.00015632883037386498),  Sample(-0.0016540450377435473),
      Sample(0.00332805359352165),     Sample(-0.004637176749537744),
      Sample(0.004891219391427155),    Sample(-0.003360128331899351),
      Sample(-0.0006651852970097721),  Sample(0.008091593801275361),
      Sample(-0.021627225128289147),   Sample(0.0633823103075712),
      Sample(0.08814995337789786),     Sample(-0.012659376950375539),
      Sample(-0.0010604941226809825),  Sample(0.006131097235523045),
      Sample(-0.00729827052736266),    Sample(0.006242958078738451),
      Sample(-0.0040932165551778215),  Sample(0.0017350744629463673),
      Sample(0.00020569939802658497),  Sample(-0.0014030499687394363),
      Sample(0.001815619392428068),    Sample(-0.001612467796738642),
      Sample(0.0010657996639487535),   Sample(-0.0004461282191503878),
      Sample(-5.14171018551276e-05),   Sample(0.00033665474924249555),
      Sample(-0.00041366996626171455), Sample(0.0003454235128983932),
      Sample(-0.00021265206885572854), Sample(8.212571686512861e-05),
      Sample(8.64685826721467e-06),    Sample(-5.116884807143025e-05),
      Sample(5.6145000664680664e-05),  Sample(-4.128385761943455e-05),
      Sample(2.2010178104189546e-05),  Sample(-7.21150150048471e-06),
      Sample(-6.274733503794657e-07),  Sample(2.963241666051421e-06),
      Sample(-2.470757291662767e-06),  Sample(1.2821486671128597e-06),
      Sample(-4.254691583283463e-07),  Sample(6.637217002795398e-08),
    },
    {
      Sample(8.736479457512682e-08),   Sample(-4.333857258811628e-07),
      Sample(1.1176588882807377e-06),  Sample(-1.7821366901904466e-06),
      Sample(1.2767201478098856e-06),  Sample(2.315202976193073e-06),
      Sample(-1.0818874028978598e-05), Sample(2.411991835114055e-05),
      Sample(-3.786397995882653e-05),  Sample(4.210000098871949e-05),
      Sample(-2.274649049110247e-05),  Sample(-3.274711478701158e-05),
      Sample(0.0001259799056216687),   Sample(-0.0002375842535554464),
      Sample(0.0003222419188796251),   Sample(-0.00031490281989730824),
      Sample(0.00015168354510415623),  Sample(0.00019708292888088205),
      Sample(-0.000691751042601519),   Sample(0.0012021776483993967),
      Sample(-0.0015169000868705698),  Sample(0.0013920824671510292),
      Sample(-0.0006359028075180551),  Sample(-0.0007919800525495209),
      Sample(0.002697758633936435),    Sample(-0.004619623168299028),
      Sample(0.00585893339727668),     Sample(-0.005558112660404424),
      Sample(0.0027405055105084513),   Sample(0.003972781637226023),
      Sample(-0.018513962609660706),   Sample(0.07675487877392995),
      Sample(0.07675487877392807),     Sample(-0.01851396260965954),
      Sample(0.0039727816372256705),   Sample(0.002740505510508167),
      Sample(-0.005558112660403846),   Sample(0.005858933397276137),
      Sample(-0.004619623168298681),   Sample(0.0026977586339362687),
      Sample(-0.0007919800525494824),  Sample(-0.0006359028075180312),
      Sample(0.0013920824671509828),   Sample(-0.0015169000868705267),
      Sample(0.0012021776483993785),   Sample(-0.0006917510426015173),
      Sample(0.0001970829288808845),   Sample(0.00015168354510415935),
      Sample(-0.0003149028198973168),  Sample(0.00032224191887963554),
      Sample(-0.00023758425355545478), Sample(0.00012597990562167365),
      Sample(-3.274711478701277e-05),  Sample(-2.2746490491103217e-05),
      Sample(4.210000098872082e-05),   Sample(-3.786397995882778e-05),
      Sample(2.41199183511414e-05),    Sample(-1.0818874028978986e-05),
      Sample(2.3152029761931604e-06),  Sample(1.2767201478099383e-06),
      Sample(-1.7821366901905197e-06), Sample(1.1176588882807847e-06),
      Sample(-4.3338572588118113e-07), Sample(8.736479457513034e-08),
    },
    {
      Sample(6.637217002795131e-08),   Sample(-4.254691583283283e-07),
      Sample(1.2821486671128057e-06),  Sample(-2.4707572916626664e-06),
      Sample(2.963241666051299e-06),   Sample(-6.274733503794421e-07),
      Sample(-7.211501500484452e-06),  Sample(2.2010178104188784e-05),
      Sample(-4.1283857619433184e-05), Sample(5.6145000664678895e-05),
      Sample(-5.116884807142852e-05),  Sample(8.646858267214352e-06),
      Sample(8.212571686512538e-05),   Sample(-0.0002126520688557211),
      Sample(0.0003454235128983822),   Sample(-0.00041366996626170365),
      Sample(0.000336654749242489),    Sample(-5.1417101855127034e-05),
      Sample(-0.0004461282191503896),  Sample(0.0010657996639487713),
      Sample(-0.0016124677967386907),  Sample(0.0018156193924281283),
      Sample(-0.001403049968739491),   Sample(0.00020569939802659532),
      Sample(0.0017350744629464772),   Sample(-0.0040932165551781355),
      Sample(0.006242958078739041),    Sample(-0.007298270527363424),
      Sample(0.006131097235523672),    Sample(-0.0010604941226810731),
      Sample(-0.012659376950376285),   Sample(0.08814995337789949),
      Sample(0.0633823103075693),      Sample(-0.021627225128287707),
      Sample(0.008091593801274624),    Sample(-0.0006651852970097025),
      Sample(-0.003360128331899005),   Sample(0.004891219391426715),
      Sample(-0.004637176749537404),   Sample(0.003328053593521451),
      Sample(-0.001654045037743469),   Sample(0.00015632883037385926),
      Sample(0.0008467223563671177),   Sample(-0.0012688393458007122),
      Sample(0.001205684823530321),    Sample(-0.0008508247702832492),
      Sample(0.00040969334769948344),  Sample(-3.706386386833384e-05),
      Sample(-0.00019013799330855967), Sample(0.00026725894638544894),
      Sample(-0.00023597912900819994), Sample(0.0001532696391233079),
      Sample(-6.72485213059669e-05),   Sample(5.482754898154322e-06),
      Sample(2.5033792116604343e-05),  Sample(-3.0865760478057266e-05),
      Sample(2.3490562550226156e-05),  Sample(-1.2867337522180968e-05),
      Sample(4.6295041644662135e-06),  Sample(-2.9802853584506556e-07),
      Sample(-1.0181403622928603e-06), Sample(8.641847852121121e-07),
      Sample(-3.906718286233078e-07),  Sample(8.983215412102735e-08),
    },
    {
      Sample(2.3491813038708346e-08),  Sample(-3.5371941808761653e-07),
      Sample(1.3150859798752827e-06),  Sample(-2.9807946075568995e-06),
      Sample(4.570143874535599e-06),   Sample(-3.934216901171415e-06),
      Sample(-2.2663745930559525e-06), Sample(1.7073098279994857e-05),
      Sample(-4.03214435306863e-05),   Sample(6.522921207695112e-05),
      Sample(-7.654469440852862e-05),  Sample(5.284873603930863e-05),
      Sample(2.525201317150834e-05),   Sample(-0.0001618457290959584),
      Sample(0.00033176925064888195),  Sample(-0.0004735085384483248),
      Sample(0.0004969705911670151),   Sample(-0.0003105487007475352),
      Sample(-0.00013572985175043016), Sample(0.0008035744899196894),
      Sample(-0.0015360427007947968),  Sample(0.002063705414969712),
      Sample(-0.002059342198497017),   Sample(0.001237087910119718),
      Sample(0.0005265554309749806),   Sample(-0.003085353228303429),
      Sample(0.0059639816456569135),   Sample(-0.008357541256707676),
      Sample(0.009135810261282585),    Sample(-0.006571829079470292),
      Sample(-0.0041173871657285),     Sample(0.09685238915065925),
      Sample(0.04885412094655251),     Sample(-0.022145871600267535),
      Sample(0.010986644189605972),    Sample(-0.0037422276031790548),
      Sample(-0.0009589409161454998),  Sample(0.003474131948829422),
      Sample(-0.004172464757351334),   Sample(0.0035784988489496127),
      Sample(-0.0022981015388729435),  Sample(0.0008898354018394546),
      Sample(0.00024297160896099621),  Sample(-0.0009026277174319133),
      Sample(0.0010835838121694716),   Sample(-0.0009119154854177472),
      Sample(0.0005664687748209395),   Sample(-0.0002096592110006478),
      Sample(-5.4154440866159094e-05), Sample(0.0001884826020729094),
      Sample(-0.00021000453302503778), Sample(0.00016246773675814005),
      Sample(-9.183857261448695e-05),  Sample(3.058792658515775e-05),
      Sample(7.020138442668789e-06),   Sample(-2.1389383294284453e-05),
      Sample(2.049101031840175e-05),   Sample(-1.33279313193946e-05),
      Sample(6.152610495780637e-06),   Sample(-1.6088963021000258e-06),
      Sample(-2.739679319335413e-07),  Sample(5.667940663935263e-07),
      Sample(-3.1393559265009034e-07), Sample(7.894987583128897e-08),
    },
    {
      Sample(-4.1668387372938476e-08), Sample(-2.112356864000657e-07),
      Sample(1.1829326779591447e-06),  Sample(-3.2133061416838266e-06),
      Sample(5.884908260819072e-06),   Sample(-7.257424837117385e-06),
      Sample(3.6048125414052085e-06),  Sample(9.545545190936834e-06),
      Sample(-3.4597176706566196e-05), Sample(6.778820289319634e-05),
      Sample(-9.567180522842151e-05),  Sample(9.508184134536545e-05),
      Sample(-3.931233183395841e-05),  Sample(-8.881111121957999e-05),
      Sample(0.0002800135793917293),   Sample(-0.00048492552502597033),
      Sample(0.0006130786660399806),   Sample(-0.0005522176058764189),
      Sample(0.00020910931315707934),  Sample(0.0004368928323761038),
      Sample(-0.0012859918833577001),  Sample(0.0020989943193559252),
      Sample(-0.002526375759275605),   Sample(0.0021908554117480313),
      Sample(-0.0008093955534936859),  Sample(-0.0016775952119606177),
      Sample(0.005009687265150538),    Sample(-0.00857037994008864),
      Sample(0.011393123241247251),    Sample(-0.012027078670245491),
      Sample(0.006849572124697083),    Sample(0.10230813019516335),
      Sample(0.03403721446135218),     Sample(-0.020393652543244253),
      Sample(0.012492188965672626),    Sample(-0.006205159923062507),
      Sample(0.0013867363411101349),   Sample(0.00178012231382334),
      Sample(-0.0033003552613430046),  Sample(0.003443551148374457),
      Sample(-0.0026687977642135827),  Sample(0.0014913994280287297),
      Sample(-0.0003530871513915865),  Sample(-0.00046303596832276023),
      Sample(0.0008558474762892264),   Sample(-0.0008745158038705157),
      Sample(0.0006545421209730625),   Sample(-0.0003491568690193198),
      Sample(7.809908318707306e-05),   Sample(9.584088089955749e-05),
      Sample(-0.00016421928710076708), Sample(0.0001540664466431891),
      Sample(-0.00010479403797941864), Sample(5.022950277413216e-05),
      Sample(-9.965772315161466e-06),  Sample(-1.0684120928898774e-05),
      Sample(1.570082969070939e-05),   Sample(-1.2344441767502947e-05),
      Sample(6.827942942080464e-06),   Sample(-2.554344119302424e-06),
      Sample(3.726960324506651e-07),   Sample(2.67321507469392e-07),
      Sample(-2.2040588788329798e-07), Sample(6.052993576902212e-08),
    },
    {
      Sample(-1.2560018667148627e-07), Sample(-3.6366478420289016e-22),
      Sample(8.677240217902351e-07),   Sample(-3.0874592551549157e-06),
      Sample(6.69760711370726e-06),    Sample(-1.0197403961937745e-05),
      Sample(9.82507288224883e-06),    Sample(-7.697189631143513e-20),
      Sample(-2.4247608688785383e-05), Sample(6.285413174006766e-05),
      Sample(-0.00010576012474584986), Sample(0.00013036490051041063),
      Sample(-0.00010491300805952946), Sample(1.547141556130793e-18),
      Sample(0.00019308704118444088),  Sample(-0.0004431805263651575),
      Sample(0.0006690363722725955),   Sample(-0.000748448227313185),
      Sample(0.0005523380980721648),   Sample(-1.7136873836304054e-18),
      Sample(-0.0008797685702814334),  Sample(0.0019054657330952445),
      Sample(-0.002742149350871736),   Sample(0.0029579250230675905),
      Sample(-0.0021336494276926058),  Sample(3.2915253065288817e-18),
      Sample(0.0034403025051419262),   Sample(-0.007850395504358435),
      Sample(0.012588823916329941),    Sample(-0.016834653925691098),
      Sample(0.019779613460697905),    Sample(0.10416668363277982),
      Sample(0.019779613460697),       Sample(-0.016834653925689808),
      Sample(0.012588823916328706),    Sample(-0.007850395504357606),
      Sample(0.003440302505141583),    Sample(3.2915253065286074e-18),
      Sample(-0.0021336494276924605),  Sample(0.0029579250230674283),
      Sample(-0.0027421493508716175),  Sample(0.0019054657330951784),
      Sample(-0.0008797685702814042),  Sample(-1.7136873836303679e-18),
      Sample(0.0005523380980721596),   Sample(-0.0007484482273131892),
      Sample(0.0006690363722726065),   Sample(-0.00044318052636516814),
      Sample(0.00019308704118444668),  Sample(1.547141556130845e-18),
      Sample(-0.00010491300805953346), Sample(0.00013036490051041564),
      Sample(-0.0001057601247458536),  Sample(6.285413174006965e-05),
      Sample(-2.424760868878617e-05),  Sample(-7.697189631143771e-20),
      Sample(9.825072882249188e-06),   Sample(-1.0197403961938121e-05),
      Sample(6.697607113707525e-06),   Sample(-3.0874592551550432e-06),
      Sample(8.677240217902713e-07),   Sample(-3.636647842029057e-22),
      Sample(-1.2560018667149143e-07), Sample(0.0),
    },
  }};
};

} // namespace SomeDSP