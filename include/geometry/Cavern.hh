/* include/geometry/Cavern.hh
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

#ifndef MU__GEOMETRY_CAVERN_HH
#define MU__GEOMETRY_CAVERN_HH
#pragma once

#include "Earth.hh"

namespace MATHUSLA { namespace MU {

namespace Cavern { /////////////////////////////////////////////////////////////////////////////

//__Cavern Materials____________________________________________________________________________
struct Material {
  static G4Material* Steel;
  static void Define();
 private:
  Material();
};
//----------------------------------------------------------------------------------------------

//__Cavern Dimensions___________________________________________________________________________
constexpr auto SteelThickness = 1.85*m;
constexpr auto DetectorLength = 40*m;
constexpr auto DetectorRadius = 11*m;
constexpr auto DetectorHeight = 11*m;
constexpr auto TotalHeight    = 35*m;
long double BaseDepth();
long double BaseDepth(long double value);
long double TopDepth();
long double CenterDepth();
//----------------------------------------------------------------------------------------------

//__Cavern Logical Volumes______________________________________________________________________
G4LogicalVolume* Volume();
G4LogicalVolume* RingVolume();
//----------------------------------------------------------------------------------------------

//__Construct Cavern and Earth__________________________________________________________________
G4VPhysicalVolume* Construct(G4LogicalVolume* world);
//----------------------------------------------------------------------------------------------

} /* namespace Cavern */ ///////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__GEOMETRY_CAVERN_HH */
