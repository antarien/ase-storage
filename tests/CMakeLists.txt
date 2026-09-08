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

# Der doctest-Einstiegspunkt steht HIER und nicht als `#define` im Kopf von test_storage_wflw.cpp,
# weil MACRO_DEFINITION_FORBIDDEN ihn dort meldet. Die Uebersetzungsdefinition wirkt auf der
# Kommandozeile, also VOR jedem #include — doctest erzeugt weiterhin genau ein main().
#
# UND SIE STEHT AUF DER QUELLE, NICHT AUF DEM ZIEL, weil dieses Ziel ZWEI Quellen hat. Ein
# target_compile_definitions gaebe das Makro BEIDEN Uebersetzungseinheiten — zwei main(), Bruch
# am Linker. Genau davor warnt der Kommentar oben an test_storage_sess_cln.cpp seit Backlog #111;
# set_source_files_properties haelt dieselbe Zusage, nur an der Stelle, wo der Validator sie
# nicht als Makrodefinition im Quelltext sieht.
#
# GEZAEHLT WERDEN DIE QUELLEN DES ZIELS, NIE DIE DATEIEN IM VERZEICHNIS: die einquellige Form
# steht in den anderen Testverzeichnissen dieses Baums und waere hier ein Linkerfehler.
set_source_files_properties(test_storage_wflw.cpp PROPERTIES
    COMPILE_DEFINITIONS DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
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
