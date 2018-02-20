#include "detector/TrapezoidCalorimeter.hh"

#include <iterator>

#include "detector/Construction.hh"

namespace MATHUSLA { namespace MU {

Envelope::Envelope(const G4String& name,
                   const G4double layer_spacing,
                   const LayerType layer_type,
                   std::initializer_list<Scintillator> scintillators)
    : fName(name), fLayerSpacing(layer_spacing), fScintillators(scintillators),
      fVolume(nullptr) {

  if (!scintillators.size()) return;

  G4double height = 0, depth = 0;
  for (auto s : scintillators) {
    height += s.GetHeight();
    depth = depth < s.GetDepth() ? s.GetDepth() : depth;
  }

  fVolume = Construction::Volume(
    Construction::Trap(
      name,
      height,
      scintillators.begin()->GetMinWidth(),
      (scintillators.end()-1)->GetMaxWidth(),
      2 * depth + layer_spacing),
    Construction::Material::Air);

  G4double shift = 0;
  G4bool layer = layer_type == LayerType::BottomFirst;
  for (auto s : scintillators) {
    shift += s.GetHeight() / 2;
    Construction::PlaceVolume(s.GetVolume(),
      fVolume,
      G4Translate3D(
          0,
          (layer ? -0.5 : 0.5) * (layer_spacing + s.GetDepth()),
          shift - 0.5 * height
      ));
    shift += s.GetHeight() / 2;
    layer = !layer;
  }
}

const Scintillator Envelope::GetScintillator(const G4String& name) const {
  for (auto s : fScintillators)
    if (s.GetName() == name) return s;
  return Scintillator();
}

} } /* namespace MATHUSLA::MU */
