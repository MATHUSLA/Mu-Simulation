#include "tracking/PrototypeHit.hh"

#include <iomanip>

#include "Geant4/G4Circle.hh"
#include "Geant4/G4UnitsTable.hh"
#include "Geant4/G4VisAttributes.hh"
#include "Geant4/G4VVisManager.hh"

namespace MATHUSLA { namespace MU {

G4ThreadLocal G4Allocator<PrototypeHit>* PrototypeHitAllocator = 0;

PrototypeHit::PrototypeHit(const G4String& particle,
                           const G4int track,
                           const G4String& chamber,
                           const G4double ionizing,
                           const G4double nonionizing,
                           const G4double time,
                           const G4ThreeVector position,
                           const G4double energy,
                           const G4ThreeVector momentum)
    : G4VHit(), _particle_name(particle), _trackID(track), _chamberID(chamber),
                _ionizing(ionizing), _nonionizing(nonionizing), _time(time),
                _position(position), _energy(energy), _momentum(momentum) {}

void PrototypeHit::Draw() {
  auto pVVisManager = G4VVisManager::GetConcreteInstance();
  if (pVVisManager) {
    G4Circle point(_position);
    point.SetScreenSize(4.);
    point.SetFillStyle(G4Circle::filled);
    point.SetVisAttributes(G4VisAttributes(G4Colour(1., 1., 1.)));
    pVVisManager->Draw(point);
  }
}

void PrototypeHit::Print() {
  G4cout << " "            << _particle_name
         // << " | Track: "   << _trackID  <- necessary ?
         << " | Chamber: " << _chamberID
         << " | Deposit: "
           << std::setw(9) << G4BestUnit(_ionizing, "Energy")
           << std::setw(!_nonionizing ? 3 : 9) << G4BestUnit(_nonionizing, "Energy")
         << "| ["  //" | Position: ["  <- labels?
           << std::setw(9) << G4BestUnit(_time,        "Time") << " "
           << std::setw(9) << G4BestUnit(_position[0], "Length")
           << std::setw(9) << G4BestUnit(_position[1], "Length")
           << std::setw(9) << G4BestUnit(_position[2], "Length")
         << "] | ["  //" | Momentum: ["  <- labels?
           << std::setw(9) << G4BestUnit(_energy,      "Momentum")
           << std::setw(9) << G4BestUnit(_momentum[0], "Momentum")
           << std::setw(9) << G4BestUnit(_momentum[1], "Momentum")
           << std::setw(9) << G4BestUnit(_momentum[2], "Momentum")
         << " ]"
    << "\n";
}

} } /* namespace MATHUSLA::MU */
