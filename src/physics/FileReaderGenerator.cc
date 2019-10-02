#include "physics/FileReaderGenerator.hh"

#include "physics/Particle.hh"
#include "analysis.hh"

#include <fstream>
#include <limits>
#include <ios>
#include <stdexcept>

namespace MATHUSLA { namespace MU { namespace Physics {

namespace {

void skip_whitespace(std::ifstream &stream) {
  std::ifstream::int_type next_char;
  while (   (next_char = stream.peek()) != std::ifstream::traits_type::eof()
         && (next_char == '\t' || next_char == '\n' || next_char == ' ')) {
    stream.ignore();
  }
}

void skip_comments_and_whitespace(std::ifstream &stream) {
  std::ifstream::int_type next_char;
  while (   (skip_whitespace(stream), next_char = stream.peek()) != std::ifstream::traits_type::eof()
         && next_char == '#') {
    stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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

  if ( ! _input_stream) {
    throw std::runtime_error("Unable to read particle parameters file");
  }

  skip_comments_and_whitespace(_input_stream);

  if ( ! (_input_stream >> _particle.id
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
    _input_stream.open(value);
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
