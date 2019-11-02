/*
 * include/physics/Units.hh
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

#ifndef MU__PHYSICS_UNITS_HH
#define MU__PHYSICS_UNITS_HH
#pragma once

#include <G4UnitsTable.hh>
#include <G4SystemOfUnits.hh>
#include <G4PhysicalConstants.hh>

namespace MATHUSLA { namespace MU {

//__Extention Momentum Units____________________________________________________________________
constexpr auto GeVperC = GeV;
constexpr auto MeVperC = MeV;
constexpr auto keVperC = keV;
constexpr auto  eVperC =  eV;
//----------------------------------------------------------------------------------------------

namespace Units { //////////////////////////////////////////////////////////////////////////////

//__Standard Units______________________________________________________________________________
constexpr auto Length   = cm;
constexpr auto Time     = ns;
constexpr auto Energy   = MeV;
constexpr auto Momentum = MeVperC;
constexpr auto Angle    = deg;
//----------------------------------------------------------------------------------------------

//__Standard Unit Names________________________________________________________________________
constexpr auto LengthString   = "cm";
constexpr auto TimeString     = "ns";
constexpr auto EnergyString   = "MeV";
constexpr auto MomentumString = "MeV/c";
constexpr auto AngleString    = "deg";
//----------------------------------------------------------------------------------------------

//__Install Momentum Units into Geant4__________________________________________________________
inline void Define() {
  new G4UnitDefinition("GeV/c", "GeV/c", "Momentum", GeVperC);
  new G4UnitDefinition("MeV/c", "MeV/c", "Momentum", MeVperC);
  new G4UnitDefinition("keV/c", "keV/c", "Momentum", keVperC);
  new G4UnitDefinition( "eV/c",  "eV/c", "Momentum",  eVperC);
}
//----------------------------------------------------------------------------------------------

//__Convert Unit To String______________________________________________________________________
inline std::string to_string(double value,
                             double unit,
                             const std::string& string) {
  return std::to_string(value / unit) + " " + string;
}
//----------------------------------------------------------------------------------------------

} /* namespace Units */ ////////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__PHYSICS_UNITS_HH */
