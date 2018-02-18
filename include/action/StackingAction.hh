#ifndef ACTION_STACKINGACTION_HH
#define ACTION_STACKINGACTION_HH
#pragma once

#include "Geant4/G4UserStackingAction.hh"
#include "Geant4/G4ClassificationOfNewTrack.hh"

namespace MATHUSLA { namespace MU {

class StackingAction : public G4UserStackingAction {
 public:
  StackingAction();
  G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);
  void NewStage();
  void PrepareNewEvent();

 private:
  // G4StackManager* fpStackManager;
};

} } /* namespace MATHUSLA::MU */

#endif /* ACTION_STACKINGACTION_HH */