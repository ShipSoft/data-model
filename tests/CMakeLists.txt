find_package(ROOT REQUIRED COMPONENTS Core RIO Tree ROOTNTuple)

# The data classes are header-only, so the test executables reference no
# symbols from libSHiPDataModel. Disable --as-needed for it to keep the
# DT_NEEDED entry that loads the ROOT dictionary at startup.
set(link_dictionary -Wl,--no-as-needed SHiPDataModel -Wl,--as-needed)

add_executable(test_ttree_io test_ttree_io.cpp)
target_link_libraries(
    test_ttree_io
    PRIVATE ${link_dictionary} ROOT::Core ROOT::RIO ROOT::Tree
)
add_test(NAME ttree_io COMMAND test_ttree_io)

add_executable(test_rntuple_io test_rntuple_io.cpp)
target_link_libraries(
    test_rntuple_io
    PRIVATE ${link_dictionary} ROOT::Core ROOT::RIO ROOT::ROOTNTuple
)
add_test(NAME rntuple_io COMMAND test_rntuple_io)

# Units and quantity views are headers-only: no dictionary, no ROOT.
add_executable(test_units test_units.cpp)
target_link_libraries(test_units PRIVATE SHiP::SHiPUnits)
add_test(NAME units COMMAND test_units)

# --- Backward-compatibility suite (see data/README.md) ---

add_executable(write_reference write_reference.cpp)
target_link_libraries(
    write_reference
    PRIVATE ${link_dictionary} ROOT::Core ROOT::RIO ROOT::ROOTNTuple
)

add_executable(test_read_reference test_read_reference.cpp)
target_link_libraries(
    test_read_reference
    PRIVATE ${link_dictionary} ROOT::Core ROOT::RIO ROOT::ROOTNTuple
)

# One ctest test per committed reference file, for clear failure attribution.
# CONFIGURE_DEPENDS: a file added at release time is picked up on reconfigure.
file(
    GLOB reference_files
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/data/reference_*.root"
)
foreach(ref IN LISTS reference_files)
    if(ref MATCHES "reference_(v[0-9]+\\.[0-9]+\\.[0-9]+|head)\\.root$")
        add_test(
            NAME compat_read_${CMAKE_MATCH_1}
            COMMAND test_read_reference ${ref} ${CMAKE_MATCH_1}
        )
    endif()
endforeach()

# The snapshot tool needs the list of dictionary classes; derive it from
# LinkDef.h so newly linked classes enter the snapshot automatically.
file(
    STRINGS ${PROJECT_SOURCE_DIR}/include/SHiP/LinkDef.h
    linkdef_lines
    REGEX "pragma link C\\+\\+ .*class SHiP::"
)
set(dictionary_classes "")
foreach(line IN LISTS linkdef_lines)
    if(NOT line MATCHES "std::vector")
        string(REGEX MATCH "SHiP::[A-Za-z0-9_]+" cls "${line}")
        string(APPEND dictionary_classes "\"${cls}\",\n")
    endif()
endforeach()
file(
    CONFIGURE
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dictionary_classes.inc
    CONTENT "${dictionary_classes}"
    @ONLY
)

add_executable(schema_snapshot schema_snapshot.cpp)
target_include_directories(schema_snapshot PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(
    schema_snapshot
    PRIVATE ${link_dictionary} ROOT::Core ROOT::RIO ROOT::ROOTNTuple
)
add_test(
    NAME schema_snapshot
    COMMAND
        schema_snapshot --check
        ${CMAKE_CURRENT_SOURCE_DIR}/data/schema_snapshot.txt
)
