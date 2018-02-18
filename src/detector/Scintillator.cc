#include "detector/TrapezoidCalorimeter.hh"

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
    : fName(name), fHeight(height), fMinWidth(minwidth), fMaxWidth(maxwidth),
      fDepth(depth), fThickness(thickness), fSpacing(spacing), fVolume(nullptr),
      fSRegion(nullptr), fCasing(nullptr), fPMT(nullptr) {

  auto casing = new G4SubtractionSolid(name + "_C",
    Construction::Trap("", height, minwidth, maxwidth, depth),
    Construction::Trap("", height   - thickness,
                           minwidth - thickness,
                           maxwidth - thickness,
                           depth    - thickness));

  auto sensitive = Construction::Trap(name + "_S",
    height   - thickness - spacing,
    minwidth - thickness - spacing,
    maxwidth - thickness - spacing,
    depth    - thickness - spacing);

  auto trap = new G4UnionSolid(name + "_T", casing, sensitive);
  auto pmt = PMTCylinder(name);

  auto pmtTransform = Construction::Transform(
    -0.5 * (maxwidth + 0.5 * PMTLength), 0, 0.5 * (height + 0.5 * PMTLength),
    0, 1, 0, -45*deg);

  auto full = new G4UnionSolid(name, trap, pmt, pmtTransform);

  fVolume = Construction::Volume(
    name, full, Construction::Material::Air, new G4VisAttributes(false));

  auto casingAttr = new G4VisAttributes(G4Colour(0., 0., 1., 0.2));
  casingAttr->SetForceSolid(true);
  auto casingLV = Construction::Volume(casing,
    TrapezoidCalorimeter::Material::Aluminum,
    casingAttr);

  auto sensitiveAttr = new G4VisAttributes(G4Colour(0., 1., 0., 1.0));
  sensitiveAttr->SetForceSolid(true);
  auto sensitiveLV = Construction::Volume(sensitive,
    TrapezoidCalorimeter::Material::Scintillator,
    sensitiveAttr);

  auto pmtAttr = new G4VisAttributes(G4Colour(0.7, 0.7, 0.7));
  pmtAttr->SetForceSolid(true);
  auto pmtLV = Construction::Volume(pmt,
    TrapezoidCalorimeter::Material::Carbon,
    pmtAttr);

  fCasing = Construction::PlaceVolume(casingLV, fVolume);
  fSRegion = Construction::PlaceVolume(sensitiveLV, fVolume);
  fPMT = Construction::PlaceVolume(pmtLV, fVolume, pmtTransform);
}

G4Tubs* Scintillator::PMTCylinder(const G4String& name) {
  return new G4Tubs(name + "_PMT", 0, PMTRadius, 0.5*PMTLength, 0, 360.0*deg);
}

} } /* namespace MATHUSLA::MU */
