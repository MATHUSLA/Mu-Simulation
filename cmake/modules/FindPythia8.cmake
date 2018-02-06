cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

find_path(PYTHIA8_INCLUDE_DIR Pythia8/Pythia.h Pythia8/Pythia8ToHepMC.h
    HINTS
        $ENV{PYTHIA8LOCATION}/include
        ${PYTHIA8LOCATION}/include
        $ENV{PYTHIA8}/include
        ${PYTHIA8}/include
)

set(PYTHIA_LIBRARY_HINTS
    HINTS
        $ENV{PYTHIA8LOCATION}/lib
        ${PYTHIA8LOCATION}/lib
        $ENV{PYTHIA8}/lib
        ${PYTHIA8}/lib
)

find_library(PYTHIA8_LIBRARY       NAMES pythia8        ${PYTHIA_LIBRARY_HINTS})
find_library(PYTHIA8_HEPMC_LIBRARY NAMES pythia8tohepmc ${PYTHIA_LIBRARY_HINTS})
find_library(PYTHIA8_HAPDF_LIBRARY NAMES lhapdfdummy    ${PYTHIA_LIBRARY_HINTS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Pythia8 DEFAULT_MSG PYTHIA8_LIBRARY)

mark_as_advanced(PYTHIA8_FOUND
                 PYTHIA8_LIBRARY
                 PYTHIA8_HEPMC_LIBRARY
                 PYTHIA8_HAPDF_LIBRARY)