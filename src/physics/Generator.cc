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

//__Get ParticleCut String______________________________________________________________________
const std::string GetParticleCutString(const ParticleCut& cut) {
  auto out = "[ " + std::to_string(cut.id) + " |";
  _cut_values_to_string(out, cut.min.pT, cut.max.pT, Units::Momentum, Units::MomentumString);
  out += "|";
  _cut_values_to_string(out, cut.min.eta, cut.max.eta, 1, "");
  out += "|";
  _cut_values_to_string(out, cut.min.phi, cut.max.phi, Units::Angle, Units::AngleString);
  return out + " ]";
}
//----------------------------------------------------------------------------------------------

namespace { ////////////////////////////////////////////////////////////////////////////////////

//__Check if String has Number__________________________________________________________________
bool _has_digit(const std::string& s) {
  return std::any_of(s.begin(), s.end(), ::isdigit);
}
//----------------------------------------------------------------------------------------------

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

  double unit{1.0L};
  for (std::size_t i{}; i < possible_unit_strings.size(); ++i) {
    if (substring.rfind(possible_unit_strings[i]) != std::string::npos) {
      unit = possible_units[i];
      break;
    }
  }

  try {
    if (!_has_digit(cuts[0])) {
      min = -std::numeric_limits<double>::infinity() * unit;
      max = std::stold(cuts[1]) * unit;
    } else if (!_has_digit(cuts[1])) {
      min = std::stold(cuts[0]) * unit;
      max = std::numeric_limits<double>::infinity() * unit;
    } else {
      min = std::stold(cuts[0]) * unit;
      max = std::stold(cuts[1]) * unit;
    }
    return true;
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
        out.push_back({{ std::stoi(particle)}, {min, max}});
      } else {
        out.push_back({{-std::stoi(particle)}, {min, max}});
        out.push_back({{ std::stoi(particle)}, {min, max}});
      }
    }

  } catch (...) {
    return PropagationList{};
  }

  return out;
}
//----------------------------------------------------------------------------------------------

//__Check if Particle Matches Any Cut in PropagationList________________________________________
bool InPropagationList(const PropagationList& list,
                       const BasicParticle& particle) {
  for (const auto& cut : list)
    if (cut(particle)) return true;
  return false;
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
                     const Particle& particle)
    : G4UImessenger(MessengerDirectory + name + '/', description),
      _name(name), _description(description), _particle(particle) {
  GenerateCommands();
}
//----------------------------------------------------------------------------------------------

//__Generator Constructor_______________________________________________________________________
Generator::Generator(const std::string& name,
                     const std::string& description)
    : Generator(name, description, {}) {}
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

  _ui_p = CreateCommand<Command::ThreeVectorUnitArg>("p", "Set Momentum.");
  _ui_p->SetParameterName("px", "py", "pz", false, false);
  _ui_p->SetDefaultUnit("GeV/c");
  _ui_p->SetUnitCandidates("eV/c keV/c MeV/c GeV/c");
  _ui_p->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_p_unit = CreateCommand<Command::ThreeVectorArg>("p_unit", "Set Momentum Direction.");
  _ui_p_unit->SetParameterName("px", "py", "pz", false, false);
  _ui_p_unit->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_p_mag = CreateCommand<Command::DoubleUnitArg>("p_mag", "Set Momentum Magnitude.");
  _ui_p_mag->SetParameterName("p_mag", false, false);
  _ui_p_mag->SetRange("p_mag > 0");
  _ui_p_mag->SetDefaultUnit("GeV/c");
  _ui_p_mag->SetUnitCandidates("eV/c keV/c MeV/c GeV/c");
  _ui_p_mag->AvailableForStates(G4State_PreInit, G4State_Idle);

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
  AddParticle(_particle, *event);
}
//----------------------------------------------------------------------------------------------

//__Get Last Event Data_________________________________________________________________________
ParticleVector Generator::GetLastEvent() const {
  return ParticleVector{_particle};
}
//----------------------------------------------------------------------------------------------

