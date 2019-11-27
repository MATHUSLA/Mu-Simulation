#include "geometry/Prototype.hh"

#include <G4UnionSolid.hh>

#include "geometry/Construction.hh"

namespace MATHUSLA { namespace MU { namespace Prototype {

const UChannel::Info UChannel::InfoArray[UChannel::Count] = {
  {"SA1", -1.07961 * m, -0.04760 * m, -2.90712 * m, 2.76551 * m,  3.13935 * rad},
  {"SA2", -0.78732 * m, -0.04867 * m, -3.14587 * m, 2.76552 * m, -3.13785 * rad},
  {"SA3", -0.38147 * m, -0.04949 * m, -2.90712 * m, 2.76266 * m, -3.13073 * rad},
  {"SA4", -0.02495 * m, -0.04655 * m, -3.14737 * m, 2.76550 * m, -3.14047 * rad},
  {"SA5",  0.52009 * m, -0.04913 * m, -2.90962 * m, 2.76159 * m,  3.13335 * rad},
  {"SA6",  0.99321 * m, -0.05426 * m, -3.14587 * m, 2.87954 * m,  3.13635 * rad},
  {"SB1",  0.09058 * m, -1.01542 * m,  3.27137 * m, 2.84850 * m, -1.57267 * rad},
  {"SB2",  0.08960 * m, -0.75195 * m,  3.03387 * m, 2.84851 * m, -1.57305 * rad},
  {"SB3",  0.08862 * m, -0.34248 * m,  3.27187 * m, 2.84851 * m, -1.57342 * rad},
  {"SB4",  0.09002 * m, -0.00722 * m,  3.02987 * m, 2.84750 * m, -1.57042 * rad},
  {"SB5",  0.09292 * m,  0.44358 * m,  3.27187 * m, 2.84350 * m, -1.57267 * rad},
  {"SB6",  0.08898 * m,  0.81572 * m,  3.03387 * m, 2.84750 * m, -1.57117 * rad},
};

UChannel::UChannel(const std::string &name, const double length) : m_name(name), m_length(length) {
  m_logical_volume = Construction::BoxVolume("", Width, m_length, Height);

  auto top_box = Construction::Box("", Width, m_length, VerticalThickness);
  auto side_box = Construction::Box("", HorizontalThickness, m_length, Height);

  auto material_volume = Construction::Volume(new G4UnionSolid(name,
                                                  new G4UnionSolid("",
                                                                   top_box,
                                                                   side_box,
                                                                   G4Translate3D(0.5 * (Width - HorizontalThickness), 0.0, 0.5 * (Height - VerticalThickness))),
                                                  side_box,
                                                  G4Translate3D(-0.5 * (Width - HorizontalThickness), 0.0, 0.5 * (Height - VerticalThickness))),
                                              Construction::Material::Aluminum,
                                              Construction::BorderAttributes());

  Construction::PlaceVolume(material_volume,
                            m_logical_volume,
                            G4Translate3D(0.0, 0.0, -0.5 * (Height - VerticalThickness)));
}

} } }
