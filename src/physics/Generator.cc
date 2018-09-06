/*
 * src/physics/Generator.cc
 *
 * Copyright 2018 Brandon Gomes
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "physics/Generator.hh"

#include <cmath>
#include <limits>
#include <ostream>

#include <Geant4/Randomize.hh>
#include <Geant4/G4ParticleTable.hh>

#include "physics/Units.hh"

#include "util/string.hh"

namespace MATHUSLA { namespace MU {

namespace Physics { ////////////////////////////////////////////////////////////////////////////

//__Check if Particle Matches the Cut___________________________________________________________
bool ParticleCut::matches(const int particle_id,
                          const G4ThreeVector& momentum) const {
  return matches(particle_id, Convert(momentum));
}
//----------------------------------------------------------------------------------------------

//__Check if Particle Matches the Cut___________________________________________________________
bool ParticleCut::matches(const G4ThreeVector& momentum) const {
  return matches(Convert(momentum));
}
//----------------------------------------------------------------------------------------------

//__Check if Particle Matches the Cut___________________________________________________________
bool ParticleCut::matches(const int particle_id,
                          const PseudoLorentzTriplet& triplet) const {
  return particle_id == id && matches(triplet);
}
//----------------------------------------------------------------------------------------------

//__Check if Particle Matches the Cut___________________________________________________________
bool ParticleCut::matches(const PseudoLorentzTriplet& triplet) const {
  bool match_pT = true, match_eta = true, match_phi = true;
  if (min.pT || max.pT) match_pT = (triplet.pT <= max.pT) && (triplet.pT >= min.pT);
  if (min.eta || max.eta) match_eta = (triplet.eta <= max.eta) && (triplet.eta >= min.eta);
  if (min.phi || max.phi) match_phi = (triplet.phi <= max.phi) && (triplet.phi >= min.phi);
  return match_pT && match_eta && match_phi;
}
//----------------------------------------------------------------------------------------------

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__Convert Particle Cut Part to String_________________________________________________________
void _cut_values_to_string(std::string& out,
                           const double min,
                           const double max,
                           const double unit,
                           const std::string& unit_string) {
  static const auto inf = std::numeric_limits<double>::infinity();
  if ((min || max) && !(min == -inf && max == inf)) {
    out.push_back(' ');
    if (min != -inf)
      out += std::to_string(min / unit);
    out.push_back(':');
    if (max != inf)
      out += std::to_string(max / unit);
    out += " " + unit_string;
  } else {
    out += " ";
  }
}
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Convert Particle Cut to String______________________________________________________________
const std::string ParticleCut::to_string() const {
  auto out = "[ " + std::to_string(id) + " |";
  _cut_values_to_string(out, min.pT, max.pT, Units::Momentum, Units::MomentumString);
  out += "|";
  _cut_values_to_string(out, min.eta, max.eta, 1, "");
  out += "|";
  _cut_values_to_string(out, min.phi, max.phi, Units::Angle, Units::AngleString);
  return out + " ]";
}
//----------------------------------------------------------------------------------------------

//__Check if Particle Matches Any Cut in PropagationList________________________________________
bool InPropagationList(const PropagationList& list,
                       const int particle_id,
                       const G4ThreeVector& momentum) {
  for (const auto& cut : list)
    if (cut.matches(particle_id, momentum)) return true;
  return false;
}
bool InPropagationList(const PropagationList& list,
                       const G4ThreeVector& momentum) {
  for (const auto& cut : list)
    if (cut.matches(momentum)) return true;
  return false;
}
bool InPropagationList(const PropagationList& list,
                       const int particle_id,
                       const PseudoLorentzTriplet& triplet) {
  for (const auto& cut : list)
    if (cut.matches(particle_id, triplet)) return true;
  return false;
}
bool InPropagationList(const PropagationList& list,
                       const PseudoLorentzTriplet& triplet) {
  for (const auto& cut : list)
    if (cut.matches(triplet)) return true;
  return false;
}
//----------------------------------------------------------------------------------------------

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__Set the Propagation Variables_______________________________________________________________
bool _set_propagation_limits(double& min,
                             double& max,
                             std::string substring,
                             const std::vector<std::string>& possible_unit_strings,
                             const std::vector<double>& possible_units) {
  if (util::string::strip(substring).empty())
    return true;

  std::vector<std::string> cuts;
  util::string::split(substring, cuts, ":");
  const auto size = cuts.size();

  double unit{1.0L};
  for (std::size_t i{}; i < possible_unit_strings.size(); ++i) {
    if (substring.rfind(possible_unit_strings[i]) != std::string::npos)
      unit = possible_units[i];
  }

  try {
    if (size == 1UL) {
      if (!cuts[0].empty()) {
        if (substring[0] == ':') {
          min = -std::numeric_limits<double>::infinity() * unit;
          max = std::stold(cuts[0]) * unit;
          return true;
        } else if (substring[substring.size() - 1UL] == ':') {
          min = std::stold(cuts[0]) * unit;
          max = std::numeric_limits<double>::infinity() * unit;
          return true;
        }
      }
    } else if (size == 2UL) {
      min = std::stold(cuts[0]) * unit;
      max = std::stold(cuts[1]) * unit;
      return true;
    }
  } catch (...) {}
  return false;
}
//----------------------------------------------------------------------------------------------

} /* anonymous namespace */ ////////////////////////////////////////////////////////////////////

//__Parse Propagation List from String__________________________________________________________
const PropagationList ParsePropagationList(const std::string& cut_string) {
  std::vector<std::string> tokens;
  util::string::split(util::string::strip(cut_string), tokens, "|");

  if (tokens.size() != 4)
    return PropagationList{};

  PropagationList out;
  std::vector<std::string> particles;
  util::string::split(util::string::strip(tokens[0]), particles, ",");
  out.reserve(std::min(1UL, particles.size()));

  static const std::vector<std::string> momentum_unit_strings{"GeV/c", "MeV/c", "keV/c", "eV/c" };
  static const std::vector<double>      momentum_units{        GeVperC, MeVperC, keVperC, eVperC};
  static const std::vector<std::string> angle_unit_strings{"rad", "deg"};
  static const std::vector<double>      angle_units{        rad,   deg };

  double min_pT{}, max_pT{}, min_eta{}, max_eta{}, min_phi{}, max_phi{};

  try {
    if (!_set_propagation_limits(min_pT,  max_pT,  tokens[1], momentum_unit_strings, momentum_units)
        || !_set_propagation_limits(min_eta, max_eta, tokens[2], {}, {})
        || !_set_propagation_limits(min_phi, max_phi, tokens[3], angle_unit_strings, angle_units))
      return PropagationList{};

    const PseudoLorentzTriplet min{min_pT, min_eta, min_phi};
    const PseudoLorentzTriplet max{max_pT, max_eta, max_phi};

    for (auto& particle : particles) {
      util::string::strip(particle);
      if (particle[0] == '-' || particle[0] == '+') {
        out.emplace_back( std::stol(particle), min, max);
      } else {
        out.emplace_back(-std::stol(particle), min, max);
        out.emplace_back( std::stol(particle), min, max);
      }
    }

  } catch (...) {
    return PropagationList{};
  }

  return out;
}
//----------------------------------------------------------------------------------------------

//__Get Momentum from Mass and Kinetic Energy___________________________________________________
const G4ThreeVector GetMomentum(const double mass,
                                const double ke,
                                const G4ThreeVector& p_unit) {
  return p_unit.unit() * std::sqrt(ke * (ke + 2.0L * mass));
}
//----------------------------------------------------------------------------------------------

//__Default Vertex for IP_______________________________________________________________________
G4PrimaryVertex* DefaultVertex() {
  return Vertex(0, 0, 100*m);
}
//----------------------------------------------------------------------------------------------

//__Generate Primary Vertex_____________________________________________________________________
G4PrimaryVertex* Vertex(const double x,
                        const double y,
                        const double z) {
  return Vertex(0, x, y, z);
}
//----------------------------------------------------------------------------------------------