//__Generator Messenger Set Value_______________________________________________________________
void Generator::SetNewValue(G4UIcommand* command,
                            G4String value) {
  if (command == _ui_id) {
    _particle.id = _ui_id->GetNewIntValue(value);
  } else if (command == _ui_pT) {
    _particle.set_pT(_ui_pT->GetNewDoubleValue(value));
  } else if (command == _ui_eta) {
    _particle.set_eta(_ui_eta->GetNewDoubleValue(value));
  } else if (command == _ui_phi) {
    _particle.set_phi(_ui_phi->GetNewDoubleValue(value));
  } else if (command == _ui_ke) {
    _particle.set_ke(_ui_ke->GetNewDoubleValue(value));
  } else if (command == _ui_p) {
    _particle.set_p(_ui_p->GetNew3VectorValue(value));
  } else if (command == _ui_p_unit) {
    _particle.set_p_unit(_ui_p_unit->GetNew3VectorValue(value).unit());
  } else if (command == _ui_p_mag) {
    _particle.set_p_mag(_ui_p_mag->GetNewDoubleValue(value));
  } else if (command == _ui_t0) {
    _particle.t = _ui_t0->GetNewDoubleValue(value);
  } else if (command == _ui_vertex) {
    _particle.set_vertex(_ui_vertex->GetNew3VectorValue(value));
  }
}
//----------------------------------------------------------------------------------------------

//__Generator Information String________________________________________________________________
std::ostream& Generator::Print(std::ostream& os) const {
  return os << "Generator Info:\n  "
            << "Name:        "  << _name                                     << "\n  "
            << "Description: "  << _description                              << "\n  "
            << "Particle ID: "  << _particle.id                              << "\n  "
            << "pT:          "  << G4BestUnit(_particle.pT(), "Momentum")    << "\n  "
            << "eta:         "  << _particle.eta()                           << "\n  "
            << "phi:         "  << G4BestUnit(_particle.phi(), "Angle")      << "\n  "
            << "ke:          "  << G4BestUnit(_particle.ke(), "Energy")      << "\n  "
            << "p:           "  << G4BestUnit(_particle.p(), "Momentum")     << "\n  "
            << "p_unit:      "  << _particle.p_unit()                        << "\n  "
            << "p_mag:       "  << G4BestUnit(_particle.p_mag(), "Momentum") << "\n"
            << "vertex:      (" << G4BestUnit(_particle.t, "Time")           << ", "
                                << G4BestUnit(_particle.x, "Length")         << ", "
                                << G4BestUnit(_particle.y, "Length")         << ", "
                                << G4BestUnit(_particle.z, "Length")         << ")\n";
}
//----------------------------------------------------------------------------------------------

//__Generator Specifications____________________________________________________________________
const Analysis::SimSettingList Generator::GetSpecification() const {
  return Analysis::Settings(SimSettingPrefix,
    "",        _name,
    "_PDG_ID", std::to_string(_particle.id),
    "_PT",     std::to_string(_particle.pT() / Units::Momentum)     + " "  + Units::MomentumString,
    "_ETA",    std::to_string(_particle.eta()),
    "_PHI",    std::to_string(_particle.phi() / Units::Angle)       + " "  + Units::AngleString,
    "_KE",     std::to_string(_particle.ke() / Units::Energy)       + " "  + Units::EnergyString,
    "_P", "(" + std::to_string(_particle.p().x() / Units::Momentum) + ", "
              + std::to_string(_particle.p().y() / Units::Momentum) + ", "
              + std::to_string(_particle.p().z() / Units::Momentum) + ") " + Units::MomentumString,
    "_P_UNIT", "(" + std::to_string(_particle.p_unit().x())         + ", "
                   + std::to_string(_particle.p_unit().y())         + ", "
                   + std::to_string(_particle.p_unit().z())         + ")",
    "_P_MAG", std::to_string(_particle.p_mag() / Units::Momentum)   + " "  + Units::MomentumString,
    "_VERTEX", "(" + std::to_string(_particle.t / Units::Time)      + ", "
                   + std::to_string(_particle.x / Units::Length)    + ", "
                   + std::to_string(_particle.y / Units::Length)    + ", "
                   + std::to_string(_particle.z / Units::Length)    + ")");
}
//----------------------------------------------------------------------------------------------

} /* namespace Physics */ //////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */
