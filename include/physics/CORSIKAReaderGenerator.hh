/*
 * include/physics/CORSIKAReaderGenerator.hh
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

#ifndef MU__PHYSICS_CORSIKA_READER_GENERATOR_HH
#define MU__PHYSICS_CORSIKA_READER_GENERATOR_HH
#pragma once

#include "Generator.hh"

namespace MATHUSLA { namespace MU {

namespace Physics { ////////////////////////////////////////////////////////////////////////////

//__CORSIKA Simulation Config Structure_________________________________________________________
struct CORSIKAConfig {
  int primary_id;
  double energy_slope,
         energy_min,
         energy_max,
         azimuth_min,
         azimuth_max,
         zenith_min,
         zenith_max;
};
//----------------------------------------------------------------------------------------------

//__CORSIKA Simulation Event Structure__________________________________________________________
struct CORSIKAEvent {
  std::vector<int> id;
  std::vector<double> t, x, y, z, px, py, pz;
  bool empty() const;
  std::size_t size() const;
  void clear();
  void reserve(std::size_t capacity);
  void push_back(int new_id,
                 double new_t,
                 double new_x,
                 double new_y,
                 double new_z,
                 double new_px,
                 double new_py,
                 double new_pz);
  const Particle operator[](const std::size_t index) const;
};
//----------------------------------------------------------------------------------------------

//__CORSIKA Simulation Event Vector_____________________________________________________________
using CORSIKAEventVector = std::vector<CORSIKAEvent>;
//----------------------------------------------------------------------------------------------

//__CORSIKA Simulation Generator________________________________________________________________
class CORSIKAReaderGenerator : public Generator {
public:
  CORSIKAReaderGenerator();

  void GeneratePrimaryVertex(G4Event* event);
  void SetNewValue(G4UIcommand* command,
                   G4String value);
  void SetFile(const std::string& path);

  virtual const Analysis::SimSettingList GetSpecification() const;

private:
  static G4ThreadLocal CORSIKAEventVector* _data;
  static G4ThreadLocal std::size_t _data_index;
  CORSIKAConfig _config;
  std::string _path;
  Command::StringArg* _read_file;
  // TODO: Command::StringArg* _read_directory;
};
//----------------------------------------------------------------------------------------------

} /* namespace Physics */ //////////////////////////////////////////////////////////////////////

} } /* namespace MATHUSLA::MU */

#endif /* MU__PHYSICS_CORSIKA_READER_GENERATOR_HH */
