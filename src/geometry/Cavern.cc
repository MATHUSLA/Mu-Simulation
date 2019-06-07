/* src/geometry/Cavern.cc
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

#include "geometry/Cavern.hh"

#include <Geant4/G4IntersectionSolid.hh>
#include <Geant4/G4UnionSolid.hh>
#include <Geant4/G4SubtractionSolid.hh>

#include "geometry/Construction.hh"

namespace MATHUSLA { namespace MU {

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__Cavern Dimensions___________________________________________________________________________
static auto _base_depth = Cavern::DefaultBaseDepth;
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

namespace Cavern { /////////////////////////////////////////////////////////////////////////////

//__Cavern Materials_____________________________________________________________________________
G4Material* Material::Steel = nullptr;
//----------------------------------------------------------------------------------------------

//__Define Cavern Materials_____________________________________________________________________
void Material::Define() {
  Steel = Construction::Material::Iron;
}
//----------------------------------------------------------------------------------------------

//__Cavern Dimensions___________________________________________________________________________
long double BaseDepth() {
  return _base_depth - Earth::TotalShift();
}
long double BaseDepth(long double value) {
  _base_depth = value;
  return BaseDepth();
}
long double TopDepth() {
  return BaseDepth() - CavernHeight;
}
long double CenterDepth() {
  return BaseDepth() - 0.5 * CavernHeight;
}
long double IP() {
  return BaseDepth() - DetectorHeight;
}
//----------------------------------------------------------------------------------------------

//__Cavern Rotations____________________________________________________________________________
long double cosP1Tilt() {
  return std::cos(P1ForwardTilt / rad);
}
long double sinP1Tilt() {
  return std::sin(P1ForwardTilt / rad);
}
long double rotate_from_P1_x(long double x, long double z) {
  return x * cosP1Tilt() + z * sinP1Tilt();
}
long double rotate_from_P1_z(long double x, long double z) {
  return z * cosP1Tilt() - x * sinP1Tilt();
}
std::pair<long double, long double> rotate_from_P1(long double x, long double z) {
  const auto cosine = cosP1Tilt();
  const auto sine = sinP1Tilt();
  return std::make_pair(x * cosine + z * sine, z * cosine - x * sine);
}
long double rotate_to_P1_x(long double x, long double z) {
  return x * cosP1Tilt() - z * sinP1Tilt();
}
long double rotate_to_P1_z(long double x, long double z) {
  return z * cosP1Tilt() + x * sinP1Tilt();
}
std::pair<long double, long double> rotate_to_P1(long double x, long double z) {
  const auto cosine = cosP1Tilt();
  const auto sine = sinP1Tilt();
  return std::make_pair(x * cosine - z * sine, z * cosine + x * sine);
}
//----------------------------------------------------------------------------------------------

//__Cavern Logical Volumes______________________________________________________________________
G4LogicalVolume* Volume() {
  auto box = Construction::Box("CavernBox", CavernLength, CavernWidth, CavernHeight - VaultRadius);

  auto intersection_box = Construction::Box("VaultIntersectionBox", CavernLength, CavernWidth, CavernHeight);
  auto intersection_cylinder = Construction::Cylinder("VaultIntersectionCylinder", CavernLength, 0.0, VaultRadius);
  auto vault = new G4IntersectionSolid("CavernVault",
    intersection_box,
    intersection_cylinder,
    G4Translate3D(0, 0, VaultRadius - CavernHeight / 2.0)
      * Construction::Rotate(0.0, 1.0, 0.0, 90.0 * deg));

  return Construction::Volume(new G4UnionSolid("Cavern",
    vault,
    box,
    G4Translate3D(0, 0, CavernHeight / 2.0 - (CavernHeight - VaultRadius) / 2.0)));
}

G4LogicalVolume* RingVolume() {
  return Construction::Volume(Construction::Cylinder("DetectorRing",
           DetectorLength, DetectorRadius - SteelThickness, DetectorRadius),
           Material::Steel,
           Construction::CasingAttributes());
}
//----------------------------------------------------------------------------------------------

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__Check Between-Ness__________________________________________________________________________
bool _between(const double min_layer,
              const double max_layer,
              const double target) {
  return min_layer < target && target < max_layer;
}
//----------------------------------------------------------------------------------------------

//__Calculate Subtraction of Volumes____________________________________________________________
G4LogicalVolume* _calculate_modification(const std::string& name,
                                         G4LogicalVolume* earth_component,
                                         const double base_depth,
                                         const double top_depth) {
  return Construction::Volume(new G4SubtractionSolid(name,
    earth_component->GetSolid(),
    Volume()->GetSolid(),
    Construction::Transform(0, 1.7 * m, -0.5 * (base_depth - top_depth) + CenterDepth() - top_depth)),
    earth_component->GetMaterial());
}
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Construct Cavern____________________________________________________________________________
G4VPhysicalVolume* Construct(G4LogicalVolume* world) {
  Material::Define();
  Earth::Material::Define();

  auto earth = Earth::Volume();

  const auto mix_top = Earth::TotalDepth() - Earth::MixDepth();
  const auto marl_top = mix_top - Earth::MarlDepth();
  const auto sandstone_top = marl_top - Earth::SandstoneDepth();

  const auto modify_mix = _between(mix_top, mix_top + Earth::MixDepth(), BaseDepth())
                       || _between(TopDepth(), BaseDepth(), mix_top)
                       || _between(TopDepth(), BaseDepth(), mix_top + Earth::MixDepth());
  const auto modify_marl = _between(marl_top, marl_top + Earth::MarlDepth(), BaseDepth())
                        || _between(TopDepth(), BaseDepth(), marl_top)
                        || _between(TopDepth(), BaseDepth(), marl_top + Earth::MarlDepth());
  const auto modify_sandstone = _between(sandstone_top, sandstone_top + Earth::SandstoneDepth(), BaseDepth())
                             || _between(TopDepth(), BaseDepth(), sandstone_top)
                             || _between(TopDepth(), BaseDepth(), sandstone_top + Earth::SandstoneDepth());

  if (modify_mix) {
    Construction::PlaceVolume(
      _calculate_modification("modified_mix", Earth::MixVolume(),
                              mix_top + Earth::MixDepth(), mix_top),
      earth, Earth::MixTransform());
  } else {
    Construction::PlaceVolume(Earth::MixVolume(), earth, Earth::MixTransform());
  }

  if (modify_marl) {
    Construction::PlaceVolume(
      _calculate_modification("modified_marl", Earth::MarlVolume(),
                              marl_top + Earth::MarlDepth(), marl_top),
      earth, Earth::MarlTransform());
  } else {
    Construction::PlaceVolume(Earth::MarlVolume(), earth, Earth::MarlTransform());
  }

  if (modify_sandstone) {
    Construction::PlaceVolume(
      _calculate_modification("modified_sandstone", Earth::SandstoneVolume(),
                              sandstone_top + Earth::SandstoneDepth(), sandstone_top),
      earth, Earth::SandstoneTransform());
  } else {
    Construction::PlaceVolume(Earth::SandstoneVolume(), earth, Earth::SandstoneTransform());
  }

  Construction::PlaceVolume(RingVolume(), earth,
    G4Translate3D(0, 0, -0.5 * Earth::TotalDepth() + IP())
      * Construction::Rotate(0, 1, 0, 90*deg));
  return Construction::PlaceVolume(earth, world, Earth::Transform());
}
//----------------------------------------------------------------------------------------------

} /* namespace Cavern */ ///////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
