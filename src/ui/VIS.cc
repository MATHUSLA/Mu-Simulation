#include "ui/VIS.hh"

namespace MATHUSLA { namespace MU {

bool VIS::Draw(const G4VMarker& marker) {
  auto pVVisManager = G4VVisManager::GetConcreteInstance();
  if (pVVisManager) {
    pVVisManager->Draw(marker);
    return true;
  }
  return false;
}

G4Circle VIS::Circle(const G4ThreeVector& point,
                     const G4double size,
                     const G4VMarker::FillStyle fill,
                     const G4VisAttributes& attr) {
  auto out = G4Circle(point);
  out.SetScreenSize(size);
  out.SetFillStyle(fill);
  out.SetVisAttributes(attr);
  return out;
}

G4Circle VIS::Circle(const G4ThreeVector& point,
                     const G4double size,
                     const G4VisAttributes& attr) {
  return Circle(point, size, G4VMarker::FillStyle::filled, attr);
}

G4Circle VIS::Circle(const G4ThreeVector& point,
                     const G4double size,
                     const G4Color& color) {
  return Circle(point, size, G4VisAttributes(color));
}

} } /* namespace MATHUSLA::MU */