//__Generate Primary Vertex_____________________________________________________________________
G4PrimaryVertex* Vertex(const double t,
                        const double x,
                        const double y,
                        const double z) {
  return new G4PrimaryVertex(x, y, z, t);
}
//----------------------------------------------------------------------------------------------

//__Generate Primary Vertex_____________________________________________________________________
G4PrimaryVertex* Vertex(const G4ThreeVector& vertex) {
  return Vertex(vertex.x(), vertex.y(), vertex.z());
}
//----------------------------------------------------------------------------------------------

//__Generate Primary Vertex_____________________________________________________________________
G4PrimaryVertex* Vertex(const double t,
                        const G4ThreeVector& vertex) {
  return Vertex(t, vertex.x(), vertex.y(), vertex.z());
}
//----------------------------------------------------------------------------------------------

//__Create Particle from ID and Momentum________________________________________________________
G4PrimaryParticle* CreateParticle(const int id,
                                  const G4ThreeVector& p) {
  return new G4PrimaryParticle(id, p.x(), p.y(), p.z());
}
//----------------------------------------------------------------------------------------------

//__Create Particle from ID and Pseudo-Lorentz Triplet__________________________________________
G4PrimaryParticle* CreateParticle(const int id,
                                  const double pT,
                                  const double eta,
                                  const double phi) {
  return CreateParticle(id, PseudoLorentzTriplet{pT, eta, phi});
}
//----------------------------------------------------------------------------------------------

//__Create Particle from ID and Pseudo-Lorentz Triplet__________________________________________
G4PrimaryParticle* CreateParticle(const int id,
                                  const PseudoLorentzTriplet& triplet) {
  return CreateParticle(id, Convert(triplet));
}
//----------------------------------------------------------------------------------------------

//__Generator Messenger Directory Path__________________________________________________________
const std::string Generator::MessengerDirectory = "/gen/";
//----------------------------------------------------------------------------------------------

//__Generator Simulation Setting Prefix_________________________________________________________
const std::string Generator::SimSettingPrefix = "GEN";
//----------------------------------------------------------------------------------------------

//__Generator Constructor_______________________________________________________________________
Generator::Generator(const std::string& name,
                     const std::string& description,
                     const int id,
                     const double pT,
                     const double eta,
                     const double phi)
    : Generator(name, description, id, pT, eta, phi, 0, {0, 0, 100*m}) {}
//----------------------------------------------------------------------------------------------

//__Generator Constructor_______________________________________________________________________
Generator::Generator(const std::string& name,
                     const std::string& description,
                     const int id,
                     const double ke,
                     const G4ThreeVector& p_unit)
    : Generator(name, description, id, ke, p_unit, 0, {0, 0, 100*m}) {}
//----------------------------------------------------------------------------------------------

//__Generator Constructor_______________________________________________________________________
Generator::Generator(const std::string& name,
                     const std::string& description,
                     const int id,
                     const double pT,
                     const double eta,
                     const double phi,
                     const G4ThreeVector& vertex)
    : Generator(name, description, id, pT, eta, phi, 0, vertex) {}
//----------------------------------------------------------------------------------------------

//__Generator Constructor_______________________________________________________________________
Generator::Generator(const std::string& name,
                     const std::string& description,
                     const int id,
                     const double ke,
                     const G4ThreeVector& p_unit,
                     const G4ThreeVector& vertex)
    : Generator(name, description, id, ke, p_unit, 0, vertex) {}
//----------------------------------------------------------------------------------------------

//__Generator Constructor_______________________________________________________________________
Generator::Generator(const std::string& name,
                     const std::string& description,
                     const int id,
                     const double pT,
                     const double eta,
                     const double phi,
                     const double t0,
                     const G4ThreeVector& vertex)
    : G4UImessenger(MessengerDirectory + name + '/', description),
      _name(name), _description(description),
      _id(id), _pT(pT), _eta(eta), _phi(phi), _mass(GetParticleMass(id)), _using_pt_eta_phi(true),
      _t0(t0), _vertex(vertex) {
  const auto conversion = Convert(PseudoLorentzTriplet{_pT, _eta, _phi});
  _ke = std::hypot(conversion.mag(), _mass) - _mass;
  _p_unit = conversion.unit();
  GenerateCommands();
}
//----------------------------------------------------------------------------------------------

