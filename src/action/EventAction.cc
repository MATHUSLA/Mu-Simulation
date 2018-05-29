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

#include <unordered_set>

namespace MATHUSLA { namespace MU {

namespace { ////////////////////////////////////////////////////////////////////////////////////
//__Printing Frequency for Event Count__________________________________________________________
G4ThreadLocal size_t _print_modulo;
//----------------------------------------------------------------------------------------------
} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Event Action Constructor____________________________________________________________________
EventAction::EventAction(const size_t print_modulo) : G4UserEventAction() {
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
