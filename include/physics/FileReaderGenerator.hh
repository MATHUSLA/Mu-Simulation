#ifndef MU__PHYSICS__FIlE_READER_GENERATOR_HH
#define MU__PHYSICS__FIlE_READER_GENERATOR_HH

#include "physics/Generator.hh"

#include "ui.hh"

#include <string>
#include <iostream>
#include <cstddef>
#include <vector>

namespace MATHUSLA { namespace MU { namespace Physics {

class FileReaderGenerator : public Generator {
public:
  FileReaderGenerator(const std::string &name, const std::string &description);

  virtual ~FileReaderGenerator() = default;

  virtual void GeneratePrimaryVertex(G4Event *event);
  virtual void SetNewValue(G4UIcommand *command, G4String value);
  virtual std::ostream &Print(std::ostream &os = std::cout) const;
  virtual const Analysis::SimSettingList GetSpecification() const;

protected:
  virtual void GenerateCommands();

  std::size_t _event_counter = 0;
  std::vector<std::string> _input_lines;

  Command::StringArg *_ui_pathname;
};

} } } // namespace MATHUSLA::MU::Physics

#endif // MU__PHYSICS__FILE_READER_GENERATOR_HH
