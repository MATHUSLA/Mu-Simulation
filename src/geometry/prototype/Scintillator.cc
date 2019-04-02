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

#include <Geant4/G4SubtractionSolid.hh>
#include <Geant4/G4UnionSolid.hh>
#include <Geant4/G4Tubs.hh>
#include <Geant4/G4VisAttributes.hh>
#include <Geant4/G4NistManager.hh>
#include <Geant4/tls.hh>

#include "geometry/Construction.hh"

namespace MATHUSLA { namespace MU {

namespace Prototype { //////////////////////////////////////////////////////////////////////////

const Scintillator::ScintillatorInfo Scintillator::scintillator_infos[Scintillator::n_scintillators] = {
  {"SA1-1", -1.00013*m,  1.09070*m, -2.94162*m,  0.00331*rad, 0.24393*m, 0.21381*m, 0.35829*m},
  {"SA1-2", -1.00388*m,  0.69624*m, -2.99162*m, -0.01336*rad, 0.27336*m, 0.24282*m, 0.41377*m},
  {"SA1-3", -1.00407*m,  0.23552*m, -2.94263*m, -0.00225*rad, 0.31781*m, 0.27581*m, 0.48080*m},
  {"SA1-4", -1.00817*m, -0.29781*m, -2.99162*m,  0.00331*rad, 0.36723*m, 0.32082*m, 0.56529*m},
  {"SA1-5", -1.00999*m, -0.93413*m, -2.94062*m, -0.01336*rad, 0.39359*m, 0.36683*m, 0.67376*m},
  {"SA2-1", -0.77387*m,  0.95470*m, -3.18038*m, -3.13229*rad, 0.43542*m, 0.36182*m, 0.67129*m},
  {"SA2-2", -0.77160*m,  0.31934*m, -3.23038*m, -3.13785*rad, 0.37382*m, 0.31582*m, 0.56380*m},
  {"SA2-3", -0.77586*m, -0.21938*m, -3.18038*m,  3.13423*rad, 0.31488*m, 0.27783*m, 0.48077*m},
  {"SA2-4", -0.77919*m, -0.67451*m, -3.23138*m,  3.13978*rad, 0.28605*m, 0.24682*m, 0.41729*m},
  {"SA2-5", -0.77935*m, -1.07082*m, -3.18038*m,  3.13423*rad, 0.24158*m, 0.21382*m, 0.35878*m},
  {"SA3-1", -0.40809*m,  1.01223*m, -2.94162*m,  0.01086*rad, 0.35382*m, 0.29382*m, 0.54380*m},
  {"SA3-2", -0.41145*m,  0.40017*m, -2.99062*m,  0.01086*rad, 0.40982*m, 0.33982*m, 0.65280*m},
  {"SA3-3", -0.41424*m, -0.12085*m, -2.94162*m,  0.01641*rad, 0.43504*m, 0.39283*m, 0.36729*m},
  {"SA3-4", -0.41862*m, -0.51992*m, -2.99062*m,  0.00530*rad, 0.45913*m, 0.42382*m, 0.40929*m},
  {"SA3-5", -0.41915*m, -1.00843*m, -2.94162*m,  0.01086*rad, 0.50781*m, 0.46181*m, 0.54580*m},
  {"SA4-1",  0.01808*m,  0.95184*m, -3.23087*m,  3.13161*rad, 0.54055*m, 0.50284*m, 0.63076*m},
  {"SA4-2",  0.01316*m,  0.35540*m, -3.18187*m, -3.13491*rad, 0.51203*m, 0.45782*m, 0.54629*m},
  {"SA4-3",  0.01297*m, -0.13154*m, -3.23187*m,  3.11495*rad, 0.44486*m, 0.42797*m, 0.40414*m},
  {"SA4-4",  0.01183*m, -0.52753*m, -3.18187*m,  3.12606*rad, 0.41719*m, 0.39386*m, 0.36825*m},
  {"SA4-5", -0.00156*m, -1.04816*m, -3.23187*m, -3.13491*rad, 0.41320*m, 0.33783*m, 0.65129*m},
  {"SA5-1",  0.49327*m,  0.87654*m, -2.99213*m, -0.03599*rad, 0.44035*m, 0.41397*m, 0.80998*m},
  {"SA5-2",  0.49740*m,  0.23578*m, -2.94413*m,  0.00287*rad, 0.52718*m, 0.47885*m, 0.45277*m},
  {"SA5-3",  0.50285*m, -0.25252*m, -2.99313*m,  0.03063*rad, 0.60165*m, 0.51924*m, 0.50691*m},
  {"SA5-4",  0.49641*m, -0.85327*m, -2.94413*m,  0.01953*rad, 0.64741*m, 0.55905*m, 0.67803*m},
  {"SA6-1",  1.02801*m,  0.93697*m, -3.18038*m,  3.13635*rad, 0.62281*m, 0.56081*m, 0.67880*m},
  {"SA6-2",  1.02047*m,  0.33140*m, -3.22938*m, -3.13572*rad, 0.58539*m, 0.51986*m, 0.50677*m},
  {"SA6-3",  1.01646*m, -0.16208*m, -3.18038*m, -3.14128*rad, 0.52703*m, 0.48182*m, 0.45629*m},
  {"SA6-4",  1.00858*m, -0.80808*m, -3.22938*m, -3.14128*rad, 0.49397*m, 0.41682*m, 0.81129*m},
  {"SB1-1", -1.07905*m, -1.03183*m,  3.18687*m,  1.56892*rad, 0.24481*m, 0.21481*m, 0.35780*m},
  {"SB1-2", -0.67875*m, -1.03761*m,  3.23687*m,  1.55226*rad, 0.27160*m, 0.24384*m, 0.41524*m},
  {"SB1-3", -0.22101*m, -1.04748*m,  3.18687*m,  1.55226*rad, 0.30054*m, 0.28284*m, 0.47723*m},
  {"SB1-4",  0.30203*m, -1.04998*m,  3.23687*m,  1.55781*rad, 0.33950*m, 0.29583*m, 0.54277*m},
  {"SB1-5",  0.92244*m, -1.05069*m,  3.18687*m,  1.57448*rad, 0.43441*m, 0.36082*m, 0.67029*m},
  {"SB2-1", -0.92530*m, -0.71079*m,  2.99938*m, -1.58416*rad, 0.42464*m, 0.36184*m, 0.67176*m},
  {"SB2-2", -0.29429*m, -0.72549*m,  2.94937*m, -1.57305*rad, 0.37482*m, 0.31682*m, 0.56780*m},
  {"SB2-3",  0.24347*m, -0.73148*m,  2.99938*m, -1.58416*rad, 0.32086*m, 0.27783*m, 0.48177*m},
  {"SB2-4",  0.70336*m, -0.74039*m,  2.94837*m, -1.58416*rad, 0.28136*m, 0.24483*m, 0.41377*m},
  {"SB2-5",  1.10088*m, -0.74856*m,  2.99938*m, -1.59526*rad, 0.24043*m, 0.21486*m, 0.35771*m},
  {"SB3-1", -0.98809*m, -0.35858*m,  3.23738*m,  1.59594*rad, 0.36987*m, 0.29495*m, 0.54409*m},
  {"SB3-2", -0.37973*m, -0.35881*m,  3.18838*m,  1.60149*rad, 0.43944*m, 0.33903*m, 0.65343*m},
  {"SB3-3",  0.13918*m, -0.36904*m,  3.23738*m,  1.59039*rad, 0.44177*m, 0.39293*m, 0.36671*m},
  {"SB3-4",  0.56081*m, -0.37175*m,  3.18737*m,  1.57928*rad, 0.52720*m, 0.48085*m, 0.45377*m},
  {"SB3-5",  1.02643*m, -0.37854*m,  3.23738*m,  1.56817*rad, 0.51881*m, 0.48081*m, 0.45680*m},
  {"SB4-1", -0.97739*m,  0.03048*m,  2.99538*m, -1.57042*rad, 0.49981*m, 0.45581*m, 0.54580*m},
  {"SB4-2", -0.37685*m,  0.03471*m,  2.94638*m, -1.57042*rad, 0.46581*m, 0.42381*m, 0.63480*m},
  {"SB4-3",  0.13512*m,  0.02786*m,  2.99538*m, -1.56487*rad, 0.42803*m, 0.39182*m, 0.36629*m},
  {"SB4-4",  0.51764*m,  0.03175*m,  2.94537*m, -1.54821*rad, 0.35723*m, 0.31190*m, 0.37770*m},
  {"SB4-5",  0.90715*m,  0.03426*m,  2.99538*m, -1.57598*rad, 0.33547*m, 0.31181*m, 0.37829*m},
  {"SB4-6",  1.26663*m,  0.03940*m,  2.94437*m, -1.56487*rad, 0.30753*m, 0.28381*m, 0.32229*m},
  {"SB5-1", -1.14177*m,  0.40502*m,  3.18637*m,  1.59113*rad, 0.32395*m, 0.28689*m, 0.32672*m},
  {"SB5-2", -0.74682*m,  0.40834*m,  3.23738*m,  1.58557*rad, 0.39414*m, 0.34687*m, 0.44624*m},
  {"SB5-3", -0.18082*m,  0.41916*m,  3.18938*m,  1.60778*rad, 0.46110*m, 0.36211*m, 0.66879*m},
  {"SB5-4",  0.56560*m,  0.41613*m,  3.23738*m,  1.58003*rad, 0.49208*m, 0.41584*m, 0.80875*m},
  {"SB5-5",  1.20456*m,  0.40948*m,  3.18838*m,  1.59114*rad, 0.52843*m, 0.47594*m, 0.44869*m},
  {"SB6-1", -0.97316*m,  0.80254*m,  2.95038*m, -1.54896*rad, 0.51377*m, 0.46093*m, 0.54666*m},
  {"SB6-2", -0.41843*m,  0.83582*m,  2.99938*m, -1.54896*rad, 0.52282*m, 0.46194*m, 0.54766*m},
  {"SB6-3",  0.09273*m,  0.84353*m,  2.95038*m, -1.56562*rad, 0.51098*m, 0.47981*m, 0.45229*m},
  {"SB6-4",  0.60275*m,  0.84554*m,  2.99938*m, -1.56006*rad, 0.50624*m, 0.45784*m, 0.54577*m},
  {"SB6-5",  1.16083*m,  0.84724*m,  2.95038*m, -1.54896*rad, 0.51477*m, 0.45793*m, 0.54666*m}
};

//__Scintillator Constructor____________________________________________________________________
Scintillator::Scintillator(const std::string& input_name,
                           const double input_height,
                           const double input_minwidth,
                           const double input_maxwidth)
    : name(input_name), height(input_height), minwidth(input_minwidth), maxwidth(input_maxwidth),
      lvolume(nullptr), pvolume(nullptr), sensitive(nullptr) {

  auto solid = Construction::Trap(name, height, minwidth, maxwidth, Depth);

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

  Material::Scintillator = new G4Material("Scintillator", 1.032*g/cm3, 2);
  Material::Scintillator->AddElement(Construction::Material::C, 9);
  Material::Scintillator->AddElement(Construction::Material::H, 10);

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
