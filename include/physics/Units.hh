#ifndef PHYSICS_UNITS_HH
#define PHYSICS_UNITS_HH
#pragma once

#include "Geant4/G4UnitsTable.hh"
#include "Geant4/G4SystemOfUnits.hh"
#include "Geant4/G4PhysicalConstants.hh"

namespace MATHUSLA { namespace MU {

const auto GeVperC = GeV;
const auto MeVperC = MeV;
const auto keVperC = keV;
const auto  eVperC =  eV;

inline void DefineNewUnits() {
  new G4UnitDefinition("GeV/c", "GeV/c", "Momentum", GeVperC);
  new G4UnitDefinition("MeV/c", "MeV/c", "Momentum", MeVperC);
  new G4UnitDefinition("keV/c", "keV/c", "Momentum", keVperC);
  new G4UnitDefinition( "eV/c",  "eV/c", "Momentum",  eVperC);
}

} } /* namespace MATHUSLA::MU */

#endif /* PHYSICS_UNITS_HH */
