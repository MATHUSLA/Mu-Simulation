#include "tracking/TrackerHit.hh"

#include <iomanip>

#include "Geant4/G4UnitsTable.hh"
#include "Geant4/G4VVisManager.hh"
#include "Geant4/G4Circle.hh"
#include "Geant4/G4Colour.hh"
#include "Geant4/G4VisAttributes.hh"

namespace MATHUSLA {

G4ThreadLocal G4Allocator<TrackerHit>* TrackerHitAllocator = 0;

TrackerHit::TrackerHit()
    : G4VHit(),
      fTrackID(-1),
      fChamber(-1),
      fDeposit(0.),
      fTime(0.),
      fPos(G4ThreeVector()) {}

TrackerHit::~TrackerHit() {}

TrackerHit::TrackerHit(const TrackerHit& right) : G4VHit() {
  fTrackID = right.fTrackID;
  fChamber = right.fChamber;
  fDeposit = right.fDeposit;
  fTime    = right.fTime;
  fPos     = right.fPos;
}

const TrackerHit& TrackerHit::operator=(const TrackerHit& right) {
  fTrackID = right.fTrackID;
  fChamber = right.fChamber;
  fDeposit = right.fDeposit;
  fTime    = right.fTime;
  fPos     = right.fPos;

  return *this;
}

G4int TrackerHit::operator==(const TrackerHit& right) const {
  return ( this == &right ) ? 1 : 0;
}

void TrackerHit::Draw() {
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if (pVVisManager) {
    G4Circle circle(fPos);
    circle.SetScreenSize(4.);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(1., 0., 0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
}

void TrackerHit::Print() {
  G4cout
    << " trackID: "                        << fTrackID
    << " chamber: "                        << fChamber
    << " Energy Deposit: " << std::setw(7) << G4BestUnit(fDeposit, "Energy")
    << " Time: "           << std::setw(7) << G4BestUnit(fTime, "Time")
    << " Position: "       << std::setw(7) << G4BestUnit(fPos, "Length")
    << "\n";
}

} /* namespace MATHUSLA */
