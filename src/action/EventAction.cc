#include "action/EventAction.hh"

#include "Geant4/G4Event.hh"

namespace MATHUSLA { namespace MU {

EventAction::EventAction(G4int print_modulo)
    : G4UserEventAction(), _print_modulo(print_modulo) {}

void EventAction::BeginOfEventAction(const G4Event* event) {
  const auto eventID = event->GetEventID();
  if (eventID && !(eventID % _print_modulo))
    G4cout << "\n\n [ Beginning of Event " << eventID << " ]\n\n";
}

void EventAction::EndOfEventAction(const G4Event*) {}

} } /* namespace MATHUSLA::MU */
