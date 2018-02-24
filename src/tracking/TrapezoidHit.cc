#include "tracking/TrapezoidHit.hh"

#include <iomanip>

#include "Geant4/G4Circle.hh"
#include "Geant4/G4Colour.hh"
#include "Geant4/G4UnitsTable.hh"
#include "Geant4/G4VisAttributes.hh"
#include "Geant4/G4VVisManager.hh"

namespace MATHUSLA { namespace MU {

G4ThreadLocal G4Allocator<TrapezoidHit>* TrapezoidHitAllocator = 0;

TrapezoidHit::TrapezoidHit(const G4String& particle,
                           const G4int track,
                           const G4String& chamber,
                           const G4double deposit,
                           const G4double time,
                           const G4ThreeVector position,
                           const G4double energy,
                           const G4ThreeVector momentum)
    : G4VHit(), fParticleName(particle), fTrackID(track), fChamberID(chamber),
                fDeposit(deposit), fTime(time), fPosition(position),
                fEnergy(energy), fMomentum(momentum) {}

void TrapezoidHit::Draw() {
  auto pVVisManager = G4VVisManager::GetConcreteInstance();
  if (pVVisManager) {
    G4Circle point(fPosition);
    point.SetScreenSize(4.);
    point.SetFillStyle(G4Circle::filled);
    point.SetVisAttributes(G4VisAttributes(G4Colour(1., 1., 1.)));
    pVVisManager->Draw(point);
  }
}

void TrapezoidHit::Print() {
  G4cout << " "            << fParticleName
         << " | Track: "   << fTrackID
         << " | " /*" | Chamber: "*/ << fChamberID
         << " | Deposit: "
           << std::setw(9) << G4BestUnit(fDeposit, "Energy")
         << " | ["  //" | Position : ["  <- labels?
           << std::setw(9) << G4BestUnit(fTime,        "Time") << " "
           << std::setw(9) << G4BestUnit(fPosition[0], "Length")
           << std::setw(9) << G4BestUnit(fPosition[1], "Length")
           << std::setw(9) << G4BestUnit(fPosition[2], "Length")
         << "]"
         << " | ["  //" | Momentum : ["  <- labels?
           << std::setw(9) << G4BestUnit(fEnergy,      "Momentum")
           << std::setw(9) << G4BestUnit(fMomentum[0], "Momentum")
           << std::setw(9) << G4BestUnit(fMomentum[1], "Momentum")
           << std::setw(9) << G4BestUnit(fMomentum[2], "Momentum")
         << " ]"
    << "\n";
}

} } /* namespace MATHUSLA::MU */
