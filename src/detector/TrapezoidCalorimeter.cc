#include "detector/TrapezoidCalorimeter.hh"

#include "Geant4/G4Trap.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4SubtractionSolid.hh"
#include "Geant4/G4NistManager.hh"
#include "Geant4/G4PVPlacement.hh"
#include "Geant4/G4Box.hh"
#include "Geant4/G4HCofThisEvent.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4Step.hh"
#include "Geant4/G4ThreeVector.hh"
#include "Geant4/G4Event.hh"
#include "Geant4/G4RunManager.hh"
#include "Geant4/G4ios.hh"

#include "detector/PMT.hh"

namespace MATHUSLA { namespace MU {

EnvelopeList TrapezoidCalorimeter::fEnvelopes = EnvelopeList();

TrapezoidCalorimeter::TrapezoidCalorimeter()
    : G4VSensitiveDetector("MATHUSLA/MU/TrapezoidCalorimeter"),
      fHitsCollection(NULL) {
  collectionName.insert("TrapCal_HC");
  for (auto envelope : fEnvelopes) {
    for (auto trap : envelope.GetScintillatorList()) {
      trap.GetSensitiveRegion()->GetLogicalVolume()->SetSensitiveDetector(this);
    }
  }
}

void TrapezoidCalorimeter::Initialize(G4HCofThisEvent* eventHitsCollection) {
  fHitsCollection = new TrapezoidHC(this->GetName(), collectionName[0]);
  eventHitsCollection->AddHitsCollection(
    G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]),
    fHitsCollection);
}

G4bool TrapezoidCalorimeter::ProcessHits(G4Step* step, G4TouchableHistory*) {
  G4double deposit = step->GetTotalEnergyDeposit();

  if (deposit == 0) return false;

  auto track = step->GetTrack();
  auto post_step = step->GetPostStepPoint();

  if (track->GetParentID() == 0) {
    auto touchable = track->GetTouchable();
    auto trap_name = touchable->GetVolume()->GetName();
    auto env_name = touchable->GetHistory()->GetVolume(2)->GetName();
    fHitsCollection->insert(
      new TrapezoidHit(
        track->GetParticleDefinition()->GetParticleName(),
        track->GetTrackID(),
        env_name + "/" + trap_name,
        deposit,
        post_step->GetGlobalTime(),
        post_step->GetPosition(),
        post_step->GetTotalEnergy(),
        post_step->GetMomentum()));
    return true;
  }
  return false;
}

void TrapezoidCalorimeter::EndOfEvent(G4HCofThisEvent*) {
  if (verboseLevel > 1) {
    G4int eventID = 0;
    const G4Event* event = G4RunManager::GetRunManager()->GetCurrentEvent();
    if (event) eventID = event->GetEventID();
    G4int hitCount = fHitsCollection->entries();

    if (!hitCount) return;

    G4cout << "\n\n------> Event: " << eventID
           << " | Hit Count: " << hitCount << "\n\n";

    G4int trackID = -1;
    G4String chamberID = "";
    for (G4int i = 0; i < hitCount; i++) {
      auto hit = static_cast<TrapezoidHit*>(fHitsCollection->GetHit(i));
      G4String newChamberID = hit->GetChamberID();
      G4int newTrackID = hit->GetTrackID();
      if (i != 0 && (chamberID != newChamberID || trackID != newTrackID)) {
        const G4int barwidth = 180
                             + hit->GetParticleName().length()
                             + std::to_string(newTrackID).length()
                             + newChamberID.length();
        G4cout << std::string(barwidth, '-') << '\n';
      }
      chamberID = newChamberID;
      trackID = newTrackID;
      hit->Print();
    }
    G4cout << "\n";
  }
}

G4Material* TrapezoidCalorimeter::Material::Aluminum     = 0;
G4Material* TrapezoidCalorimeter::Material::Carbon       = 0;
G4Material* TrapezoidCalorimeter::Material::Scintillator = 0;

void TrapezoidCalorimeter::DefineMaterials() {
  Material::Aluminum = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");
  Material::Carbon = G4NistManager::Instance()->FindOrBuildMaterial("G4_C");

  Material::Scintillator = new G4Material("Scintillator", 1.032*g/cm3, 2);
  Material::Scintillator->AddElement(Construction::Material::C, 9);
  Material::Scintillator->AddElement(Construction::Material::H, 10);

  const G4int nSci = 1;
  G4double eSci[nSci] = { 3.10*eV };
  G4double rSci[nSci] = { 1.58    };

  auto sciProp = new G4MaterialPropertiesTable();
  sciProp->AddProperty("RINDEX", eSci, rSci, nSci);
  Material::Scintillator->SetMaterialPropertiesTable(sciProp);
}

