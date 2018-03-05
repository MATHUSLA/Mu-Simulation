#include "detector/Prototype.hh"

#include <cmath>

#include "Geant4/G4SubtractionSolid.hh"
#include "Geant4/G4UnionSolid.hh"
#include "Geant4/G4Tubs.hh"
#include "Geant4/G4VisAttributes.hh"
#include "Geant4/G4NistManager.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4RunManager.hh"

#include "detector/Construction.hh"

namespace MATHUSLA { namespace MU {

Scintillator::Scintillator(const G4String& name,
                           const G4double height,
                           const G4double minwidth,
                           const G4double maxwidth,
                           const G4double depth,
                           const G4double thickness,
                           const G4double spacing)
    : G4VSensitiveDetector("MATHUSLA/MU/Prototype/Scintillators/" + name),
      _hit_collection(nullptr), _name(name), _height(height),
      _minwidth(minwidth), _maxwidth(maxwidth), _depth(depth),
      _thickness(thickness), _spacing(spacing), _solid(nullptr),
      _volume(nullptr), _sensitive(nullptr), _casing(nullptr), _pmt(nullptr) {

  collectionName.insert("Prototype_Scintillator" + name + "_HC");
  _solid = Construction::Trap(name, height, minwidth, maxwidth, depth);

  const G4double dims[4] = {height   - thickness, minwidth - thickness,
                            maxwidth - thickness, depth    - thickness};

  auto casing = new G4SubtractionSolid(name + "_C", _solid,
    Construction::Trap("", dims[0], dims[1], dims[2], dims[3]));

  auto sensitive = Construction::Trap(name,
    dims[0] - spacing, dims[1] - spacing, dims[2] - spacing, dims[3] - spacing);

  auto pmt = new G4Tubs(
    name + "_PMT", 0, PMTRadius, 0.5 * PMTLength, 0, 360.0*deg);

  auto pmtTransform = Construction::Transform(
    0.5 * maxwidth + 0.25 * PMTLength, 0, 0.5 * height + 0.25 * PMTLength,
    0, 1, 0, 45*deg);

  auto full = new G4UnionSolid(name,
    new G4UnionSolid("", casing, sensitive), pmt, pmtTransform);

  _volume = Construction::Volume(full, G4VisAttributes(false));

  auto casingLV = Construction::Volume(casing,
    Material::Casing,
    Construction::CasingAttributes());

  auto sensitiveLV = Construction::Volume(sensitive,
    Material::Scintillator,
    Construction::SensitiveAttributes());

  auto pmtAttr = G4VisAttributes(G4Colour(0.7, 0.7, 0.7));
  pmtAttr.SetForceSolid(true);
  auto pmtLV = Construction::Volume(pmt, Material::PMT, pmtAttr);

  _casing    = Construction::PlaceVolume(casingLV, _volume);
  _sensitive = Construction::PlaceVolume(sensitiveLV, _volume);
  _pmt       = Construction::PlaceVolume(pmtLV, _volume, pmtTransform);

  // _sensitive->GetLogicalVolume()->SetSensitiveDetector(this);
}

G4Material* Scintillator::Material::PMT          = nullptr;
G4Material* Scintillator::Material::Casing       = nullptr;
G4Material* Scintillator::Material::Scintillator = nullptr;

void Scintillator::Material::Define() {
  Material::PMT = G4NistManager::Instance()->FindOrBuildMaterial("G4_C");
  Material::Casing = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al");

  Material::Scintillator = new G4Material("Scintillator", 1.032*g/cm3, 2);
  Material::Scintillator->AddElement(Construction::Material::C, 9);
  Material::Scintillator->AddElement(Construction::Material::H, 10);

  constexpr G4int nSci = 1;
  G4double eSci[nSci] = { 3.10*eV };
  G4double rSci[nSci] = { 1.58    };

  auto sciProp = new G4MaterialPropertiesTable();
  sciProp->AddProperty("RINDEX", eSci, rSci, nSci);
  Material::Scintillator->SetMaterialPropertiesTable(sciProp);
}

void Scintillator::Initialize(G4HCofThisEvent* eventHC) {
  _hit_collection = new PrototypeHC(GetName(), collectionName[0]);
  eventHC->AddHitsCollection(
    G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]),
    _hit_collection);
}

G4bool Scintillator::ProcessHits(G4Step* step, G4TouchableHistory*) {
  auto position = step->GetPostStepPoint()->GetPosition();
  auto rotation = _casing->GetRotation()->inverse();
  auto delta = (_casing->GetTranslation() - position).transform(rotation);

  auto x = delta.x();
  auto y = delta.y();
  auto z = delta.z();

  auto center_half_width = 0.25 * (_maxwidth + _minwidth);
  auto point = Point{
    0.5 * _height - y,
    std::sqrt(y*y + (center_half_width - x)*(center_half_width - x))
  };
  return false;
}

void Scintillator::EndOfEvent(G4HCofThisEvent*) {
  if (verboseLevel < 2) return;

  G4int eventID = 0;
  auto event = G4RunManager::GetRunManager()->GetCurrentEvent();
  if (event) eventID = event->GetEventID();
  auto hitCount = _hit_collection->entries();

  if (!hitCount) return;

  auto boxside = std::string(25 + std::to_string(eventID).length()
                                + std::to_string(hitCount).length(), '-');

  G4cout << "\n\n" << boxside << '\n'
         << "| Event: " << eventID << " | Hit Count: " << hitCount << " |\n"
         << boxside << '\n';

  G4int trackID = -1;
  G4String chamberID = "";
  for (G4int i = 0; i < hitCount; ++i) {
    auto hit = dynamic_cast<PrototypeHit*>(_hit_collection->GetHit(i));

    auto new_chamberID = hit->GetChamberID();
    auto new_trackID = hit->GetTrackID();
    G4bool hit_type = chamberID[0] == 'A' || chamberID[0] == 'B';
    G4bool new_hit_type = new_chamberID[0] == 'A' || new_chamberID[0] == 'B';

    if (i != 0 && (hit_type != new_hit_type || trackID != new_trackID)) {
      const auto barlength = 162
        + hit->GetParticleName().length()
        + std::to_string(new_trackID).length()
        + new_chamberID.length();
      G4cout << std::string(barlength, '-') << '\n';
    }

    chamberID = new_chamberID;
    trackID = new_trackID;
    hit->Print();
  }
  G4cout << '\n';
}

Scintillator* Scintillator::Clone(const Scintillator* other) {
  return new Scintillator(other->_name, other->_height, other->_minwidth,
    other->_maxwidth, other->_depth, other->_thickness, other->_spacing);
}

} } /* namespace MATHUSLA::MU */
