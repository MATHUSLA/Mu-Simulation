#ifndef ACTION_RUNACTION_HH
#define ACTION_RUNACTION_HH
#pragma once

#include "Geant4/globals.hh"
#include "Geant4/G4UserRunAction.hh"
#include "Geant4/G4Run.hh"
#include "analysis/HistoManager.hh"

namespace MATHUSLA { namespace MU {

class RunAction : public G4UserRunAction {
public:
  RunAction();

  G4Run* GenerateRun();

  void BeginOfRunAction(const G4Run* run);
  void FillPerEvent(G4double Edep);
  void EndOfRunAction(const G4Run* run);

private:
  G4double fAvgEdep;
};

} } /* namespace MATHUSLA::MU */

#endif /* ACTION_RUNACTION_HH */
