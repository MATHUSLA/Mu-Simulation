#include "detector/Prototype.hh"

#include "Geant4/G4UnionSolid.hh"
#include "Geant4/G4Box.hh"

#include "detector/Construction.hh"

namespace MATHUSLA { namespace MU {

Envelope::Envelope(const G4String& name,
                   const G4double layer_spacing,
                   const LayerType layer_type,
                   const Alignment alignment,
                   std::initializer_list<Scintillator> scintillators)
    : fName(name), fLayerSpacing(layer_spacing), fHeight(0), fTopWidth(0),
      fBottomWidth(0), fScintillators(scintillators), fVolume(nullptr) {

  unsigned char size = fScintillators.size();
  if (!size) return;

  auto align = static_cast<signed char>(alignment);
  G4double height = 0, depth = 0;
  for (auto s : fScintillators) {
    height += s.GetHeight();
    depth = depth < s.GetDepth() ? s.GetDepth() : depth;
  }
  depth = 2 * depth + layer_spacing;
  fHeight = height;

  std::vector<G4double> transform{};
  G4VSolid* solid = new G4Box("", 0.1, 0.1, 0.1);

  G4double align_shift = 0, stack_shift = 0;
  G4double last_maxwidth = fScintillators[0].GetMinWidth();
  bool layer = layer_type == LayerType::BottomFirst;

  fBottomWidth = last_maxwidth;
  for (auto s : fScintillators) {
    stack_shift += 0.5 * s.GetHeight();
    align_shift += 0.5 * align * (s.GetMinWidth() - last_maxwidth);
    transform.push_back(align_shift);
    transform.push_back((layer ? 0.5 : -0.5) * (layer_spacing + s.GetDepth()));
    transform.push_back(stack_shift - 0.5 * height);
    stack_shift += 0.5 * s.GetHeight();
    layer = !layer;
    last_maxwidth = s.GetMaxWidth();
  }
  fTopWidth = last_maxwidth;

  for (unsigned char i = 0; i < size; ++i) {
    auto s = fScintillators[i];
    solid = new G4UnionSolid("", solid,
      Construction::Trap("",
        s.GetHeight(),
        s.GetMinWidth(),
        s.GetMaxWidth(),
        depth),
      G4Translate3D(transform[3*i], 0, transform[3*i+2]));
  }

  fVolume = Construction::Volume(name, solid, Construction::Material::Air);

  for (unsigned char i = 0; i < size; ++i) {
    auto s = fScintillators[i];
    auto volume = s.GetVolume();
    volume->SetName(G4String()
      + name[0] + name[1] + std::to_string(1 + i) + '/' + s.GetName());
    Construction::PlaceVolume(volume, fVolume,
      G4Translate3D(transform[3*i], transform[3*i+1], transform[3*i+2]));
  }
}

const Scintillator Envelope::GetScintillator(const G4String& name) const {
  for (auto s : fScintillators)
    if (s.GetName() == name) return s;
  return Scintillator();
}

} } /* namespace MATHUSLA::MU */
