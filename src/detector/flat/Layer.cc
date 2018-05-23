#include "detector/Flat.hh"

#include "Geant4/G4Box.hh"
#include "Geant4/G4UnionSolid.hh"
#include "Geant4/G4VSolid.hh"

namespace MATHUSLA { namespace MU {

namespace Flat { ///////////////////////////////////////////////////////////////////////////////

//__Layer Constructor___________________________________________________________________________
Layer::Layer(const std::string& name,
             const size_t count,
             const Scintillator& scintillator)
    : _scintillator(&scintillator), _volume(nullptr), _placement(nullptr), _name(name), _count(count) {

  auto scintillator_solid = scintillator.lvolume->GetSolid();
  const auto&& shift = scintillator.base_width + ScintillatorSpacing;

  const auto&& full_width = (count - 1) * shift + scintillator.GetFullWidth();
  _volume = Construction::BoxVolume(name,
    full_width,
    scintillator.height,
    scintillator.length);

  const auto& name_prefix = name + scintillator.name;
  for (size_t i = 0; i < count; ++i) {
    auto clone = Scintillator::Clone(scintillator, name_prefix + std::to_string(i));

    clone->pvolume = Construction::PlaceVolume(clone->lvolume, _volume,
      Construction::Transform(i * shift + 0.5 * (scintillator.GetFullWidth() - full_width), 0, 0));
  }
}
//----------------------------------------------------------------------------------------------

//__Layer Constructor___________________________________________________________________________
void Layer::Register(G4VSensitiveDetector* detector) {
  const auto&& count = _volume->GetNoDaughters();
  for (size_t i = 0; i < count; ++i) {
    _volume->GetDaughter(i)->GetLogicalVolume()->SetSensitiveDetector(detector);
  }
}
//----------------------------------------------------------------------------------------------

//__Place Layer in the Detector_________________________________________________________________
G4VPhysicalVolume* Layer::PlaceIn(G4LogicalVolume* parent,
                                  const G4Transform3D& transform) {
  return (_placement = Construction::PlaceVolume(_volume, parent, transform));
}
//----------------------------------------------------------------------------------------------

//__Clone Layer_________________________________________________________________________________
Layer* Layer::Clone(const Layer& other,
                    const std::string& new_name) {
  // TODO: optimize this
  return new Layer(new_name, other._count, *other._scintillator);
}
//----------------------------------------------------------------------------------------------

} /* namespace Flat */ /////////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
