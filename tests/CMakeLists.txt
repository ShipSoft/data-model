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
