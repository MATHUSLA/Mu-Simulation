#ifndef PHYSICS_UNITS_HH
#define PHYSICS_UNITS_HH
#pragma once

#include "Geant4/G4UnitsTable.hh"
#include "Geant4/G4SystemOfUnits.hh"
#include "Geant4/G4PhysicalConstants.hh"

namespace MATHUSLA { namespace MU {

void DefineNewUnits() {
  new G4UnitDefinition("GeV/c", "GeV/c", "Momentum", GeV*c_light);
  new G4UnitDefinition("MeV/c", "MeV/c", "Momentum", MeV*c_light);
  new G4UnitDefinition("keV/c", "keV/c", "Momentum", keV*c_light);
  new G4UnitDefinition( "eV/c",  "eV/c", "Momentum",  eV*c_light);
}

} } /* namespace MATHUSLA::MU */

#endif /* PHYSICS_UNITS_HH */