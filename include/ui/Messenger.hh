#ifndef UI_MESSENGER_HH
#define UI_MESSENGER_HH
#pragma once

#include "Geant4/G4UImessenger.hh"
#include "Geant4/G4UIcommand.hh"

namespace MATHUSLA { namespace MU {

class Messenger: public G4UImessenger {
 public:
  Messenger();
  virtual ~Messenger();

  virtual void SetNewValue(G4UIcommand*, G4String);
  virtual G4String GetCurrentValue(G4UIcommand*);
};

} } /* namespace MATHUSLA::MU */

#endif /* UI_MESSENGER_HH */
