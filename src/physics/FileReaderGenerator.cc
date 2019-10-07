#include "physics/FileReaderGenerator.hh"

#include "physics/Particle.hh"
#include "analysis.hh"

#include <string>
#include <sstream>
#include <stdexcept>
#include <fstream>

namespace MATHUSLA { namespace MU { namespace Physics {

namespace {

void remove_comments(std::string &line) {
  const auto first_comment_character = line.find('#');
  if (first_comment_character == std::string::npos) {
    return;
  } else {
    line.erase(first_comment_character);
  }
}

bool is_blank(const std::string &line) {
  if (line.find_first_not_of(" \t\n") == std::string::npos) {
    return true;
  } else {
    return false;
  }
}

} // anonymous namespace

FileReaderGenerator::FileReaderGenerator(const std::string &name,
                                         const std::string &description)
    : Generator(name, description, {}) {
  GenerateCommands();
}

void FileReaderGenerator::GeneratePrimaryVertex(G4Event *event) {
  _particle.t = 0.0;

  std::istringstream stream(_input_lines[_event_counter++]);

  if ( ! (stream >> _particle.id
                 >> _particle.x
                 >> _particle.y
                 >> _particle.z
                 >> _particle.px
                 >> _particle.py
                 >> _particle.pz)) {
    throw std::runtime_error("Unable to parse particle parameters file");
  }

  AddParticle(_particle, *event);
}

void FileReaderGenerator::SetNewValue(G4UIcommand *command, G4String value) {
  if (command == _ui_pathname) {
    std::ifstream stream(value);
    if ( ! stream) {
      throw std::runtime_error("Unable to read particle parameters file");
    }
    while ( ! stream.eof()) {
      if ( ! stream.good() ) {
        throw std::runtime_error("Unable to read particle parameters file");
      }
      std::string temp;
      std::getline(stream, temp);
      remove_comments(temp);
      if ( ! is_blank(temp)) {
        _input_lines.push_back(temp);
      }
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
