#include "physics/Generator.hh"

#include <cmath>
#include <sstream>

#include "Geant4/Randomize.hh"

#include "physics/Units.hh"

namespace MATHUSLA { namespace MU {

const G4String Generator::MessengerDirectory = "/gen/";

Generator::Generator(const G4String& name,
                     const G4String& description,
                     const G4int id,
                     const G4double pT,
                     const G4double eta,
                     const G4double phi)
    : G4UImessenger(MessengerDirectory + name + '/', description),
      _name(name), _description(description),
      _id(id), _pT(pT), _eta(eta), _phi(phi) {

  _ui_id = CreateCommand<G4CMD_Integer>("id", "Set Particle Id.");
  _ui_id->SetParameterName("id", false);
  _ui_id->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_pT = CreateCommand<G4CMD_DoubleUnit>("pT", "Set Transverse Momentum.");
  _ui_pT->SetParameterName("pT", false, false);
  _ui_pT->SetRange("pT > 0");
  _ui_pT->SetDefaultUnit("GeV/c");
  _ui_pT->SetUnitCandidates("eV/c keV/c MeV/c GeV/c");
  _ui_pT->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_eta = CreateCommand<G4CMD_Double>("eta", "Set Pseudorapidity.");
  _ui_eta->SetParameterName("eta", false);
  _ui_eta->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_phi = CreateCommand<G4CMD_DoubleUnit>("phi", "Set Semi-Opening Angle.");
  _ui_phi->SetParameterName("phi", false, false);
  _ui_phi->SetDefaultUnit("deg");
  _ui_phi->SetUnitCandidates("degree deg radian rad milliradian mrad");
  _ui_phi->AvailableForStates(G4State_PreInit, G4State_Idle);
}

void Generator::GeneratePrimaryVertex(G4Event* event) {
  auto vertex = DefaultVertex();
  vertex->SetPrimary(CreateParticle(_id, _pT, _eta, _phi));
  event->AddPrimaryVertex(vertex);
}

void Generator::SetNewValue(G4UIcommand* command, G4String value) {
  if (command == _ui_id) {
    _id = _ui_id->GetNewIntValue(value);
  } else if (command == _ui_pT) {
    _pT = _ui_pT->GetNewDoubleValue(value);
  } else if (command == _ui_eta) {
    _eta = _ui_eta->GetNewDoubleValue(value);
  } else if (command == _ui_phi) {
    _phi = _ui_phi->GetNewDoubleValue(value);
  }
}

const std::string Generator::InfoString() const {
  std::stringstream out;
  out << "Generator Info:\n  "
      << "Name: "        << _name                       << "\n  "
      << "Description: " << _description                << "\n  "
      << "pT: "          << G4BestUnit(_pT, "Momentum") << "\n  "
      << "eta: "         << _eta                        << "\n  "
      << "phi: "         << G4BestUnit(_phi, "Angle")   << "\n";
  return out.str();
}

G4PrimaryVertex* Generator::DefaultVertex() {
  return new G4PrimaryVertex(0, 0, 100*m, 0);
}

G4PrimaryParticle* Generator::CreateParticle(const G4int id,
                                             const G4ThreeVector& momentum) {
  return new G4PrimaryParticle(id, momentum.x(), momentum.y(), momentum.z());
}

G4PrimaryParticle* Generator::CreateParticle(const G4int id,
                                             const G4double pT,
                                             const G4double eta,
                                             const G4double phi) {
  return new G4PrimaryParticle(
    id, pT*std::sinh(eta), pT*std::sin(phi), -pT*std::cos(phi));
}

RangeGenerator::RangeGenerator(const G4String& name,
                               const G4String& description,
                               const G4int id,
                               const G4double pT,
                               const G4double eta,
                               const G4double phi)
    : RangeGenerator(name, description, id, pT, pT, -eta, eta, -phi, phi) {}

RangeGenerator::RangeGenerator(const G4String& name,
                               const G4String& description,
                               const G4int id,
                               const G4double pT_min,
                               const G4double pT_max,
                               const G4double eta_min,
                               const G4double eta_max,
                               const G4double phi_min,
                               const G4double phi_max)
    : Generator(name, description, id, pT_min, 0, 0),
      _pT_min(pT_min), _pT_max(pT_max), _eta_min(eta_min), _eta_max(eta_max),
      _phi_min(phi_min), _phi_max(phi_max) {

  _ui_pT_min = CreateCommand<G4CMD_DoubleUnit>("pT_min", "Set Minimum Transverse Momentum.");
  _ui_pT_min->SetParameterName("pT_min", false, false);
  _ui_pT_min->SetRange("pT_min > 0");
  _ui_pT_min->SetDefaultUnit("GeV/c");
  _ui_pT_min->SetUnitCandidates("eV/c keV/c MeV/c GeV/c");
  _ui_pT_min->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_pT_max = CreateCommand<G4CMD_DoubleUnit>("pT_max", "Set Maximum Transverse Momentum.");
  _ui_pT_max->SetParameterName("pT_max", false, false);
  _ui_pT_max->SetRange("pT_max > 0");
  _ui_pT_max->SetDefaultUnit("GeV/c");
  _ui_pT_max->SetUnitCandidates("eV/c keV/c MeV/c GeV/c");
  _ui_pT_max->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_eta_min = CreateCommand<G4CMD_Double>("eta_min", "Set Minimum Pseudorapidity.");
  _ui_eta_min->SetParameterName("eta_min", false);
  _ui_eta_min->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_eta_max = CreateCommand<G4CMD_Double>("eta_max", "Set Maximum Pseudorapidity.");
  _ui_eta_max->SetParameterName("eta_max", false);
  _ui_eta_max->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_phi_min = CreateCommand<G4CMD_DoubleUnit>("phi_min", "Set Minimum Semi-Opening Angle.");
  _ui_phi_min->SetParameterName("phi_min", false, false);
  _ui_phi_min->SetDefaultUnit("deg");
  _ui_phi_min->SetUnitCandidates("degree deg radian rad milliradian mrad");
  _ui_phi_min->AvailableForStates(G4State_PreInit, G4State_Idle);

  _ui_phi_max = CreateCommand<G4CMD_DoubleUnit>("phi_max", "Set Maximum Semi-Opening Angle.");
  _ui_phi_max->SetParameterName("phi_max", false, false);
  _ui_phi_max->SetDefaultUnit("deg");
  _ui_phi_max->SetUnitCandidates("degree deg radian rad milliradian mrad");
  _ui_phi_max->AvailableForStates(G4State_PreInit, G4State_Idle);
}

void RangeGenerator::GeneratePrimaryVertex(G4Event* event) {
  auto vertex = DefaultVertex();
  _pT  = G4RandFlat::shoot(_pT_min, _pT_max);
  _eta = G4RandFlat::shoot(_eta_min, _eta_max);
  _phi = G4RandFlat::shoot(_phi_min, _phi_max);
  vertex->SetPrimary(CreateParticle(_id, _pT, _eta, _phi));
  event->AddPrimaryVertex(vertex);
}

void RangeGenerator::SetNewValue(G4UIcommand* command, G4String value) {
  if (command == _ui_id) {
    _id = _ui_id->GetNewIntValue(value);

  } else if (command == _ui_pT) {
    _pT = _ui_pT->GetNewDoubleValue(value);
    _pT_min = _pT;
    _pT_max = _pT;
  } else if (command == _ui_pT_min) {
    _pT_min = _ui_pT_min->GetNewDoubleValue(value);
  } else if (command == _ui_pT_max) {
    _pT_max = _ui_pT_max->GetNewDoubleValue(value);

  } else if (command == _ui_eta) {
    _eta = _ui_eta->GetNewDoubleValue(value);
    _eta_min = _eta;
    _eta_max = _eta;
  } else if (command == _ui_eta_min) {
    _eta_min = _ui_eta_min->GetNewDoubleValue(value);
  } else if (command == _ui_eta_max) {
    _eta_max = _ui_eta_max->GetNewDoubleValue(value);

  } else if (command == _ui_phi) {
    _phi = _ui_phi->GetNewDoubleValue(value);
    _phi_min = _phi;
    _phi_max = _phi;
  } else if (command == _ui_phi_min) {
    _phi_min = _ui_phi_min->GetNewDoubleValue(value);
  } else if (command == _ui_phi_max) {
    _phi_max = _ui_phi_max->GetNewDoubleValue(value);
  }
}

const std::string RangeGenerator::InfoString() const {
  std::stringstream out;
  out << "Generator Info:\n  "
      << "Name: "        << _name                                             << "\n  "
      << "Description: " << _description                                      << "\n  "
      << "avg pT: "      << G4BestUnit(0.5 * (_pT_min + _pT_max), "Momentum") << "\n    "
      << "pT min: "      << G4BestUnit(_pT_min, "Momentum")                   << "\n    "
      << "pT max: "      << G4BestUnit(_pT_max, "Momentum")                   << "\n  "
      << "avg eta: "     << 0.5 * (_eta_min + _eta_max)                       << "\n    "
      << "eta min: "     << _eta_min                                          << "\n    "
      << "eta max: "     << _eta_max                                          << "\n  "
      << "avg phi: "     << G4BestUnit(0.5 * (_phi_min + _phi_max), "Angle")  << "\n    "
      << "phi min: "     << G4BestUnit(_phi_min, "Angle")                     << "\n    "
      << "phi max: "     << G4BestUnit(_phi_max, "Angle")                     << "\n";
  return out.str();
}

} } /* namespace MATHUSLA::MU */
