/* src/geometry/prototype/Scintillator.cc
 *
 * Copyright 2018 Brandon Gomes
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "geometry/Prototype.hh"

#include <cmath>

#include <G4SubtractionSolid.hh>
#include <G4UnionSolid.hh>
#include <G4Tubs.hh>
#include <G4VisAttributes.hh>
#include <G4NistManager.hh>
#include <tls.hh>

#include "geometry/Construction.hh"

namespace MATHUSLA { namespace MU {

namespace Prototype { //////////////////////////////////////////////////////////////////////////

//__Scintillator Info___________________________________________________________________________
const Scintillator::Info Scintillator::InfoArray[Scintillator::Count] = {
  {"SA1-1", -1.00015*m,  1.09070*m, -2.94162*m,  0.00331*rad, 0.24386*m, 0.21381*m, 0.35829*m},
  {"SA1-2", -1.00266*m,  0.69618*m, -2.99162*m, -0.01336*rad, 0.27822*m, 0.24283*m, 0.41378*m},
  {"SA1-3", -1.00470*m,  0.23554*m, -2.94263*m, -0.00225*rad, 0.31529*m, 0.27581*m, 0.48080*m},
  {"SA1-4", -1.00885*m, -0.29779*m, -2.99162*m,  0.00331*rad, 0.36453*m, 0.32081*m, 0.56529*m},
  {"SA1-5", -1.00506*m, -0.93431*m, -2.94062*m, -0.01336*rad, 0.41329*m, 0.36683*m, 0.67383*m},
  {"SA2-1", -0.76814*m,  0.95462*m, -3.18038*m, -3.13229*rad, 0.41253*m, 0.36181*m, 0.67139*m},
  {"SA2-2", -0.76802*m,  0.31925*m, -3.23038*m, -3.13785*rad, 0.35949*m, 0.31581*m, 0.56385*m},
  {"SA2-3", -0.77578*m, -0.21938*m, -3.18038*m,  3.13423*rad, 0.31453*m, 0.27783*m, 0.48077*m},
  {"SA2-4", -0.77695*m, -0.67459*m, -3.23138*m,  3.13978*rad, 0.27709*m, 0.24681*m, 0.41732*m},
  {"SA2-5", -0.77965*m, -1.07081*m, -3.18038*m,  3.13423*rad, 0.24278*m, 0.21382*m, 0.35878*m},
  {"SA3-1", -0.41191*m,  1.01230*m, -2.94162*m,  0.01086*rad, 0.33856*m, 0.29381*m, 0.54385*m},
  {"SA3-2", -0.41632*m,  0.40023*m, -2.99062*m,  0.01086*rad, 0.39035*m, 0.33981*m, 0.65287*m},
  {"SA3-3", -0.41763*m, -0.12076*m, -2.94162*m,  0.01641*rad, 0.42150*m, 0.39281*m, 0.36736*m},
  {"SA3-4", -0.41948*m, -0.51989*m, -2.99062*m,  0.00530*rad, 0.45568*m, 0.42381*m, 0.40930*m},
  {"SA3-5", -0.42093*m, -1.00840*m, -2.94162*m,  0.01086*rad, 0.50069*m, 0.46181*m, 0.54581*m},
  {"SA4-1",  0.01599*m,  0.95191*m, -3.23087*m,  3.13161*rad, 0.54891*m, 0.50284*m, 0.63077*m},
  {"SA4-2",  0.01561*m,  0.35535*m, -3.18187*m, -3.13491*rad, 0.50224*m, 0.45782*m, 0.54631*m},
  {"SA4-3",  0.01011*m, -0.13135*m, -3.23187*m,  3.11495*rad, 0.45631*m, 0.42798*m, 0.40418*m},
  {"SA4-4",  0.01043*m, -0.52745*m, -3.18187*m,  3.12606*rad, 0.42276*m, 0.39387*m, 0.36826*m},
  {"SA4-5",  0.00379*m, -1.04825*m, -3.23187*m, -3.13491*rad, 0.39181*m, 0.33781*m, 0.65138*m},
  {"SA5-1",  0.50356*m,  0.87596*m, -2.99213*m, -0.03599*rad, 0.48155*m, 0.41398*m, 0.81024*m},
  {"SA5-2",  0.49444*m,  0.23587*m, -2.94413*m,  0.00287*rad, 0.51533*m, 0.47884*m, 0.45281*m},
  {"SA5-3",  0.49207*m, -0.25252*m, -2.99313*m,  0.03063*rad, 0.55858*m, 0.51921*m, 0.50736*m},
  {"SA5-4",  0.48803*m, -0.85323*m, -2.94413*m,  0.01953*rad, 0.61391*m, 0.55903*m, 0.67824*m},
  {"SA6-1",  1.02996*m,  0.93692*m, -3.18038*m,  3.13635*rad, 0.61503*m, 0.56081*m, 0.67881*m},
  {"SA6-2",  1.02524*m,  0.33127*m, -3.22938*m, -3.13572*rad, 0.56633*m, 0.51984*m, 0.50686*m},
  {"SA6-3",  1.01898*m, -0.16217*m, -3.18038*m, -3.14128*rad, 0.51695*m, 0.48182*m, 0.45632*m},
  {"SA6-4",  1.01231*m, -0.80815*m, -3.22938*m, -3.14128*rad, 0.47909*m, 0.41681*m, 0.81132*m},
  {"SB1-1", -1.07908*m, -1.03253*m,  3.18687*m,  1.56892*rad, 0.24198*m, 0.21481*m, 0.35780*m},
  {"SB1-2", -0.67870*m, -1.03663*m,  3.23687*m,  1.55226*rad, 0.27550*m, 0.24384*m, 0.41525*m},
  {"SB1-3", -0.22084*m, -1.04418*m,  3.18687*m,  1.55226*rad, 0.31373*m, 0.28285*m, 0.47728*m},
  {"SB1-4",  0.30202*m, -1.05020*m,  3.23687*m,  1.55781*rad, 0.33859*m, 0.29583*m, 0.54277*m},
  {"SB1-5",  0.92233*m, -1.05586*m,  3.18687*m,  1.57448*rad, 0.41375*m, 0.36081*m, 0.67037*m},
  {"SB2-1", -0.92521*m, -0.70816*m,  2.99938*m, -1.58416*rad, 0.41412*m, 0.36183*m, 0.67178*m},
  {"SB2-2", -0.29418*m, -0.72191*m,  2.94937*m, -1.57305*rad, 0.36048*m, 0.31681*m, 0.56785*m},
  {"SB2-3",  0.24353*m, -0.73007*m,  2.99938*m, -1.58416*rad, 0.31522*m, 0.27783*m, 0.48178*m},
  {"SB2-4",  0.70345*m, -0.73878*m,  2.94837*m, -1.58416*rad, 0.27493*m, 0.24482*m, 0.41379*m},
  {"SB2-5",  1.10085*m, -0.74894*m,  2.99938*m, -1.59526*rad, 0.24196*m, 0.21486*m, 0.35771*m},
  {"SB3-1", -0.98812*m, -0.36642*m,  3.23738*m,  1.59594*rad, 0.33851*m, 0.29493*m, 0.54431*m},
  {"SB3-2", -0.37966*m, -0.37054*m,  3.18838*m,  1.60149*rad, 0.39257*m, 0.33901*m, 0.65385*m},
  {"SB3-3",  0.13907*m, -0.37363*m,  3.23738*m,  1.59039*rad, 0.42345*m, 0.39291*m, 0.36682*m},
  {"SB3-4",  0.56073*m, -0.37476*m,  3.18737*m,  1.57928*rad, 0.51516*m, 0.48084*m, 0.45381*m},
  {"SB3-5",  1.02640*m, -0.37922*m,  3.23738*m,  1.56817*rad, 0.51611*m, 0.48081*m, 0.45680*m},
  {"SB4-1", -0.97739*m,  0.03031*m,  2.99538*m, -1.57042*rad, 0.50050*m, 0.45581*m, 0.54580*m},
  {"SB4-2", -0.37688*m,  0.03355*m,  2.94638*m, -1.57042*rad, 0.47043*m, 0.42381*m, 0.63480*m},
  {"SB4-3",  0.13517*m,  0.02928*m,  2.99538*m, -1.56487*rad, 0.42236*m, 0.39182*m, 0.36631*m},
  {"SB4-4",  0.51771*m,  0.03567*m,  2.94537*m, -1.54821*rad, 0.34155*m, 0.31189*m, 0.37779*m},
  {"SB4-5",  0.90707*m,  0.03276*m,  2.99538*m, -1.57598*rad, 0.34147*m, 0.31182*m, 0.37831*m},
  {"SB4-6",  1.26660*m,  0.03878*m,  2.94437*m, -1.56487*rad, 0.31002*m, 0.28381*m, 0.32230*m},
  {"SB5-1", -1.14184*m,  0.40222*m,  3.18637*m,  1.59113*rad, 0.31280*m, 0.28688*m, 0.32676*m},
  {"SB5-2", -0.74689*m,  0.40519*m,  3.23738*m,  1.58557*rad, 0.38156*m, 0.34686*m, 0.44628*m},
  {"SB5-3", -0.18067*m,  0.40764*m,  3.18938*m,  1.60778*rad, 0.41504*m, 0.36209*m, 0.66918*m},
  {"SB5-4",  0.56556*m,  0.41329*m,  3.23738*m,  1.58003*rad, 0.48073*m, 0.41584*m, 0.80877*m},
  {"SB5-5",  1.20453*m,  0.40706*m,  3.18838*m,  1.59114*rad, 0.51880*m, 0.47593*m, 0.44871*m},
  {"SB6-1", -0.97313*m,  0.80630*m,  2.95038*m, -1.54896*rad, 0.49875*m, 0.46092*m, 0.54671*m},
  {"SB6-2", -0.41839*m,  0.84173*m,  2.99938*m, -1.54896*rad, 0.49921*m, 0.46192*m, 0.54779*m},
  {"SB6-3",  0.09269*m,  0.84195*m,  2.95038*m, -1.56562*rad, 0.51731*m, 0.47982*m, 0.45230*m},
  {"SB6-4",  0.60277*m,  0.84690*m,  2.99938*m, -1.56006*rad, 0.50080*m, 0.45784*m, 0.54577*m},
  {"SB6-5",  1.16085*m,  0.85065*m,  2.95038*m, -1.54896*rad, 0.50116*m, 0.45792*m, 0.54670*m}
};
//----------------------------------------------------------------------------------------------

//__Scintillator Constructor____________________________________________________________________
Scintillator::Scintillator(const std::string& input_name,
                           const double input_height,
                           const double input_minwidth,
                           const double input_maxwidth)
    : name(input_name), height(input_height), minwidth(input_minwidth), maxwidth(input_maxwidth),
      lvolume(nullptr), pvolume(nullptr), sensitive(nullptr) {

  auto solid = Construction::Trap("", height, minwidth, maxwidth, Depth);

  const double dims[4] = {height   - 2.0 * Thickness, minwidth - 2.0 * Thickness,
                          maxwidth - 2.0 * Thickness, Depth    - 2.0 * Thickness};

  auto casing = new G4SubtractionSolid(name + "_C", solid,
    Construction::Trap("", dims[0], dims[1], dims[2], dims[3]));

  auto sensitive_trap = Construction::Trap(name,
    dims[0] - 2.0 * Spacing, dims[1] - 2.0 * Spacing, dims[2] - 2.0 * Spacing, dims[3] - 2.0 * Spacing);

  auto pmt = new G4Tubs(
    name + "_PMT", 0, PMTRadius, 0.5 * PMTLength, 0, 360.0*deg);

  auto pmtTransform = Construction::Transform(
    0.5 * maxwidth + 0.25 * PMTLength, 0, 0.5 * height + 0.25 * PMTLength,
    0, 1, 0, 45*deg);

  lvolume = Construction::Volume(
    new G4UnionSolid(name,
    new G4UnionSolid("", casing, sensitive_trap), pmt, pmtTransform),
    Construction::BorderAttributes());

  auto casingLV = Construction::Volume(casing,
    Material::Casing,
    Construction::CasingAttributes());

  auto sensitiveLV = Construction::Volume(sensitive_trap,
    Material::Scintillator,
    Construction::SensitiveAttributes());

  auto pmtAttr = G4VisAttributes(G4Colour(0.7, 0.7, 0.7));
  pmtAttr.SetForceSolid(true);
  auto pmtLV = Construction::Volume(pmt, Material::PMT, pmtAttr);

  Construction::PlaceVolume(casingLV, lvolume);
  sensitive = Construction::PlaceVolume(sensitiveLV, lvolume);
  Construction::PlaceVolume(pmtLV, lvolume, pmtTransform);
}
//----------------------------------------------------------------------------------------------

//__Scintillator Material_______________________________________________________________________
G4Material* Scintillator::Material::PMT          = nullptr;
G4Material* Scintillator::Material::Casing       = nullptr;
G4Material* Scintillator::Material::Scintillator = nullptr;
//----------------------------------------------------------------------------------------------

//__Define Scintillator Material________________________________________________________________
void Scintillator::Material::Define() {
  Material::PMT = G4NistManager::Instance()->FindOrBuildMaterial("G4_C");
  Material::Casing = Construction::Material::Aluminum;
  Material::Scintillator = Construction::Material::Polyvinyltoluene;

  constexpr int_fast32_t nSci = 1;
  double eSci[nSci] = { 3.10*eV };
  double rSci[nSci] = { 1.58    };

  auto sciProp = new G4MaterialPropertiesTable();
  sciProp->AddProperty("RINDEX", eSci, rSci, nSci);
  Material::Scintillator->SetMaterialPropertiesTable(sciProp);
}
//----------------------------------------------------------------------------------------------

//__Scintillator Full Name______________________________________________________________________
const std::string Scintillator::GetFullName() const {
  return sensitive ? sensitive->GetName() : name;
}
//----------------------------------------------------------------------------------------------

//__Calculate Distance to PMT___________________________________________________________________
Scintillator::PMTPoint Scintillator::PMTDistance(const G4ThreeVector position,
                                                 const Scintillator* sci,
                                                 const G4ThreeVector translation,
                                                 const G4RotationMatrix rotation) {

  const auto delta = rotation*(translation - position);

  // Trapezoid coordinates
  const auto x = -delta.x();
  const auto y = -delta.z();

  const auto up_distance = 0.5 * sci->height - y;

  return {
    up_distance,
    std::hypot(y, 0.25 * (sci->maxwidth + sci->minwidth) - x),
    std::hypot(up_distance, 0.5 * sci->maxwidth - x)
  };
}
//----------------------------------------------------------------------------------------------

//__Register Scintillator with Detector_________________________________________________________
void Scintillator::Register(G4VSensitiveDetector* detector) {
  sensitive->GetLogicalVolume()->SetSensitiveDetector(detector);
}
//----------------------------------------------------------------------------------------------

//__Clone Scintillator__________________________________________________________________________
Scintillator* Scintillator::Clone(const Scintillator* other) {
  return new Scintillator(other->name, other->height, other->minwidth, other->maxwidth);
}
//----------------------------------------------------------------------------------------------

} /* namespace Prototype */ ////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
