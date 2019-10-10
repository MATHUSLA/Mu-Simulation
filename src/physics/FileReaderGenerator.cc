#include "physics/FileReaderGenerator.hh"

#include "physics/Particle.hh"
#include "analysis.hh"

#include <Geant4/G4AutoLock.hh>

#include <string>
#include <cstddef>
#include <fstream>
#include <limits>
#include <ios>
#include <stdexcept>

namespace MATHUSLA { namespace MU { namespace Physics {

namespace {

G4Mutex mutex = G4MUTEX_INITIALIZER;

} // anonymous namespace

FileReaderGenerator::FileReaderGenerator(const std::string &name,
                                         const std::string &description)
    : Generator(name, description, {}) {
  GenerateCommands();
}

void FileReaderGenerator::GeneratePrimaryVertex(G4Event *event) {
  std::size_t particle_parameters_index = _particle_parameters.size();
  {
    G4AutoLock lock(mutex);
    particle_parameters_index = _event_counter;
    ++_event_counter;
  }
  AddParticle(_particle_parameters.at(particle_parameters_index), *event);
}

void FileReaderGenerator::SetNewValue(G4UIcommand *command, G4String value) {
  if (command == _ui_pathname) {
    std::ifstream input_stream(value);
    while (input_stream) {
      const auto next_char = input_stream.peek();
      if (next_char == std::ifstream::traits_type::eof()) {
              break;
      }
      if (next_char == ' ' || next_char == '\t' || next_char == '\r' || next_char == '\n') {
              input_stream.ignore();
              continue;
      }
      if (next_char == '#') {
              input_stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
              continue;
      }
      _particle_parameters.emplace_back();
      auto &new_parameters = _particle_parameters.back();
      if ( ! (input_stream >> new_parameters.id
                           >> new_parameters.x
                           >> new_parameters.y
                           >> new_parameters.z
                           >> new_parameters.px
                           >> new_parameters.py
                           >> new_parameters.pz)) {
        throw std::runtime_error("Unable to parse particle parameters file");
      }
    }
    if ( ! input_stream) {
      throw std::runtime_error("Unable to read particle parameters file");
    }
  } else {
    Generator::SetNewValue(command, value);
  }
}

std::ostream &FileReaderGenerator::Print(std::ostream &os) const {
  os << "Generator Info:\n  "
     << "Name:        " << _name                           << "\n  "
     << "Description: " << _description                    << "\n  "
     << "Pathname:    " << _ui_pathname->GetCurrentValue() << "\n  ";
  return os;
}

const Analysis::SimSettingList FileReaderGenerator::GetSpecification() const {
  return Analysis::Settings(SimSettingPrefix,
    "",         _name,
    "_PATHNAME",  _ui_pathname->GetCurrentValue());
}

void FileReaderGenerator::GenerateCommands() {
  _ui_pathname = CreateCommand<Command::StringArg>("pathname", "Set pathname of particle parameters file.");
  _ui_pathname->SetParameterName("pathname", false, false);
  _ui_pathname->AvailableForStates(G4State_PreInit, G4State_Idle);
}

} } } // namespace MATHUSLA::MU::Physics
