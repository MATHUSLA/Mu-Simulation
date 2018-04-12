#include "tracking/PrototypeHit.hh"

#include <iomanip>

#include "physics/Units.hh"
#include "ui/VIS.hh"

namespace MATHUSLA { namespace MU {

G4ThreadLocal G4Allocator<PrototypeHit>* PrototypeHitAllocator = nullptr;

PrototypeHit::PrototypeHit(const std::string& particle,
                           const int track,
                           const int parent,
                           const std::string& chamber,
                           const double deposit,
                           const G4LorentzVector position,
                           const G4LorentzVector momentum)
    : G4VHit(), _particle(particle), _trackID(track), _parentID(parent),
      _chamberID(chamber), _deposit(deposit), _position(position),
      _momentum(momentum) {}

void PrototypeHit::Draw() {
  VIS::Draw(VIS::Circle(_position.vect(), 4, G4Color::White()));
}

void PrototypeHit::Print() {
  std::cout << " "            << _particle
            << " | "          << _trackID
            << " | "          << _parentID
            << " | "          << _chamberID
            << " | Deposit: " << std::setw(9) << G4BestUnit(_deposit, "Energy")
            << " | ["
              << std::setw(9) << G4BestUnit(_position.t(), "Time") << " "
              << std::setw(9) << G4BestUnit(_position.x(), "Length")
              << std::setw(9) << G4BestUnit(_position.y(), "Length")
              << std::setw(9) << G4BestUnit(_position.z(), "Length")
            << "] | ["
              << std::setw(9) << G4BestUnit(_momentum.e(),  "Energy")
              << std::setw(9) << G4BestUnit(_momentum.px(), "Momentum")
              << std::setw(9) << G4BestUnit(_momentum.py(), "Momentum")
              << std::setw(9) << G4BestUnit(_momentum.pz(), "Momentum")
            << " ]"
            << "\n";
}

} } /* namespace MATHUSLA::MU */
