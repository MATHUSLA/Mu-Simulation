/* include/geometry/Earth.hh
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

#ifndef MU__GEOMETRY_EARTH_HH
#define MU__GEOMETRY_EARTH_HH
#pragma once

#include <Geant4/G4Material.hh>
#include <Geant4/G4VPhysicalVolume.hh>
#include <Geant4/G4LogicalVolume.hh>
#include <Geant4/G4Transform3D.hh>
#include <Geant4/G4SystemOfUnits.hh>

namespace MATHUSLA { namespace MU {

namespace Earth { //////////////////////////////////////////////////////////////////////////////

//__Earth Materials_____________________________________________________________________________
struct Material {
  static G4Material* CaCO3;
  static G4Material* Kaolinite;
  static G4Material* SiO2;
  static G4Material* Marl;
  static G4Material* Mix;
  static void Define();
 private:
  Material();
};
//----------------------------------------------------------------------------------------------

//__Earth Layer Sizes___________________________________________________________________________
long double LastShift();
long double LastShift(long double shift);
long double LayerWidthX();
long double LayerWidthX(long double value);
long double LayerWidthY();
long double LayerWidthY(long double value);
long double BufferZoneLength();
long double BufferZoneLength(long double value);
long double BufferZoneHigherWidth();
long double BufferZoneHigherWidth(long double value);
long double BufferZoneLowerWidth();
long double BufferZoneLowerWidth(long double value);
long double BufferZoneHigherDepth();
long double BufferZoneHigherDepth(long double value);
long double BufferZoneLowerDepth();
long double BufferZoneLowerDepth(long double value);
long double SandstoneDepth();
long double SandstoneDepth(long double value);
long double MarlDepth();
long double MarlDepth(long double value);
long double MixDepth();
long double MixDepth(long double value);
long double TotalShift();
long double TotalDepth();
//----------------------------------------------------------------------------------------------

//__Earth Logical Volumes_______________________________________________________________________
G4LogicalVolume* Volume();
G4LogicalVolume* SandstoneVolume();
G4LogicalVolume* MarlVolume();
G4LogicalVolume* MixVolume();
//----------------------------------------------------------------------------------------------

//__Earth Transformations_______________________________________________________________________
const G4Translate3D Transform();
const G4Translate3D SandstoneTransform();
const G4Translate3D MarlTransform();
const G4Translate3D MixTransform();
//----------------------------------------------------------------------------------------------

//__Construct Earth_____________________________________________________________________________
G4VPhysicalVolume* Construct(G4LogicalVolume* world);
//----------------------------------------------------------------------------------------------

} /* namespace Earth */ ////////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__GEOMETRY_EARTH_HH */
