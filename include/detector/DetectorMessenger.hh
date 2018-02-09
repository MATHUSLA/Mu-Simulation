#ifndef DETECTORMESSENGER_HH
#define DETECTORMESSENGER_HH
#pragma once

#include "Geant4/globals.hh"
#include "Geant4/G4UImessenger.hh"
#include "Geant4/G4UIdirectory.hh"
#include "Geant4/G4UIcmdWithAString.hh"
#include "Geant4/G4UIcmdWithADoubleAndUnit.hh"

#include "DetectorConstruction.hh"

namespace MATHUSLA { namespace MU {

class DetectorConstruction;

class DetectorMessenger: public G4UImessenger {
 public:
  explicit DetectorMessenger(DetectorConstruction*);
  virtual ~DetectorMessenger();

  virtual void SetNewValue(G4UIcommand*, G4String);

 private:
  DetectorConstruction* fDetectorConstruction;

  G4UIdirectory* fDirectory;
  G4UIdirectory* fDetDirectory;

  G4UIcmdWithAString* fTargMatCmd;
  G4UIcmdWithAString* fChamMatCmd;

  G4UIcmdWithADoubleAndUnit* fStepMaxCmd;
};

} } /* namespace MATHUSLA::MU */

#endif /* DETECTORMESSENGER_HH */
