#include "detector/Prototype.hh"

#include "Geant4/G4UnionSolid.hh"
#include "Geant4/G4Box.hh"

namespace MATHUSLA { namespace MU {

namespace Prototype { //////////////////////////////////////////////////////////////////////////

//__Envelope Constructor________________________________________________________________________
Envelope::Envelope(const std::string& name,
                   const LayerType layer_type,
                   const Alignment alignment,
                   const Rotation rotation,
                   std::initializer_list<Scintillator*> scintillators)
    : _name(name), _height(0), _top_width(0), _bottom_width(0),
      _scintillators(scintillators), _volume(nullptr) {

  const auto size = _scintillators.size();
  if (!size) return;

  for (const auto s : _scintillators)
    _height += s->height;

  const auto align = static_cast<signed char>(alignment);
  std::vector<double> transform{};
  double align_shift = 0, stack_shift = 0;
  double last_maxwidth = _scintillators[0]->minwidth;
  bool layer = layer_type == LayerType::BottomFirst;

  _bottom_width = last_maxwidth;
  for (const auto s : _scintillators) {
    stack_shift += 0.5 * s->height;
    align_shift += 0.5 * align * (s->minwidth - last_maxwidth);
    transform.push_back(align_shift);
    transform.push_back((layer ? 0.5 : -0.5) * (LayerSpacing + Scintillator::Depth));
    transform.push_back(stack_shift - 0.5 * _height);
    stack_shift += 0.5 * s->height;
    layer = !layer;
    last_maxwidth = s->maxwidth;
  }
  _top_width = last_maxwidth;

  G4VSolid* solid = new G4Box("", 0.1, 0.1, 0.1);
  for (size_t i = 0; i < size; ++i) {
    const auto s = _scintillators[i];
    solid = new G4UnionSolid("", solid,
      Construction::Trap("",
        s->height, s->minwidth, s->maxwidth,
        2 * Scintillator::Depth + LayerSpacing),
      G4Translate3D(transform[3*i], 0, transform[3*i + 2]));
  }

  _volume = Construction::Volume(name, solid);

  for (size_t i = 0; i < size; ++i) {
    auto s = _scintillators[i];
    const auto copyname = s->GetFullName();
    const auto fullname = std::string() + name[0] + name[1]
                        + std::to_string(rotation == Rotation::NoFlip ? (1 + i) : (size - i))
                        + '_' + s->name;

    if (copyname != fullname && copyname != s->name) {
      _scintillators[i] = Scintillator::Clone(s);
      s = _scintillators[i];
    }

    s->lvolume->SetName(fullname);
    s->sensitive->SetName(fullname);

    s->pvolume = Construction::PlaceVolume(s->lvolume, _volume,
      G4Translate3D(transform[3*i], transform[3*i + 1], transform[3*i + 2]));
  }

  if (rotation == Rotation::Flip) {
    std::reverse(_scintillators.begin(), _scintillators.end());
  }
}
//----------------------------------------------------------------------------------------------

//__Place Envelope in Layer_____________________________________________________________________
G4VPhysicalVolume* Envelope::PlaceIn(G4LogicalVolume* parent,
                                     const G4Transform3D& transform) {
  return (_placement = Construction::PlaceVolume(_volume, parent, transform));
}
//----------------------------------------------------------------------------------------------

} /* namespace Prototype */ ////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
