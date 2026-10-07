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
target_link_libraries(test_units PRIVATE SHiP::Units)
add_test(NAME units COMMAND test_units)

# The mother-index invariants are pure header logic: no dictionary, no ROOT.
# SHiP::Units is linked for the include/ search path it carries.
add_executable(test_mc_mothers test_mc_mothers.cpp)
target_link_libraries(test_mc_mothers PRIVATE SHiP::Units)
add_test(NAME mc_mothers COMMAND test_mc_mothers)

# Every public header, compiled once on its own. There is no test to run: the
# build either succeeds or the header does not stand alone. It also puts the
# headers no other test includes into compile_commands.json, which is the only
# way clang-tidy reaches them - see the comment in headers_compile.cpp.
add_library(headers_compile OBJECT headers_compile.cpp)
target_link_libraries(headers_compile PRIVATE SHiPDataModel SHiP::Units)

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
        set(label ${CMAKE_MATCH_1})
        add_test(
            NAME compat_read_${label}
            COMMAND test_read_reference ${ref} ${label}
        )
        # The pre-v0.5.0 files were written by unversioned classes, which ROOT
        # 6.40 cannot rule-read into the versioned, renamed classes (see
        # data/README.md). Mark them expected-to-fail rather than deleting or
        # weakening them: they keep asserting the true values, and ctest starts
        # reporting them as FAILED the day a ROOT with the fix reaches
        # pixi.lock, because passing is what WILL_FAIL does not allow. That
        # report is the signal to drop this block and the known-issue sections.
        # The cost is that a genuine regression in one of these four reads
        # stays hidden until then. compat_read_v0.5.0 and compat_read_head
        # use the same reader and rules and are not marked, but nothing else
        # reads the pre-v0.5.0 layout checksums listed in the rules.
        string(REGEX REPLACE "^v" "" version "${label}")
        if((NOT label STREQUAL "head") AND (version VERSION_LESS 0.5.0))
            set_tests_properties(compat_read_${label} PROPERTIES WILL_FAIL TRUE)
        endif()
    endif()
endforeach()

# The reconstruction fixture: TrackFitResult and a detector wrapper, which the
# reference files above do not carry. One test per RNTuple in the file.
add_executable(write_reco_fixture write_reco_fixture.cpp)
target_link_libraries(
    write_reco_fixture
    PRIVATE ${link_dictionary} ROOT::Core ROOT::RIO ROOT::ROOTNTuple
)

add_executable(test_read_reco_fixture test_read_reco_fixture.cpp)
target_link_libraries(
    test_read_reco_fixture
    PRIVATE ${link_dictionary} ROOT::Core ROOT::RIO ROOT::ROOTNTuple
)

set(reco_fixture ${CMAKE_CURRENT_SOURCE_DIR}/data/reco_fixture_v0.5.0.root)
foreach(ntuple IN ITEMS trackfits wrappers)
    add_test(
        NAME compat_read_reco_${ntuple}_v0.5.0
        COMMAND test_read_reco_fixture ${reco_fixture} ${ntuple}
    )
endforeach()
# The wrappers' recHit -> rec_hit rename has no I/O rule, only one that stops
# the read with a Fatal error (see include/SHiP/LinkDef.h). Require that exact
# message: a read that silently returns default rec_hits must still fail here.
set_tests_properties(
    compat_read_reco_wrappers_v0.5.0
    PROPERTIES
        PASS_REGULAR_EXPRESSION
            "Fatal in <SHiP::UBTHit>: camelCase recHit data \\(data-model <= v0\\.5\\.0\\) cannot be read into rec_hit"
)

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
