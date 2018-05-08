#include "action/EventAction.hh"

#include "Geant4/G4Event.hh"

#include <unordered_set>

namespace MATHUSLA { namespace MU {

EventAction::EventAction(int print_modulo)
    : G4UserEventAction(), _print_modulo(print_modulo) {}

void EventAction::BeginOfEventAction(const G4Event* event) {
  const auto eventID = event->GetEventID();
  if (eventID && !(eventID % _print_modulo))
    std::cout << "\n\n [ Beginning of Event " << eventID << " ]\n\n";
}

void EventAction::EndOfEventAction(const G4Event* event) {
  /* TODO: find a place to put this
  const auto& trajectories = *event->GetTrajectoryContainer()->GetVector();
  std::unordered_set<std::string> particles;
  std::cout << "Iteraction Products: \n";
  for (const auto& trajectory : trajectories) {
    const auto& name = trajectory->GetParticleName();
    if (particles.count(name)) continue;
    std::cout << "  " << name << "\n";
    particles.insert(name);
  }
  std::cout << "\n";
  */
}

} } /* namespace MATHUSLA::MU */
