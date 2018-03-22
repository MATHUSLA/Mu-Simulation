#ifndef ACTION_STEPPINGACTION_H
#define ACTION_STEPPINGACTION_H
#pragma once

#include "Geant4/G4UserSteppingAction.hh"
#include "Geant4/G4Step.hh"

namespace MATHUSLA { namespace MU {

class SteppingAction : public G4UserSteppingAction {
public:
  SteppingAction();
  void UserSteppingAction(const G4Step* step);
private:
  // G4SteppingManager* _manager;
};

} } /* namespace MATHUSLA::MU */

#endif /* ACTION_STEPPINGACTION_H */