G4VPhysicalVolume* TrapezoidCalorimeter::Construct(G4LogicalVolume* world) {
  // TODO: move to ROgeometry
  auto Calorimeter = Construction::Volume(
    new G4Box("TrapezoidCalorimeter", 1000.0*cm, 1000.0*cm, 1000.0*cm),
    Construction::Material::Air);

  auto TopFirst = Envelope::LayerType::TopFirst;
  auto BottomFirst = Envelope::LayerType::BottomFirst;

/*

  // Tentative constants
  G4double depth = 2.0*cm;
  G4double thickness = 0.1*cm;
  G4double spacing = 0.1*cm;
  G4double angle = 4.5*deg;        // what is this
  G4double cfac = tan(0.5*angle);  // what is this

  // TODO: calculate the minwidth, maxwidth of each scintillator

  auto A10 = Scintillator("A10", 34.2735559430568*cm,,,depth, thickness, spacing);
  auto A11 = Scintillator("A11", 39.2697011242604*cm,);
  // NO DEFINITION FOUND: auto A12 = Scintillator("A12");

  auto B6 = Scintillator("B6", 46.963764703314*cm,,,depth, thickness, spacing);
  auto B7 = Scintillator("B7", 55.5571344149842*cm,,,depth, thickness, spacing);
  auto B8 = Scintillator("B8", 66.8484225245044*cm,,,depth, thickness, spacing);
  auto B9 = Scintillator("B9", 37.8707804735234*cm,,,depth, thickness, spacing);
  auto B10 = Scintillator("B10", 42.3673111366067*cm,,,depth, thickness, spacing);
  auto B11 = Scintillator("B11", 55.8569031258564*cm,,,depth, thickness, spacing);
  auto B12 = Scintillator("B12", 64.8499644520229*cm,,,depth, thickness, spacing);
  auto B12_1 = Scintillator("B12-1", 64.8499644520229*cm,,,depth, thickness, spacing);

  auto C3 = Scintillator("C3", 36.9714743409067*cm,,,depth, thickness, spacing);
  auto C4 = Scintillator("C4", 42.6670798474789*cm,,,depth, thickness, spacing);
  auto C5 = Scintillator("C5", 49.5617601975399*cm,,,depth, thickness, spacing);
  auto C6 = Scintillator("C6", 57.8553611983379*cm,,,depth, thickness, spacing);
  auto C7 = Scintillator("C7", 68.9468035006099*cm,,,depth, thickness, spacing);
  auto C8 = Scintillator("C8", 81.9367809717393*cm,,,depth, thickness, spacing);
  auto C9 = Scintillator("C9", 46.8638417996899*cm,,,depth, thickness, spacing);
  auto C10 = Scintillator("C10", 52.2596785953898*cm,,,depth, thickness, spacing);
  auto C11 = Scintillator("C11", 69.2465722114821*cm,,,depth, thickness, spacing);

  auto SA1 = Envelope("SA1", 10*cm, BottomFirst, {C3, C4, C5, C6, C7});
  auto SA2 = Envelope("SA2", 10*cm, BottomFirst, {C3, C4, C5, C6, C7});
  auto SA3 = Envelope("SA3", 10*cm, BottomFirst, {B7, B8, B9, B10, B11});
  auto SA4 = Envelope("SA4", 10*cm, TopFirst, {B8, B9, B10, B11, B12});
  auto SA5 = Envelope("SA5", 10*cm, TopFirst, {C8, C9, C10, C11});
  auto SA6 = Envelope("SA6", 10*cm, TopFirst, {C8, C9, C10, C11});

  auto SB1 = Envelope("SB1", 10*cm, BottomFirst, {C3, C4, B6, B7, C7});
  auto SB2 = Envelope("SB2", 10*cm, BottomFirst, {C3, C4, C5, C6, C7});
  auto SB3 = Envelope("SB3", 10*cm, BottomFirst, {B7, B8, B9, C9, C9});
  auto SB4 = Envelope("SB4", 10*cm, TopFirst, {A10, A11, A11, B9, B12_1, B11});
  auto SB5 = Envelope("SB5", 10*cm, TopFirst, {A10, A12, C7, C8, C9});
  auto SB6 = Envelope("SB6", 10*cm, TopFirst, {B11, B11, C9, B11, B11});

  fEnvelopes = EnvelopeList(
    {SA1, SA2, SA3, SA4, SA5, SA6, SB1, SB2, SB3, SB4, SB5, SB6});
*/

  auto test_env1 = Envelope("TEST_ENV1", 10*cm, TopFirst, {
    Scintillator("TEST1A", 30*cm, 40*cm, 50*cm, 5*cm, 1*cm, 0.5*cm),
    Scintillator("TEST2A", 30*cm, 50*cm, 60*cm, 5*cm, 1*cm, 0.5*cm),
    Scintillator("TEST3A", 30*cm, 60*cm, 70*cm, 5*cm, 1*cm, 0.5*cm)
  });

  auto test_env2 = Envelope("TEST_ENV2", 10*cm, BottomFirst, {
    Scintillator("TEST1B", 10*cm, 40*cm, 50*cm, 5*cm, 1*cm, 0.5*cm),
    Scintillator("TEST2B", 9*cm, 50*cm, 60*cm, 5*cm, 1*cm, 0.5*cm),
    Scintillator("TEST3B", 8*cm, 60*cm, 70*cm, 5*cm, 1*cm, 0.5*cm)
  });

  fEnvelopes.push_back(test_env1);
  fEnvelopes.push_back(test_env2);

  Construction::PlaceVolume(
    test_env1.GetVolume(),
    Calorimeter,
    Construction::Rotate(1, 0, 0, -90*deg));

  Construction::PlaceVolume(
    test_env2.GetVolume(),
    Calorimeter,
    Construction::Transform(0, 0, 1.1*m, 1, 0, 0, -90*deg));

  return Construction::PlaceVolume(Calorimeter, world);
}

} } /* namespace MATHUSLA::MU */