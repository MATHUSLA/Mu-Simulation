#include "action/SteppingAction.hh"

namespace MATHUSLA { namespace MU {

SteppingAction::SteppingAction() : G4UserSteppingAction() {}

SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step) {
  G4UserSteppingAction::UserSteppingAction(step);
}

} } /* namespace MATHUSLA::MU */
