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
  RunAction(HistoManager*);
  ~RunAction();
  G4Run* GenerateRun();
  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);

  void FillPerEvent(G4double);

private:
	HistoManager* fHistoManager;

	G4double fAvgEdep;
};

} } /* namespace MATHUSLA::MU */

#endif /* ACTION_RUNACTION_HH */
