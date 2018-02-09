#include "action/StackingAction.hh"

namespace MATHUSLA { namespace MU {

StackingAction::StackingAction() : G4UserStackingAction() {}

StackingAction::~StackingAction() {}

G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track* track) {
  return G4UserStackingAction::ClassifyNewTrack(track);
}

void StackingAction::NewStage() {
  G4UserStackingAction::NewStage();
}

void StackingAction::PrepareNewEvent() {
  G4UserStackingAction::PrepareNewEvent();
}

} } /* namespace MATHUSLA::MU */