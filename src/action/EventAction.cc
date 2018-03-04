#include "action/EventAction.hh"

#include "Geant4/G4Event.hh"

#include "analysis/HistoManager.hh"
#include "tracking/PrototypeHit.hh"

namespace MATHUSLA { namespace MU {

EventAction::EventAction(RunAction* action)
    : G4UserEventAction(), fRunAction(action), fEnergy(0), fPrintModulo(100) {}

void EventAction::BeginOfEventAction(const G4Event*event) {
  auto eventID = event->GetEventID();
  if (eventID % fPrintModulo == 0)
    G4cout << "\n---> Begin of event: " << eventID << '\n';

  // initialisation per event
  fEnergy = 0;
}

void EventAction::EndOfEventAction(const G4Event* event) {
  auto hit_collections = event->GetHCofThisEvent();
  auto collection_count = hit_collections->GetNumberOfCollections();

  for (size_t i = 0; i < collection_count; ++i) {
    auto collection = dynamic_cast<PrototypeHC*>(hit_collections->GetHC(i));
    fRunAction->FillPerEvent(fEnergy);
    HistoManager::FillHisto(0, fEnergy);
  }
}

} } /* namespace MATHUSLA::MU */
