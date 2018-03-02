#include "detector/Prototype.hh"

#include "Geant4/G4SubtractionSolid.hh"
#include "Geant4/G4UnionSolid.hh"
#include "Geant4/G4VisAttributes.hh"

#include "detector/Construction.hh"

namespace MATHUSLA { namespace MU {

Scintillator::Scintillator() {}

Scintillator::Scintillator(const G4String& name,
                           const G4double height,
                           const G4double minwidth,
                           const G4double maxwidth,
                           const G4double depth,
                           const G4double thickness,
                           const G4double spacing)
    : _name(name), _height(height), _min_width(minwidth), _max_width(maxwidth),
      _depth(depth), _thickness(thickness), _spacing(spacing), _solid(nullptr),
      _volume(nullptr), _sensitive(nullptr), _casing(nullptr), _pmt(nullptr) {

  _solid = Construction::Trap(name, height, minwidth, maxwidth, depth);

  const G4double dims[4] = {height   - thickness, minwidth - thickness,
                            maxwidth - thickness, depth    - thickness};

  auto casing = new G4SubtractionSolid(name + "_C", _solid,
    Construction::Trap("", dims[0], dims[1], dims[2], dims[3]));

  auto sensitive = Construction::Trap(name,
    dims[0] - spacing, dims[1] - spacing, dims[2] - spacing, dims[3] - spacing);

  auto pmt = new G4Tubs(
    name + "_PMT", 0, PMTRadius, 0.5*PMTLength, 0, 360.0*deg);

  auto pmtTransform = Construction::Transform(
    0.5 * (maxwidth + 0.5 * PMTLength), 0, 0.5 * (height + 0.5 * PMTLength),
    0, 1, 0, 45*deg);

  auto full = new G4UnionSolid(name,
    new G4UnionSolid("", casing, sensitive), pmt, pmtTransform);

  _volume = Construction::Volume(
    name, full, Construction::Material::Air, G4VisAttributes(false));

  auto casingLV = Construction::Volume(casing,
    Prototype::Material::Aluminum,
    Construction::CasingAttributes());

  auto sensitiveLV = Construction::Volume(sensitive,
    Prototype::Material::Scintillator,
    Construction::SensitiveAttributes());

  auto pmtAttr = G4VisAttributes(G4Colour(0.7, 0.7, 0.7));
  pmtAttr.SetForceSolid(true);
  auto pmtLV = Construction::Volume(pmt,
    Prototype::Material::Carbon,
    pmtAttr);

  _casing    = Construction::PlaceVolume(casingLV, _volume);
  _sensitive = Construction::PlaceVolume(sensitiveLV, _volume);
  _pmt       = Construction::PlaceVolume(pmtLV, _volume, pmtTransform);
}

} } /* namespace MATHUSLA::MU */
