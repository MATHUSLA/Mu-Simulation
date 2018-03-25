#ifndef UI_COMMAND_HH
#define UI_COMMAND_HH
#pragma once

#include "Geant4/G4UIcommand.hh"
#include "Geant4/G4UIdirectory.hh"
#include "Geant4/G4UIcmdWithoutParameter.hh"
#include "Geant4/G4UIcmdWithABool.hh"
#include "Geant4/G4UIcmdWithAnInteger.hh"
#include "Geant4/G4UIcmdWithADouble.hh"
#include "Geant4/G4UIcmdWithAString.hh"
#include "Geant4/G4UIcmdWith3Vector.hh"
#include "Geant4/G4UIcmdWithADoubleAndUnit.hh"
#include "Geant4/G4UIcmdWith3VectorAndUnit.hh"
#include "Geant4/G4UImessenger.hh"

namespace MATHUSLA { namespace MU {

using G4CMD_NoArg       = G4UIcmdWithoutParameter;
using G4CMD_Bool        = G4UIcmdWithABool;
using G4CMD_Integer     = G4UIcmdWithAnInteger;
using G4CMD_Double      = G4UIcmdWithADouble;
using G4CMD_String      = G4UIcmdWithAString;
using G4CMD_3Vector     = G4UIcmdWith3Vector;
using G4CMD_DoubleUnit  = G4UIcmdWithADoubleAndUnit;
using G4CMD_3VectorUnit = G4UIcmdWith3VectorAndUnit;

} } /* namespace MATHUSLA::MU */

#endif /* UI_COMMAND_HH */
