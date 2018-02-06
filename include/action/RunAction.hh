#ifndef RUNACTION_HH
#define RUNACTION_HH
#pragma once

#include "Geant4/globals.hh"
#include "Geant4/G4UserRunAction.hh"
#include "Geant4/G4Run.hh"

namespace MATHUSLA {

class RunAction : public G4UserRunAction {
 public:
  RunAction();
  virtual ~RunAction();

  virtual G4Run* GenerateRun();
  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);
};

} /* namespace MATHUSLA */

#endif /* RUNACTION_HH */
