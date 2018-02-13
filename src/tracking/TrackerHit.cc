#include "tracking/TrackerHit.hh"

#include <iomanip>

#include "Geant4/G4Track.hh"
#include "Geant4/G4Circle.hh"
#include "Geant4/G4Colour.hh"
#include "Geant4/G4UnitsTable.hh"
#include "Geant4/G4VisAttributes.hh"
#include "Geant4/G4VVisManager.hh"

namespace MATHUSLA { namespace MU {

G4ThreadLocal G4Allocator<TrackerHit>* TrackerHitAllocator = 0;

TrackerHit::TrackerHit(G4String particle,
                       G4int track,
                       G4int chamber,
                       G4double deposit,
                       G4double time,
                       G4ThreeVector position)
    : G4VHit(), fParticleName(particle), fTrackID(track), fChamberID(chamber),
                fDeposit(deposit), fTime(time), fPosition(position) {}

// FIXME: Optional constructor with all the behavior of the detector.
// Not sure if it should stay here or stay in TrackerSD.cc.
// At this point it is unused.
TrackerHit::TrackerHit(G4Step* step)
    : TrackerHit(step->GetTrack()->GetParticleDefinition()->GetParticleName(),
                 step->GetTrack()->GetTrackID(),
                 step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(),
                 step->GetTotalEnergyDeposit(),
                 step->GetPostStepPoint()->GetGlobalTime(),
                 step->GetPostStepPoint()->GetPosition()) {}

void TrackerHit::Draw() {
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if (pVVisManager) {
    G4Circle circle(fPosition);
    circle.SetScreenSize(5.);
    circle.SetFillStyle(G4Circle::filled);
    circle.SetVisAttributes(G4VisAttributes(G4Colour(1., 1., 1.)));
    pVVisManager->Draw(circle);
  }
}

void TrackerHit::Print() {
  G4cout << " " << fParticleName
         << " | Track: " << fTrackID
         << " | Chamber: " << fChamberID
         << " | Deposit: "
           << std::setw(9) << G4BestUnit(fDeposit, "Energy")
         << " | Position : ["
           << std::setw(9) << G4BestUnit(fTime, "Time") << " "
           << std::setw(9) << G4BestUnit(fPosition[0], "Length")
           << std::setw(9) << G4BestUnit(fPosition[1], "Length")
           << std::setw(9) << G4BestUnit(fPosition[2], "Length")
         << "]"
    << "\n";
}

} } /* namespace MATHUSLA::MU */
