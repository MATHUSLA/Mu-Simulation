#ifndef ACTION_EVENTACTION_HH
#define ACTION_EVENTACTION_HH
#pragma once

#include "Geant4/globals.hh"
#include "Geant4/G4UserEventAction.hh"

namespace MATHUSLA { namespace MU {

class EventAction : public G4UserEventAction {
 public:
  EventAction();
  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);
};

} } /* namespace MATHUSLA::MU */

#endif /* ACTION_EVENTACTION_HH */
