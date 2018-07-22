# CMakeLists.txt
#
# Copyright 2018 Brandon Gomes
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(MATHUSLA_MU)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

find_package(Geant4  REQUIRED gdml ui_all vis_all)
find_package(Pythia8 REQUIRED)
find_package(HepMC   REQUIRED)
find_package(ROOT    REQUIRED)

include(${Geant4_USE_FILE})

add_executable(simulation
    src/simulation.cc

    src/action/ActionInitialization.cc
    src/action/EventAction.cc
    src/action/GeneratorAction.cc
    src/action/RunAction.cc

    src/analysis.cc

    src/geometry/box/Box.cc
    src/geometry/box/Scintillator.cc

    src/geometry/Construction.cc
    src/geometry/Earth.cc
    src/geometry/MuonMapper.cc

    src/geometry/flat/Flat.cc
    src/geometry/flat/Layer.cc
    src/geometry/flat/Scintillator.cc

    src/geometry/prototype/Envelope.cc
    src/geometry/prototype/Prototype.cc
    src/geometry/prototype/RPC.cc
    src/geometry/prototype/Scintillator.cc

    src/physics/Generator.cc
    src/physics/HepMCGenerator.cc
    src/physics/PythiaGenerator.cc
    src/physics/RangeGenerator.cc

    src/tracking.cc

    src/util/command_line_parser.cc
)

target_compile_features(simulation PUBLIC cxx_std_14)
target_compile_options(simulation PUBLIC -Wno-missing-braces)

target_link_libraries(simulation PUBLIC
    ${Geant4_LIBRARIES}
    ${PYTHIA8_LIBRARY}
    ${ROOT_LIBRARIES}
    ${HEPMC_LIBRARIES})

target_include_directories(simulation PUBLIC
    ${Geant4_INCLUDE_DIRS}
    ${ROOT_INCLUDE_DIRS}
    ${PYTHIA8_INCLUDE_DIR}
    ${HEPMC_INCLUDE_DIR})

target_include_directories(simulation PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_SOURCE_DIR}/include>)

install(DIRECTORY scripts DESTINATION bin)
install(TARGETS simulation DESTINATION bin)
