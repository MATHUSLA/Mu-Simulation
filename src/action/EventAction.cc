#include "action.hh"

#include <unordered_set>

namespace MATHUSLA { namespace MU {

namespace { ////////////////////////////////////////////////////////////////////////////////////
//__Printing Frequency for Event Count__________________________________________________________
G4ThreadLocal int _print_modulo;
//----------------------------------------------------------------------------------------------
} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Event Action Constructor____________________________________________________________________
EventAction::EventAction(int print_modulo) : G4UserEventAction() {
  _print_modulo = print_modulo;
}
//----------------------------------------------------------------------------------------------

//__Event Initialization________________________________________________________________________
void EventAction::BeginOfEventAction(const G4Event* event) {
  const auto eventID = event->GetEventID();
  if (eventID && !(eventID % _print_modulo))
    std::cout << "\n\n [ Starting Event " << eventID << " ]\n\n";
}
//----------------------------------------------------------------------------------------------

} } /* namespace MATHUSLA::MU */
