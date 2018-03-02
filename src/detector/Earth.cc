#include "detector/Earth.hh"

#include "Geant4/G4Box.hh"

#include "detector/Construction.hh"

namespace MATHUSLA { namespace MU {

namespace Earth {

G4Material* Material::CaCO3     = 0;
G4Material* Material::Kaolinite = 0;
G4Material* Material::SiO2      = 0;
G4Material* Material::Marl      = 0;
G4Material* Material::Mix       = 0;

void Material::Define() {
  static bool defined = false;
  if (!defined) {
    auto Al = new G4Element("Aluminum", "Al", 13.,  26.981*g/mole);
    auto Si = new G4Element("Silicon",  "Si", 14., 28.0855*g/mole);
    auto Ca = new G4Element("Calcium",  "Ca", 20.,  40.078*g/mole);

    Material::CaCO3 = new G4Material("CaCO3", 2.71*g/cm3, 3);
    Material::CaCO3->AddElement(Construction::Material::C,  1);
    Material::CaCO3->AddElement(Ca, 1);
    Material::CaCO3->AddElement(Construction::Material::O,  3);

    Material::Kaolinite = new G4Material("Clay", 2.65*g/cm3, 4);
    Material::Kaolinite->AddElement(Al, 2);
    Material::Kaolinite->AddElement(Si, 2);
    Material::Kaolinite->AddElement(Construction::Material::O,  9);
    Material::Kaolinite->AddElement(Construction::Material::H,  4);

    Material::SiO2 = new G4Material("Quartz", 2.445*g/cm3, 2);
    Material::SiO2->AddElement(Si, 46.743*perCent);
    Material::SiO2->AddElement(Construction::Material::O, 53.257*perCent);

    Material::Marl = new G4Material("Marl", 2.46*g/cm3, 2);
    Material::Marl->AddMaterial(Material::Kaolinite, 35*perCent);
    Material::Marl->AddMaterial(Material::CaCO3,     65*perCent);

    Material::Mix = new G4Material("Mix", 2.54*g/cm3, 2);
    Material::Mix->AddMaterial(Material::Marl, 50*perCent);
    Material::Mix->AddMaterial(Material::SiO2, 50*perCent);
    defined = true;
  }
}

G4VPhysicalVolume* Construct(G4LogicalVolume* world) {
  Construction::PlaceVolume(
    new G4Box("Sandstone", 10000.0*cm, 10000.0*cm, 2265*cm),
    Material::SiO2,
    world,
    G4Translate3D(0, 0, 2265*cm));
  Construction::PlaceVolume(
    new G4Box("Marl", 10000.0*cm, 10000.0*cm, 912.5*cm),
    Material::Marl,
    world,
    G4Translate3D(0, 0, 5442.5*cm));
  Construction::PlaceVolume(
    new G4Box("Mix", 10000.0*cm, 10000.0*cm, 1822.5*cm),
    Material::Mix,
    world,
    G4Translate3D(0, 0, 8177.5*cm));
  return nullptr;
}

} /* namespace Earth */

} } /* namespace MATHUSLA::MU */
