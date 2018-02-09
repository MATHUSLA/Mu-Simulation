#include "detector/TrackerSD.hh"

#include "Geant4/G4HCofThisEvent.hh"
#include "Geant4/G4ios.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4SDParticleFilter.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4ThreeVector.hh"

namespace MATHUSLA { namespace MU {

TrackerSD::TrackerSD(const G4String& name, const G4String& hitsCollectionName)
    : G4VSensitiveDetector(name), fHitsCollection(NULL) {
  collectionName.insert(hitsCollectionName);
}

TrackerSD::~TrackerSD() {}

void TrackerSD::Initialize(G4HCofThisEvent* eventhc) {
  fHitsCollection = new TrackerHitsCollection(SensitiveDetectorName, collectionName[0]);
  eventhc->AddHitsCollection(
    G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]),
    fHitsCollection);
}

G4bool TrackerSD::ProcessHits(G4Step* step, G4TouchableHistory*) {
  G4double energy_deposit = step->GetTotalEnergyDeposit();

  if (energy_deposit == 0.) return false;

  G4Track* track = step->GetTrack();
  G4int parentID = track->GetParentID();

  if (parentID == 0) {
    fHitsCollection->insert(
      new TrackerHit(
        track->GetParticleDefinition()->GetParticleName(),
        track->GetTrackID(),
        step->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber(),
        energy_deposit,
        step->GetPostStepPoint()->GetGlobalTime(),
        step->GetPostStepPoint()->GetPosition()));
    return true;
  }
  return false;
}

void TrackerSD::EndOfEvent(G4HCofThisEvent*) {
  if (verboseLevel > 1) {
    G4int nofHits = fHitsCollection->entries();
    G4cout << "\n--------> Hits Collection: in this event there are "
           << nofHits << " hits in the tracker chambers: \n";
    G4int chamberID = -1, trackID = -1;
    for (G4int i = 0; i < nofHits; i++) {
      auto hit = static_cast<TrackerHit*>(fHitsCollection->GetHit(i));
      G4int newChamberID = hit->GetChamberID();
      G4int newTrackID = hit->GetTrackID();
      if (chamberID != newChamberID || trackID != newTrackID) {
        G4cout << /* std::string(129 + hit->GetParticleName().size(), '-') << */ '\n';
        chamberID = newChamberID;
        trackID = newTrackID;
      }
      hit->Print();
    }
  }
}

} } /* namespace MATHUSLA::MU */