//__Generator Constructor_______________________________________________________________________
Generator::Generator(const std::string& name,
                     const std::string& description,
                     const int id,
                     const double ke,
                     const G4ThreeVector& p_unit,
                     const double t0,
                     const G4ThreeVector& vertex)
    : G4UImessenger(MessengerDirectory + name + '/', description),
      _name(name), _description(description),
      _id(id), _ke(ke), _p_unit(p_unit), _mass(GetParticleMass(id)), _using_pt_eta_phi(false),
      _t0(t0), _vertex(vertex) {
  const auto conversion = Convert(GetMomentum(_mass, _ke, _p_unit));
  _pT = conversion.pT;
  _eta = conversion.eta;
  _phi = conversion.phi;
  GenerateCommands();
}
//----------------------------------------------------------------------------------------------

//__Generator Constructor_______________________________________________________________________
Generator::Generator(const std::string& name,
                     const std::string& description)
    : G4UImessenger(MessengerDirectory + name + '/', description),
      _name(name), _description(description) {}
//----------------------------------------------------------------------------------------------

//__Generate UI Commands________________________________________________________________________
void Generator::GenerateCommands() {
  _ui_id = CreateCommand<Command::IntegerArg>("id", "Set Particle Id.");
  _ui_id->SetParameterName("id", false);
  _ui_id->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_pT = CreateCommand<Command::DoubleUnitArg>("pT", "Set Transverse Momentum.");
  _ui_pT->SetParameterName("pT", false, false);
  _ui_pT->SetRange("pT > 0");
  _ui_pT->SetDefaultUnit("GeV/c");
  _ui_pT->SetUnitCandidates("eV/c keV/c MeV/c GeV/c");
  _ui_pT->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_eta = CreateCommand<Command::DoubleArg>("eta", "Set Pseudorapidity.");
  _ui_eta->SetParameterName("eta", false);
  _ui_eta->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_phi = CreateCommand<Command::DoubleUnitArg>("phi", "Set Semi-Opening Angle.");
  _ui_phi->SetParameterName("phi", false, false);
  _ui_phi->SetDefaultUnit("deg");
  _ui_phi->SetUnitCandidates("degree deg radian rad milliradian mrad");
  _ui_phi->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_ke = CreateCommand<Command::DoubleUnitArg>("ke", "Set Kinetic Energy.");
  _ui_ke->SetParameterName("ke", false, false);
  _ui_ke->SetRange("ke > 0");
  _ui_ke->SetDefaultUnit("GeV");
  _ui_ke->SetUnitCandidates("eV keV MeV GeV");
  _ui_ke->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_p = CreateCommand<Command::ThreeVectorArg>("p_unit", "Set Momentum Direction.");
  _ui_p->SetParameterName("px", "py", "pz", false, false);
  _ui_p->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_t0 = CreateCommand<Command::DoubleUnitArg>("t0", "Set Initial Time");
  _ui_t0->SetParameterName("t0", false, false);
  _ui_t0->SetDefaultUnit("ns");
  _ui_t0->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_vertex = CreateCommand<Command::ThreeVectorUnitArg>("vertex", "Set Initial Vertex Position");
  _ui_vertex->SetParameterName("x0", "y0", "z0", false, false);
  _ui_vertex->SetDefaultUnit("m");
  _ui_vertex->AvailableForStates(G4State_PreInit, G4State_Idle);
}
//----------------------------------------------------------------------------------------------

//__Generate Initial Particles__________________________________________________________________
void Generator::GeneratePrimaryVertex(G4Event* event) {
  auto vertex = Vertex(_t0, _vertex);
  if (_using_pt_eta_phi) {
    vertex->SetPrimary(CreateParticle(_id, _pT, _eta, _phi));
  } else {
    vertex->SetPrimary(CreateParticle(_id, GetMomentum(_mass, _ke, _p_unit)));
  }
  event->AddPrimaryVertex(vertex);
}
//----------------------------------------------------------------------------------------------

