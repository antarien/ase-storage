#[[
ASE TEST CONFIGURATION

@file        tests/CMakeLists.txt
@brief       Test configuration for ase-storage
@description Unit tests with doctest framework (workflow transition engine).

@module      ase-storage
@layer       3 (Module)
@created     2026-07-11
@modified    2026-07-11
@version     1.0.0

CMAKE TEST COMPLIANCE

[ ] add_executable() with explicit test file list
[ ] NO file(GLOB ...) for test files!
[ ] target_link_libraries() with module and doctest::doctest
[ ] set_target_properties() with RUNTIME_OUTPUT_DIRECTORY
[ ] add_test() for CTest integration
[ ] Test executable name: ${PROJECT_NAME}-test
]]

add_executable(${PROJECT_NAME}-test
    #[[
    TEST FILES
    Unit test source files (explicit, no GLOB).
    ]]
    test_storage_wflw.cpp
    #[[ Backlog #111: the SES_* family a dead session used to leave behind.
        test_storage_wflw.cpp owns DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN for this
        executable — this file must NOT define a second main. ]]
    test_storage_sess_cln.cpp
)

target_link_libraries(${PROJECT_NAME}-test
    PRIVATE
        ase::storage
        doctest::doctest
)

set_target_properties(${PROJECT_NAME}-test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

add_test(NAME ${PROJECT_NAME}-test COMMAND ${PROJECT_NAME}-test)
