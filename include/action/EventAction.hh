#ifndef ACTION_EVENTACTION_HH
#define ACTION_EVENTACTION_HH
#pragma once

#include "Geant4/globals.hh"
#include "Geant4/G4UserEventAction.hh"

namespace MATHUSLA { namespace MU {

class EventAction : public G4UserEventAction {
public:
  EventAction(G4int print_modulo);

  void BeginOfEventAction(const G4Event* event);
  void EndOfEventAction(const G4Event* event);

private:
  G4int _print_modulo;
};

} } /* namespace MATHUSLA::MU */

#endif /* ACTION_EVENTACTION_HH */
