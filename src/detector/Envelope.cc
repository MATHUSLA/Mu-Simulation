#include "detector/Prototype.hh"

#include "Geant4/G4UnionSolid.hh"
#include "Geant4/G4Box.hh"

#include "detector/Construction.hh"

namespace MATHUSLA { namespace MU {

Envelope::Envelope(const G4String& name,
                   const G4double layer_spacing,
                   const LayerType layer_type,
                   const Alignment alignment,
                   std::initializer_list<Scintillator*> scintillators)
    : _name(name), _layer_spacing(layer_spacing), _height(0), _top_width(0),
      _bottom_width(0), _scintillators(scintillators), _volume(nullptr) {

  unsigned char size = _scintillators.size();
  if (!size) return;

  auto align = static_cast<signed char>(alignment);
  G4double height = 0, depth = 0;
  for (auto s : _scintillators) {
    height += s->GetHeight();
    depth = depth < s->GetDepth() ? s->GetDepth() : depth;
  }
  depth = 2 * depth + layer_spacing;
  _height = height;

  std::vector<G4double> transform{};
  G4VSolid* solid = new G4Box("", 0.1, 0.1, 0.1);

  G4double align_shift = 0, stack_shift = 0;
  G4double last_maxwidth = _scintillators[0]->GetMinWidth();
  bool layer = layer_type == LayerType::BottomFirst;

  _bottom_width = last_maxwidth;
  for (auto s : _scintillators) {
    stack_shift += 0.5 * s->GetHeight();
    align_shift += 0.5 * align * (s->GetMinWidth() - last_maxwidth);
    transform.push_back(align_shift);
    transform.push_back((layer ? 0.5 : -0.5) * (layer_spacing + s->GetDepth()));
    transform.push_back(stack_shift - 0.5 * height);
    stack_shift += 0.5 * s->GetHeight();
    layer = !layer;
    last_maxwidth = s->GetMaxWidth();
  }
  _top_width = last_maxwidth;

  for (unsigned char i = 0; i < size; ++i) {
    auto s = _scintillators[i];
    solid = new G4UnionSolid("", solid,
      Construction::Trap("",
        s->GetHeight(),
        s->GetMinWidth(),
        s->GetMaxWidth(),
        depth),
      G4Translate3D(transform[3*i], 0, transform[3*i + 2]));
  }

  _volume = Construction::Volume(name, solid);

  for (unsigned char i = 0; i < size; ++i) {
    auto s = _scintillators[i];

    const G4String fullname = G4String() + name[0] + name[1]
                            + std::to_string(1 + i) + '_' + s->GetName();

    auto copyname = s->GetFullName();

    if (copyname != fullname && copyname != s->GetName()) {
      _scintillators[i] = Scintillator::Clone(s);
      s = _scintillators[i];
    }

    s->GetVolume()->SetName(fullname);
    s->GetSensitiveVolume()->SetName(fullname);

    Construction::PlaceVolume(s->GetVolume(), _volume,
      G4Translate3D(transform[3*i], transform[3*i + 1], transform[3*i + 2]));
  }
}

} } /* namespace MATHUSLA::MU */
