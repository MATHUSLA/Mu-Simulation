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

#include "geometry/Construction.hh"

namespace MATHUSLA { namespace MU {

namespace Cavern { /////////////////////////////////////////////////////////////////////////////

//__Cavern Materials_____________________________________________________________________________
G4Material* Material::Steel = nullptr;
//----------------------------------------------------------------------------------------------

//__Define Cavern Materials_____________________________________________________________________
void Material::Define() {
  Steel = Construction::Material::Iron;
}
//----------------------------------------------------------------------------------------------

//__Cavern Logical Volumes______________________________________________________________________
G4LogicalVolume* Volume() {
  return nullptr;
}
G4LogicalVolume* RingVolume() {
  return Construction::Volume(Construction::Cylinder("DetectorRing",
           DetectorLength, DetectorRadius - SteelThickness, DetectorRadius),
           Material::Steel);
}
//----------------------------------------------------------------------------------------------

//__Cavern Transformations______________________________________________________________________
const G4Translate3D Transform() {
  return G4Translate3D(0, 0, 0.5L * TotalDepth);
}
const G4Translate3D RingTransform() {
  return G4Translate3D(0, 0, 0.5L * (SandstoneDepth - TotalDepth));
}
//----------------------------------------------------------------------------------------------

//__Construct Cavern____________________________________________________________________________
G4VPhysicalVolume* Construct(G4LogicalVolume* world) {
  Material::Define();
  auto cavern = Volume();
  Construction::PlaceVolume(RingVolume(), cavern, RingTransform());
  return Construction::PlaceVolume(cavern, world, Transform());
}
//----------------------------------------------------------------------------------------------

} /* namespace Cavern */ ///////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
