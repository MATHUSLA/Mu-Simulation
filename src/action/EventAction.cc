#include "action/EventAction.hh"

#include "Geant4/G4ios.hh"
#include "Geant4/G4Event.hh"
#include "Geant4/G4EventManager.hh"
#include "Geant4/G4TrajectoryContainer.hh"
#include "Geant4/G4Trajectory.hh"

namespace MATHUSLA {

EventAction::EventAction() : G4UserEventAction() {}

EventAction::~EventAction() {}

void EventAction::BeginOfEventAction(const G4Event*) {}

void EventAction::EndOfEventAction(const G4Event* event) {
  // get number of stored trajectories

  G4TrajectoryContainer* trajectoryContainer = event->GetTrajectoryContainer();
  G4int n_trajectories = 0;
  if (trajectoryContainer) n_trajectories = trajectoryContainer->entries();

  // periodic printing

  G4int eventID = event->GetEventID();
  if (eventID < 100 || eventID % 100 == 0) {
    G4cout << ">>> Event: " << eventID  << "\n";
    if (trajectoryContainer) {
      G4cout << "    " << n_trajectories
             << " trajectories stored in this event.\n";
    }
    G4VHitsCollection* hc = event->GetHCofThisEvent()->GetHC(0);
    G4cout << "    " << hc->GetSize() << " hits stored in this event\n";
  }
}

} /* namespace MATHUSLA */
