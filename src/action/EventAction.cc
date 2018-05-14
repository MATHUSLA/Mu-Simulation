#include "action.hh"

#include <unordered_set>

namespace MATHUSLA { namespace MU {

namespace {
G4ThreadLocal int _print_modulo;
//----------------------------------------------------------------------------------------------
} /* anonymous namespace */

EventAction::EventAction(int print_modulo) : G4UserEventAction() {
  _print_modulo = print_modulo;
}
//----------------------------------------------------------------------------------------------

void EventAction::BeginOfEventAction(const G4Event* event) {
  const auto eventID = event->GetEventID();
  if (eventID && !(eventID % _print_modulo))
    std::cout << "\n\n [ Beginning of Event " << eventID << " ]\n\n";
}
//----------------------------------------------------------------------------------------------

void EventAction::EndOfEventAction(const G4Event*) {}
//----------------------------------------------------------------------------------------------

} } /* namespace MATHUSLA::MU */
