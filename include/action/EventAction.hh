#ifndef ACTION_EVENTACTION_HH
#define ACTION_EVENTACTION_HH
#pragma once

#include "tracking/PrototypeHit.hh"
#include "action/RunAction.hh"

#include "Geant4/globals.hh"
#include "Geant4/G4UserEventAction.hh"

#include "action/RunAction.hh"

namespace MATHUSLA { namespace MU {

class EventAction : public G4UserEventAction {
public:
  EventAction(RunAction* action);

  void BeginOfEventAction(const G4Event* event);
  void EndOfEventAction(const G4Event* event);

  void AddEnergy(G4double energy) { _energy += energy; }

private:
  RunAction* _run_action;
  G4double   _energy;
  G4int      _print_modulo;
};

} } /* namespace MATHUSLA::MU */

#endif /* ACTION_EVENTACTION_HH */
