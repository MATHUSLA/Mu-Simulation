#include "action/EventAction.hh"

#include "Geant4/G4Event.hh"
#include "Geant4/G4SystemOfUnits.hh"

#include "analysis/Analysis.hh"
#include "tracking/PrototypeHit.hh"

namespace MATHUSLA { namespace MU {

EventAction::EventAction(RunAction* action)
    : G4UserEventAction(), _run_action(action), _energy(0),
      _print_modulo(100) {}

void EventAction::BeginOfEventAction(const G4Event* event) {
  _energy = 0;
  auto eventID = event->GetEventID();
  if (eventID && !(eventID % _print_modulo))
    G4cout << "\n\n\n [ Beginning of event: " << eventID << " ]\n\n\n";
}

void EventAction::EndOfEventAction(const G4Event* event) {
  auto hit_collections = event->GetHCofThisEvent();
  auto collection_count = hit_collections->GetNumberOfCollections();

  for (size_t i = 0; i < collection_count; ++i) {
    auto collection = dynamic_cast<PrototypeHC*>(hit_collections->GetHC(i));

    auto hit_count = collection->GetSize();
    for (size_t i = 0; i < collection->GetSize(); ++i) {
      auto hit = dynamic_cast<PrototypeHit*>(collection->GetHit(i));

      auto analysis = G4AnalysisManager::Instance();
      _energy = hit->GetTotalDeposit();
      _run_action->FillPerEvent(_energy);
      analysis->FillH1(1, _energy);
    }
  }
}

} } /* namespace MATHUSLA::MU */
