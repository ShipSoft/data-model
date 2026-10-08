cmake_minimum_required(VERSION 3.21)
project(SHiPDataModel VERSION 0.5.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ROOT REQUIRED COMPONENTS Core RIO)
find_package(mp-units 2.5 CONFIG REQUIRED)

add_library(SHiPDataModel SHARED)
target_sources(
    SHiPDataModel
    PUBLIC
        FILE_SET headers
            TYPE HEADERS
            BASE_DIRS include
            FILES
                include/SHiP/EventHeader.hpp
                include/SHiP/MCParticle.hpp
                include/SHiP/SimHit.hpp
                include/SHiP/SimParticle.hpp
                include/SHiP/SimResult.hpp
                include/SHiP/RecHit.hpp
                include/SHiP/TrackFitResult.hpp
                include/SHiP/RecParticle.hpp
                include/SHiP/detectors/UBTHit.hpp
                include/SHiP/detectors/SBTHit.hpp
                include/SHiP/detectors/StrawTubesHit.hpp
                include/SHiP/detectors/CaloHit.hpp
                include/SHiP/detectors/TimeDetHit.hpp
                include/SHiP/detectors/detector_id.hpp
)
target_link_libraries(SHiPDataModel PUBLIC ROOT::Core)
# Propagate the C++ standard to installed consumers: the ROOT dictionary is
# built at C++23, so consumers streaming these classes must match it. This is
# not carried by CMAKE_CXX_STANDARD, which does not export.
target_compile_features(SHiPDataModel PUBLIC cxx_std_23)
set_target_properties(SHiPDataModel PROPERTIES LINKER_LANGUAGE CXX)

# Canonical unit vocabulary (mp-units quantity types + views over the data
# classes). Headers-only and deliberately separate from SHiPDataModel: the
# quantity types must never enter the ROOT dictionary (ROOT cannot stream
# them), and units-only consumers must not pay for a ROOT linkage.
add_library(SHiPUnits INTERFACE)
add_library(SHiP::SHiPUnits ALIAS SHiPUnits)
target_sources(
    SHiPUnits
    INTERFACE
        FILE_SET headers
            TYPE HEADERS
            BASE_DIRS include
            FILES include/SHiP/Units.hpp include/SHiP/QuantityView.hpp
)
target_link_libraries(SHiPUnits INTERFACE mp-units::mp-units)
# Header-only and ROOT-free: C++20 is the true minimum (concepts, defaulted
# comparisons), matching what mp-units already requires.
target_compile_features(SHiPUnits INTERFACE cxx_std_20)

root_generate_dictionary(
  G__SHiPDataModel
  SHiP/EventHeader.hpp
  SHiP/MCParticle.hpp
  SHiP/SimHit.hpp
  SHiP/SimParticle.hpp
  SHiP/SimResult.hpp
  SHiP/RecHit.hpp
  SHiP/TrackFitResult.hpp
  SHiP/RecParticle.hpp
  SHiP/detectors/UBTHit.hpp
  SHiP/detectors/SBTHit.hpp
  SHiP/detectors/StrawTubesHit.hpp
  SHiP/detectors/CaloHit.hpp
  SHiP/detectors/TimeDetHit.hpp
  MODULE
  SHiPDataModel
  LINKDEF
  include/SHiP/LinkDef.h
)

# Workaround for a rootcling bug seen with ROOT 6.40.04: when a selected header
# includes <algorithm>, rootcling writes `#include "execution_defs.h"` into the
# generated dictionary. It means libstdc++'s pstl/execution_defs.h, which sits a
# directory below the C++ include root, so the bare name resolves against
# nothing and compiling the dictionary fails. MCParticle.hpp is the header that
# triggers it here, for the std::find in mothers_are_consistent.
#
# Putting the pstl directory on the include path makes the bare name resolve.
# It goes on the dictionary object library rather than on SHiPDataModel so that
# libstdc++'s internal headers stay out of reach of our own sources, which have
# no business including them by basename.
#
# Whether rootcling emits the bad include varies between installations built
# from the same pixi.lock, with the same compiler and the same flags, for
# reasons we could not pin down, so this is keyed on the directory existing
# rather than on a ROOT version. See root-project/root#23640; remove this once
# rootcling emits a path that resolves.
find_path(
    SHIP_CXX_INCLUDE_ROOT
    NAMES pstl/execution_defs.h
    HINTS ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}
    NO_DEFAULT_PATH
)
mark_as_advanced(SHIP_CXX_INCLUDE_ROOT)
if(SHIP_CXX_INCLUDE_ROOT AND TARGET G__SHiPDataModel)
    target_include_directories(
        G__SHiPDataModel
        PRIVATE ${SHIP_CXX_INCLUDE_ROOT}/pstl
    )
endif()

include(GNUInstallDirs)

install(
    TARGETS SHiPDataModel
    EXPORT SHiPDataModelTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    FILE_SET headers DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(TARGETS SHiPUnits EXPORT SHiPDataModelTargets FILE_SET headers)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/libSHiPDataModel_rdict.pcm
        ${CMAKE_CURRENT_BINARY_DIR}/libSHiPDataModel.rootmap
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(
    EXPORT SHiPDataModelTargets
    FILE SHiPDataModelTargets.cmake
    NAMESPACE SHiP::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SHiPDataModel
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/SHiPDataModelConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SHiPDataModelConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SHiPDataModel
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/SHiPDataModelConfigVersion.cmake
    COMPATIBILITY AnyNewerVersion
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/SHiPDataModelConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/SHiPDataModelConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SHiPDataModel
)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
