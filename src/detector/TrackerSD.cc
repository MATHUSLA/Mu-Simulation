#include "detector/TrackerSD.hh"

#include "Geant4/G4HCofThisEvent.hh"
#include "Geant4/G4ios.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4SDParticleFilter.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4Event.hh"
#include "Geant4/G4RunManager.hh"

namespace MATHUSLA { namespace MU {

TrackerSD::TrackerSD(const G4String& name, const G4String& hitsCollectionName)
    : G4VSensitiveDetector(name), fHitsCollection(NULL) {
  collectionName.insert(hitsCollectionName);
}

G4VPhysicalVolume* TrackerSD::Construct(G4LogicalVolume*) {
    return 0;
}

void TrackerSD::Initialize(G4HCofThisEvent* eventhc) {
  fHitsCollection = new TrackerHitsCollection(this->GetName(), collectionName[0]);
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
    G4int hitCount = fHitsCollection->entries();
    G4int eventID = 0;
    const G4Event* event = G4RunManager::GetRunManager()->GetCurrentEvent();
    if (event) eventID = event->GetEventID();

    G4cout << "\n\n------> Event: " << eventID
           << " | Hit Count: " << hitCount << "\n\n";

    G4int chamberID = -1, trackID = -1;
    for (G4int i = 0; i < hitCount; i++) {
      auto hit = static_cast<TrackerHit*>(fHitsCollection->GetHit(i));
      G4int newChamberID = hit->GetChamberID();
      G4int newTrackID = hit->GetTrackID();
      if (i != 0 && (chamberID != newChamberID || trackID != newTrackID)) {
        const G4int barwidth = 116
                             + std::to_string(newChamberID).length()
                             + std::to_string(newTrackID).length()
                             + hit->GetParticleName().length();
        G4cout << std::string(barwidth, '-') << '\n';
      }
      chamberID = newChamberID;
      trackID = newTrackID;
      hit->Print();
    }
    G4cout << "\n";
  }
}

} } /* namespace MATHUSLA::MU */
