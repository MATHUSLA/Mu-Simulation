/* src/action/EventAction.cc
 *
 * Copyright 2018 Brandon Gomes
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "action.hh"

#include <G4MTRunManager.hh>
#include <tls.hh>

namespace MATHUSLA { namespace MU {

namespace { ////////////////////////////////////////////////////////////////////////////////////
//__Printing Frequency for Event Count__________________________________________________________
G4ThreadLocal size_t _print_modulo;
G4ThreadLocal uint_fast64_t _event_id{};
//----------------------------------------------------------------------------------------------
} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Event Action Constructor____________________________________________________________________
EventAction::EventAction(const size_t print_modulo) : G4UserEventAction() {
  _print_modulo = print_modulo;
}
//----------------------------------------------------------------------------------------------

//__Event Initialization________________________________________________________________________
void EventAction::BeginOfEventAction(const G4Event* event) {
  _event_id = event->GetEventID();
  std::cout << "\r  Event [ "
             + std::to_string(_event_id)
             + " ] @ ("
             + std::to_string(event->GetNumberOfPrimaryVertex())
             + " primaries)"
             + (!(_event_id % _print_modulo) ? "\n\n" : "");
}
//----------------------------------------------------------------------------------------------

//__Get Current Event___________________________________________________________________________
const G4Event* EventAction::GetEvent() {
  return G4RunManager::GetRunManager()->GetCurrentEvent();
}
//----------------------------------------------------------------------------------------------

//__Get Current RunID___________________________________________________________________________
size_t EventAction::EventID() {
  return _event_id;
}
//----------------------------------------------------------------------------------------------

} } /* namespace MATHUSLA::MU */
