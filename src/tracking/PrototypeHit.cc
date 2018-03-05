#include "tracking/PrototypeHit.hh"

#include <iomanip>

#include "Geant4/G4Circle.hh"
#include "Geant4/G4UnitsTable.hh"
#include "Geant4/G4VisAttributes.hh"
#include "Geant4/G4VVisManager.hh"

namespace MATHUSLA { namespace MU {

G4ThreadLocal G4Allocator<PrototypeHit>* PrototypeHitAllocator = nullptr;

PrototypeHit::PrototypeHit(const G4String& particle,
                           const G4int track,
                           const G4String& chamber,
                           const G4double ionizing,
                           const G4double nonionizing,
                           const G4LorentzVector position,
                           const G4LorentzVector momentum)
    : G4VHit(), _particle(particle), _trackID(track), _chamberID(chamber),
                _ionizing(ionizing), _nonionizing(nonionizing),
                _position(position), _momentum(momentum) {}

void PrototypeHit::Draw() {
  auto pVVisManager = G4VVisManager::GetConcreteInstance();
  if (pVVisManager) {
    G4Circle point(_position.vect());
    point.SetScreenSize(4.);
    point.SetFillStyle(G4Circle::filled);
    point.SetVisAttributes(G4VisAttributes(G4Colour(1., 1., 1.)));
    pVVisManager->Draw(point);
  }
}

void PrototypeHit::Print() {
  G4cout << " " << _particle
         << " | Track: " << _trackID
         << " | " << _chamberID
         << " | Deposit: "
           << std::setw(9) << G4BestUnit(_ionizing, "Energy")
           << std::setw(!_nonionizing ? 3 : 9)
             << G4BestUnit(_nonionizing, "Energy")
         << "| ["
           << std::setw(9) << G4BestUnit(_position.t(), "Time") << " "
           << std::setw(9) << G4BestUnit(_position.x(), "Length")
           << std::setw(9) << G4BestUnit(_position.y(), "Length")
           << std::setw(9) << G4BestUnit(_position.z(), "Length")
         << "] | ["
           << std::setw(9) << G4BestUnit(_momentum.e(),  "Momentum")
           << std::setw(9) << G4BestUnit(_momentum.px(), "Momentum")
           << std::setw(9) << G4BestUnit(_momentum.py(), "Momentum")
           << std::setw(9) << G4BestUnit(_momentum.pz(), "Momentum")
         << " ]"
    << "\n";
}

} } /* namespace MATHUSLA::MU */
