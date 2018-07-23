#include "geometry/Box.hh"

#include <Geant4/G4SubtractionSolid.hh>
#include <Geant4/tls.hh>

namespace MATHUSLA { namespace MU {

namespace Box { ////////////////////////////////////////////////////////////////////////////////

Scintillator::Scintillator(const std::string& name,
                           const double length,
                           const double height,
                           const double width,
                           const double thickness)
    : _name(name), _length(length), _height(height), _width(width), _thickness(thickness) {

  const auto border = 2 * _thickness;

  auto outer = Construction::Box("", _length, _height, _width);
  auto inner = Construction::Box("", _length - border, _height - border, _width - border);

  auto casing = new G4SubtractionSolid(name + "_C", outer, inner);

  _lvolume = Construction::Volume(name, outer, Construction::BorderAttributes());
  _sensitive = Construction::PlaceVolume(name,
    inner, Material::Scintillator, Construction::SensitiveAttributes(), _lvolume);
  Construction::PlaceVolume(casing, Material::Casing, Construction::CasingAttributes(), _lvolume);
}

//__Scintillator Material_______________________________________________________________________
G4Material* Scintillator::Material::Casing       = nullptr;
G4Material* Scintillator::Material::Scintillator = nullptr;
//----------------------------------------------------------------------------------------------

//__Define Scintillator Material________________________________________________________________
void Scintillator::Material::Define() {
  Material::Casing = Construction::Material::Aluminum;

  // Material::Scintillator = Construction::Material::Scintillator;

  Material::Scintillator = new G4Material("Scintillator", 1.032*g/cm3, 2);
  Material::Scintillator->AddElement(Construction::Material::C, 9);
  Material::Scintillator->AddElement(Construction::Material::H, 10);

  constexpr int_fast32_t nSci = 1;
  double eSci[nSci] = { 3.10*eV };
  double rSci[nSci] = { 1.58    };

  auto sciProp = new G4MaterialPropertiesTable();
  sciProp->AddProperty("RINDEX", eSci, rSci, nSci);
  Material::Scintillator->SetMaterialPropertiesTable(sciProp);
}
//----------------------------------------------------------------------------------------------

//__Register Scintillator with Detector_________________________________________________________
void Scintillator::Register(G4VSensitiveDetector* detector) {
  _sensitive->GetLogicalVolume()->SetSensitiveDetector(detector);
}
//----------------------------------------------------------------------------------------------

//__Place Scintillator in World_____________________________________________________________________
G4VPhysicalVolume* Scintillator::PlaceIn(G4LogicalVolume* parent,
                                         const G4Transform3D& transform) {
  return Construction::PlaceVolume(_lvolume, parent, transform);
}
//----------------------------------------------------------------------------------------------

} /* namespace Box */ //////////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
