#include "ui/Messenger.hh"

namespace MATHUSLA { namespace MU {

Messenger::Messenger() : G4UImessenger() {}

Messenger::~Messenger() {}

void Messenger::SetNewValue(G4UIcommand* command, G4String value) {
  G4UImessenger::SetNewValue(command, value);
}

G4String Messenger::GetCurrentValue(G4UIcommand* command) {
  return G4UImessenger::GetCurrentValue(command);
}

} } /* namespace MATHUSLA::MU */
