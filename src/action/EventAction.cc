#include "action/EventAction.hh"

#include "Geant4/G4ios.hh"
#include "Geant4/G4Event.hh"
#include "Geant4/G4EventManager.hh"
#include "Geant4/G4TrajectoryContainer.hh"
#include "Geant4/G4Trajectory.hh"

namespace MATHUSLA { namespace MU {

EventAction::EventAction(RunAction* run, HistoManager* histo) 
:G4UserEventAction(),
 fRunAct(run),fHistoManager(histo),
 fEnergy(0.),
 fPrintModulo(0)                             
{
 fPrintModulo = 100; }

 EventAction::~EventAction()
{}

void EventAction::BeginOfEventAction(const G4Event*event) 
{
  G4int eventNb = event->GetEventID();
  if (eventNb%fPrintModulo == 0) 
    G4cout << "\n---> Begin of event: " << eventNb << G4endl;
 
 // initialisation per event
 fEnergy = 0.;
}

void EventAction::EndOfEventAction(const G4Event* event) {
  /* TODO: reimplement
  
  auto trajectoryContainer = event->GetTrajectoryContainer();
  G4int n_trajectories = 0;
  if (trajectoryContainer) n_trajectories = trajectoryContainer->entries();

  auto eventID = event->GetEventID();
  if (eventID < 100 || eventID % 100 == 0) {
    G4cout << ">>> Event: " << eventID  << "\n";
    if (trajectoryContainer) {
      G4cout << "    " << n_trajectories
             << " trajectories stored in this event.\n";
    }
    auto hc = event->GetHCofThisEvent()->GetHC(0);
    G4cout << "    " << hc->GetSize() << " hits stored in this event\n";
  }
  */
  auto Collection = static_cast <PrototypeHC* > (event->GetHCofThisEvent()->GetHC(0));

  fRunAct->FillPerEvent(fEnergy);

  fHistoManager->FillHisto(0, fEnergy);
}

} } /* namespace MATHUSLA::MU */