//__Generator Messenger Set Value_______________________________________________________________
void Generator::SetNewValue(G4UIcommand* command, G4String value) {
  if (command == _ui_id) {
    _id = _ui_id->GetNewIntValue(value);
    _mass = GetParticleMass(_id);
  } else if (command == _ui_pT) {
    _pT = _ui_pT->GetNewDoubleValue(value);
    _using_pt_eta_phi = true;
  } else if (command == _ui_eta) {
    _eta = _ui_eta->GetNewDoubleValue(value);
    _using_pt_eta_phi = true;
  } else if (command == _ui_phi) {
    _phi = _ui_phi->GetNewDoubleValue(value);
    _using_pt_eta_phi = true;
  } else if (command == _ui_ke) {
    _ke = _ui_ke->GetNewDoubleValue(value);
    _using_pt_eta_phi = false;
  } else if (command == _ui_p) {
    _p_unit = _ui_p->GetNew3VectorValue(value).unit();
    _using_pt_eta_phi = false;
  } else if (command == _ui_t0) {
    _t0 = _ui_t0->GetNewDoubleValue(value);
  } else if (command == _ui_vertex) {
    _vertex = _ui_vertex->GetNew3VectorValue(value);
  }

  if (_using_pt_eta_phi) {
    const auto conversion = Convert(PseudoLorentzTriplet{_pT, _eta, _phi});
    _ke = std::hypot(conversion.mag(), _mass) - _mass;
    _p_unit = conversion.unit();
  } else {
    const auto conversion = Convert(GetMomentum(_mass, _ke, _p_unit));
    _pT = conversion.pT;
    _eta = conversion.eta;
    _phi = conversion.phi;
  }
}
//----------------------------------------------------------------------------------------------

//__Total Momentum______________________________________________________________________________
const G4ThreeVector Generator::p() const {
  return GetMomentum(_mass, _ke, _p_unit);
}
//----------------------------------------------------------------------------------------------

//__Generator Information String________________________________________________________________
std::ostream& Generator::Print(std::ostream& os) const {
  return os << "Generator Info:\n  "
            << "Name: "        << _name                        << "\n  "
            << "Description: " << _description                 << "\n  "
            << "Particle ID: " << _id                          << "\n  "
            << "pT: "          << G4BestUnit(_pT, "Momentum")  << "\n  "
            << "eta: "         << _eta                         << "\n  "
            << "phi: "         << G4BestUnit(_phi, "Angle")    << "\n  "
            << "ke: "          << G4BestUnit(_ke, "Energy")    << "\n  "
            << "p_unit: "      << _p_unit                      << "\n  "
            << "vertex: "      << G4BestUnit(_t0, "Time")       << " "
                               << G4BestUnit(_vertex, "Length") << "\n";
}
//----------------------------------------------------------------------------------------------

//__Generator Specifications____________________________________________________________________
const Analysis::SimSettingList Generator::GetSpecification() const {
  return Analysis::Settings(SimSettingPrefix,
    "",        _name,
    "_PDG_ID", std::to_string(_id),
    "_PT",     std::to_string(_pT / Units::Momentum) + " " + Units::MomentumString,
    "_ETA",    std::to_string(_eta),
    "_PHI",    std::to_string(_phi / Units::Angle) + " " + Units::AngleString,
    "_KE",     std::to_string(_ke / Units::Energy) + " " + Units::EnergyString,
    "_P_UNIT", "(" + std::to_string(_p_unit.x()) + ", "
                   + std::to_string(_p_unit.y()) + ", "
                   + std::to_string(_p_unit.z()) + ")",
    "_VERTEX", "(" + std::to_string(_t0         / Units::Time)   + ", "
                   + std::to_string(_vertex.x() / Units::Length) + ", "
                   + std::to_string(_vertex.y() / Units::Length) + ", "
                   + std::to_string(_vertex.z() / Units::Length) + ")");
}
//----------------------------------------------------------------------------------------------

} /* namespace Physics */ //////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
