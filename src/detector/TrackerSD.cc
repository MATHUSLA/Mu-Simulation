#include "detector/TrackerSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4SDParticleFilter.hh"
#include "G4ios.hh"

namespace MATHUSLA {

TrackerSD::TrackerSD(const G4String& name, const G4String& hitsCollectionName)
    : G4VSensitiveDetector(name),
      fHitsCollection(NULL) {

  collectionName.insert(hitsCollectionName);
}

TrackerSD::~TrackerSD() {}

void TrackerSD::Initialize(G4HCofThisEvent* hce) {
  // Create hits collection
  fHitsCollection = new TrackerHitsCollection(SensitiveDetectorName, collectionName[0]);

  // Add this collection in hce

  G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection(hcID, fHitsCollection);
}

G4bool TrackerSD::ProcessHits(G4Step* step, G4TouchableHistory* history) {
  G4double edep = step->GetTotalEnergyDeposit();

  if (edep == 0.) return false;

  TrackerHit* newHit = new TrackerHit();

  newHit->SetTrackID(step->GetTrack()->GetTrackID());
  newHit->SetChamberNb(step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
  newHit->SetEdep(edep);
  newHit->SetTime(step->GetPostStepPoint()->GetGlobalTime());
  newHit->SetPos(step->GetPostStepPoint()->GetPosition());

  fHitsCollection->insert(newHit);

  return true;
}

void TrackerSD::EndOfEvent(G4HCofThisEvent*) {
  if (verboseLevel > 1) {
     G4int nofHits = fHitsCollection->entries();
     G4cout << "\n-------->Hits Collection: in this event they are "
            << nofHits << " hits in the tracker chambers: \n";
     for (G4int i = 0; i < nofHits; i++)
        (*fHitsCollection)[i]->Print();
  }
}

} /* namespace MATHUSLA */
