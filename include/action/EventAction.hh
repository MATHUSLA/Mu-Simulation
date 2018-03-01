#ifndef ACTION_EVENTACTION_HH
#define ACTION_EVENTACTION_HH
#pragma once

#include "tracking/PrototypeHit.hh"
#include "analysis/HistoManager.hh"
#include "action/RunAction.hh"

#include "Geant4/globals.hh"
#include "Geant4/G4UserEventAction.hh"

namespace MATHUSLA { namespace MU {

class EventAction : public G4UserEventAction 
{
 public:
  EventAction(RunAction*, HistoManager*);
  		~EventAction();

  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);

  void AddE(G4double de) {fEnergy += de;};

 private:
   RunAction*    fRunAct;
   HistoManager* fHistoManager;

   G4double fEnergy;

   G4int fPrintModulo;    
};

} } /* namespace MATHUSLA::MU */

#endif /* ACTION_EVENTACTION_HH */
