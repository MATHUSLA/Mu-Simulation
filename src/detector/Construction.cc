#include "detector/Construction.hh"

#include "Geant4/G4Box.hh"
#include "Geant4/G4GeometryManager.hh"
#include "Geant4/G4GeometryTolerance.hh"
#include "Geant4/G4LogicalVolume.hh"
#include "Geant4/G4LogicalVolumeStore.hh"
#include "Geant4/G4Material.hh"
#include "Geant4/G4PhysicalVolumeStore.hh"
#include "Geant4/G4SDManager.hh"
#include "Geant4/G4SolidStore.hh"
#include "Geant4/G4PVPlacement.hh"

#include "detector/Earth.hh"
#include "detector/TrackerSD.hh"
#include "detector/TrapezoidCalorimeter.hh"

#include "detector/TestDetector.hh"

namespace MATHUSLA { namespace MU {

auto Construction::Material::H  = new G4Element("Hydrogen", "H",  1.,     1.01*g/mole);
auto Construction::Material::C  = new G4Element("Carbon",   "C",  6.,    12.01*g/mole);
auto Construction::Material::N  = new G4Element("Nitrogen", "N",  7.,    14.01*g/mole);
auto Construction::Material::O  = new G4Element("Oxygen",   "O",  8.,    16.00*g/mole);
auto Construction::Material::Al = new G4Element("Aluminum", "Al", 13.,  26.981*g/mole);
auto Construction::Material::Si = new G4Element("Silicon",  "Si", 14., 28.0855*g/mole);
auto Construction::Material::Ca = new G4Element("Calcium",  "Ca", 20.,  40.078*g/mole);

G4Material* Construction::Material::Air = 0;

G4VPhysicalVolume* Construction::Construct() {
  DefineMaterials();
  return DefineVolumes();
}

void Construction::DefineMaterials() {
  Material::Air = new G4Material("Air", 1.290*mg/cm3, 2);
  Material::Air->AddElement(Material::N, 0.7);
  Material::Air->AddElement(Material::O, 0.3);

  Earth::DefineMaterials();
  TrapezoidCalorimeter::DefineMaterials();

  G4cout << *(G4Material::GetMaterialTable()) << "\n";
}

G4VPhysicalVolume* Construction::DefineVolumes() {
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  G4cout << "Computed tolerance = "
         << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
         << " mm\n";

  constexpr G4double worldlen = 12000.0;

  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(worldlen);

  auto worldLV = Volume(
    new G4Box("World", worldlen*cm, worldlen*cm, worldlen*cm),
    Material::Air);

  auto worldPV = PlaceVolume(worldLV, NULL, G4Transform3D());

  Earth::Construct(worldLV);
  TrapezoidCalorimeter::Construct(worldLV);

  return worldPV;
}

void Construction::ConstructSDandField() {
  auto detector = new TrackerSD("MATHUSLA/MU/TrackerSD", "TrackerHC");
  G4SDManager::GetSDMpointer()->AddNewDetector(detector);
  SetSensitiveDetector("TrapezoidCalorimeter", detector);
}

G4RotationMatrix* Construction::Matrix(const G4double th1,
                                       const G4double phi1,
                                       const G4double th2,
                                       const G4double phi2,
                                       const G4double th3,
                                       const G4double phi3) {
  const G4double sinth1 = std::sin(th1);
  const G4double sinth2 = std::sin(th2);
  const G4double sinth3 = std::sin(th3);
  const G4double xpx = sinth1 * std::cos(phi1);
  const G4double xpy = sinth1 * std::sin(phi1);
  const G4double xpz = std::cos(th1);
  const G4double ypx = sinth2 * std::cos(phi2);
  const G4double ypy = sinth2 * std::sin(phi2);
  const G4double ypz = std::cos(th2);
  const G4double zpx = sinth3 * std::cos(phi3);
  const G4double zpy = sinth3 * std::sin(phi3);
  const G4double zpz = std::cos(th3);

  G4RotationMatrix* matrix = new G4RotationMatrix();
  matrix->rotateAxes(CLHEP::Hep3Vector(xpx, xpy, xpz),
                     CLHEP::Hep3Vector(ypx, ypy, ypz),
                     CLHEP::Hep3Vector(zpx, zpy, zpz));
  if (*matrix != G4RotationMatrix()) {
    matrix->invert();
  } else {
    delete matrix;
    matrix = 0;
  }
  return matrix;
}

G4LogicalVolume* Construction::Volume(const G4String& name,
                                      G4VSolid* solid,
                                      G4Material* material) {
  return new G4LogicalVolume(solid, material, name);
}

G4LogicalVolume* Construction::Volume(G4VSolid* solid,
                                      G4Material* material) {
  return Volume(solid->GetName(), solid, material);
}

G4VPhysicalVolume* Construction::PlaceVolume(const G4String& name,
                                             G4LogicalVolume* current,
                                             G4LogicalVolume* mother,
                                             const G4Transform3D& transform) {
  return new G4PVPlacement(transform, current, name, mother, false, 0);
}

G4VPhysicalVolume* Construction::PlaceVolume(G4LogicalVolume* current,
                                             G4LogicalVolume* mother,
                                             const G4Transform3D& transform) {
  return PlaceVolume(current->GetName(), current, mother, transform);
}

G4VPhysicalVolume* Construction::PlaceVolume(const G4String& name,
                                             G4VSolid* solid,
                                             G4Material* material,
                                             G4LogicalVolume* mother,
                                             const G4Transform3D& transform) {
  return PlaceVolume(name, Volume(name, solid, material), mother, transform);
}

G4VPhysicalVolume* Construction::PlaceVolume(G4VSolid* solid,
                                             G4Material* material,
                                             G4LogicalVolume* mother,
                                             const G4Transform3D& transform) {
  return PlaceVolume(solid->GetName(), solid, material, mother, transform);
}

} } /* namespace MATHUSLA::MU */
