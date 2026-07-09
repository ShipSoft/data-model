cmake_minimum_required(VERSION 3.21)
project(SHiPDataModel VERSION 0.1.0 LANGUAGES CXX)

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
                include/SHiP/MCParticle.hpp
                include/SHiP/SimHit.hpp
                include/SHiP/SimParticle.hpp
                include/SHiP/SimResult.hpp
                include/SHiP/RecParticle.hpp
)
target_link_libraries(SHiPDataModel PUBLIC ROOT::Core)
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

root_generate_dictionary(
  G__SHiPDataModel
  SHiP/MCParticle.hpp
  SHiP/SimHit.hpp
  SHiP/SimParticle.hpp
  SHiP/SimResult.hpp
  SHiP/RecParticle.hpp
  MODULE
  SHiPDataModel
  LINKDEF
  include/SHiP/LinkDef.h
)

include(GNUInstallDirs)

install(
    TARGETS SHiPDataModel
    EXPORT SHiPDataModelTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    FILE_SET headers
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
