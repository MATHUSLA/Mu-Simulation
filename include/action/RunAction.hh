#ifndef MU__ACTION_RUNACTION_HH
#define MU__ACTION_RUNACTION_HH
#pragma once

#include "Geant4/G4UserRunAction.hh"
#include "Geant4/G4Run.hh"

namespace MATHUSLA { namespace MU {

class RunAction : public G4UserRunAction {
public:
  void BeginOfRunAction(const G4Run* run);
  void EndOfRunAction(const G4Run* run);
};

} } /* namespace MATHUSLA::MU */

#endif /* MU__ACTION_RUNACTION_HH */
