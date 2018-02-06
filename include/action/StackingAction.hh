#ifndef STACKINGACTION_HH
#define STACKINGACTION_HH
#pragma once

#include "Geant4/G4UserStackingAction.hh"
#include "Geant4/G4ClassificationOfNewTrack.hh"

namespace MATHUSLA {

class StackingAction : public G4UserStackingAction {
 public:
  StackingAction();
  virtual ~StackingAction();

  virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);
  virtual void NewStage();
  virtual void PrepareNewEvent();

 private:
  G4StackManager* fpStackManager;
};

} /* namespace MATHUSLA */

#endif /* STACKINGACTION_HH */