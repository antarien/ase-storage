cmake_minimum_required(VERSION 3.20)

#[[
ASE MODULE BUILD CONFIGURATION

@file        CMakeLists.txt
@brief       Build configuration for ase-storage
@description A/ACS platform storage - Realms, Keycards, ACL, Audit.
             A/ACS patterns applied to real-world engine asset management.

@module      ase-storage
@layer       3 (Module)
@created     2026-04-04
@modified    2026-04-05
@version     00.00.01.00001 [stub]

CMAKE MODULE COMPLIANCE

[ ] cmake_minimum_required(VERSION 3.20) as FIRST line
[ ] project() with VERSION and LANGUAGES CXX
[ ] message(STATUS) for configuration output
[ ] add_library() with SHARED and EXPLICIT file list
[ ] NO file(GLOB_RECURSE ...) - explicit files only!
[ ] NO file(GLOB ...) - explicit files only!
[ ] Files organized by category with bracket comment headers
[ ] add_library(ase::{module} ALIAS ...) for namespace
[ ] target_include_directories() with PUBLIC and generator expressions
[ ] target_compile_features() with cxx_std_20
[ ] target_link_libraries() with PUBLIC/PRIVATE separation
[ ] Layer dependencies correct (L3 to L0, L1, L2 only)
[ ] ase::hub for cross-module communication (Star Topology)
[ ] VERBOTEN comment for forbidden dependencies
[ ] if(ASE_BUILD_TESTS) block for tests
[ ] add_subdirectory(tests) inside test block
[ ] include(GNUInstallDirs) for installation
[ ] install(DIRECTORY include/ ...) for headers
]]

# Parse local VERSION (SSOT) if not building from root
if(NOT DEFINED ASE_STORAGE_VERSION)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")
        message(FATAL_ERROR "VERSION file not found: ${CMAKE_CURRENT_SOURCE_DIR}/VERSION")
    endif()
    file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" _VERSION_LINES)
    foreach(_LINE ${_VERSION_LINES})
        if(_LINE MATCHES "^ASE_STORAGE=(.*)$")
            set(ASE_STORAGE_VERSION "${CMAKE_MATCH_1}")
        endif()
    endforeach()
    if(NOT DEFINED ASE_STORAGE_VERSION)
        message(FATAL_ERROR "ASE_STORAGE version not found in VERSION file")
    endif()
endif()

project(ase-storage VERSION ${ASE_STORAGE_VERSION} LANGUAGES CXX)

# Generate version.hpp from template (SSOT)
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/include/ase/storage/version.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/ase/storage/version.hpp"
    @ONLY
)

# Find OpenSSL for HMAC-SHA256 keycard validation
find_package(OpenSSL REQUIRED)

message(STATUS "Configuring ${PROJECT_NAME} v${PROJECT_VERSION}")

add_library(${PROJECT_NAME} SHARED
    src/module_export.cpp
    #[[
    MODULE HEADERS
    Umbrella, module definition, types, resource manager.
    ]]
    include/ase/storage/storage.hpp
    include/ase/storage/storage_module.hpp
    include/ase/storage/storage_resource_manager.hpp
    include/ase/storage/types.hpp

    #[[
    COMPONENTS - STATE
    Realm, keycard, ACL, audit, lattice, vote, task state data.
    ]]
    include/ase/storage/components/state/storage_sta_relm_comp.hpp
    include/ase/storage/components/state/storage_sta_kycd_comp.hpp
    include/ase/storage/components/state/storage_sta_idn_comp.hpp
    include/ase/storage/components/state/storage_sta_tkn_comp.hpp
    include/ase/storage/components/state/storage_sta_vote_comp.hpp
    include/ase/storage/components/state/storage_sta_task_comp.hpp
    include/ase/storage/components/state/storage_kycd_cwrd_comp.hpp
    include/ase/storage/components/state/storage_acss_rule_comp.hpp
    include/ase/storage/components/state/storage_acss_cwrd_comp.hpp
    include/ase/storage/components/state/storage_lat_lnk_comp.hpp
    include/ase/storage/components/state/storage_buf_audt_comp.hpp
    include/ase/storage/components/state/storage_req_acss_comp.hpp
    include/ase/storage/components/state/storage_vote_cnt_comp.hpp
    include/ase/storage/components/state/storage_bllt_vote_comp.hpp
    include/ase/storage/components/state/storage_sta_cur_cur_comp.hpp
    include/ase/storage/components/state/storage_req_cur_comp.hpp

    #[[
    COMPONENTS - TAG
    Classification tags for storage entities.
    ]]
    include/ase/storage/components/tag/storage_tag_mgr.hpp
    include/ase/storage/components/tag/storage_tag_kycd_vld.hpp
    include/ase/storage/components/tag/storage_tag_kycd_exp.hpp
    include/ase/storage/components/tag/storage_tag_kycd_rjct.hpp
    include/ase/storage/components/tag/storage_tag_kycd_rev.hpp
    include/ase/storage/components/tag/storage_tag_kycd_pend.hpp
    include/ase/storage/components/tag/storage_tag_acss_grant.hpp
    include/ase/storage/components/tag/storage_tag_acss_deny.hpp
    include/ase/storage/components/tag/storage_tag_audt_pend.hpp
    include/ase/storage/components/tag/storage_tag_vot_pend.hpp
    include/ase/storage/components/tag/storage_tag_relm_personal.hpp
    include/ase/storage/components/tag/storage_tag_relm_org.hpp
    include/ase/storage/components/tag/storage_tag_relm_public.hpp
    include/ase/storage/components/tag/storage_tag_relm_active.hpp
    include/ase/storage/components/tag/storage_tag_relm_suspended.hpp
    include/ase/storage/components/tag/storage_tag_relm_archived.hpp
    include/ase/storage/components/tag/storage_tag_relm_conceal.hpp
    include/ase/storage/components/tag/storage_tag_cur_unrated.hpp
    include/ase/storage/components/tag/storage_tag_cur_approved.hpp
    include/ase/storage/components/tag/storage_tag_cur_rejected.hpp
    include/ase/storage/components/tag/storage_tag_cur_rework.hpp
    include/ase/storage/components/tag/storage_tag_cur_req.hpp
    include/ase/storage/components/tag/storage_tag_cur_done.hpp

    #[[
    SYSTEMS - KEYCARD
    Init, drain, validate, link, expiry, revocation.
    ]]
    include/ase/storage/systems/keycard/storage_ini_sys.hpp
    include/ase/storage/systems/keycard/storage_kycd_drn_sys.hpp
    include/ase/storage/systems/keycard/storage_kycd_vld_sys.hpp
    include/ase/storage/systems/keycard/storage_kycd_lnk_sys.hpp
    include/ase/storage/systems/keycard/storage_kycd_exp_sys.hpp
    include/ase/storage/systems/keycard/storage_kycd_rev_sys.hpp

    #[[
    SYSTEMS - ACL + CONCEALMENT
    ]]
    include/ase/storage/systems/acl/storage_acss_chk_sys.hpp
    include/ase/storage/systems/acl/storage_cncm_flt_sys.hpp

    #[[
    SYSTEMS - FILESYSTEM
    ]]
    include/ase/storage/systems/fs/storage_file_writ_sys.hpp

    #[[
    SYSTEMS - WORKFLOW
    ]]
    include/ase/storage/systems/workflow/storage_wflw_tran_sys.hpp

    #[[
    SYSTEMS - AUDIT + SURVEILLANCE
    ]]
    include/ase/storage/systems/audit/storage_audt_writ_sys.hpp
    include/ase/storage/systems/audit/storage_srvl_log_sys.hpp

    #[[
    SYSTEMS - LATTICE
    ]]
    include/ase/storage/systems/lattice/storage_latc_sync_sys.hpp

    #[[
    SYSTEMS - QUOTA
    ]]
    include/ase/storage/systems/quota/storage_quot_chk_sys.hpp

    #[[
    SYSTEMS - VOTE
    ]]
    include/ase/storage/systems/vote/storage_vote_prc_sys.hpp

    #[[
    SYSTEMS - CURATOR
    Curator request processing (business logic only, no persistence).
    ]]
    include/ase/storage/systems/curator/storage_cur_prc_sys.hpp

    #[[
    RESOURCE MANAGER IMPLEMENTATION
    ]]
    src/storage_resource_manager.cpp

    #[[
    SYSTEM IMPLEMENTATIONS - KEYCARD
    ]]
    src/keycard/storage_ini_sys.cpp
    src/keycard/storage_kycd_drn_sys.cpp
    src/keycard/storage_kycd_vld_sys.cpp
    src/keycard/storage_kycd_lnk_sys.cpp
    src/keycard/storage_kycd_exp_sys.cpp
    src/keycard/storage_kycd_rev_sys.cpp

    #[[
    SYSTEM IMPLEMENTATIONS - ACL + CONCEALMENT
    ]]
    src/acl/storage_acss_chk_sys.cpp
    src/acl/storage_cncm_flt_sys.cpp

    #[[
    SYSTEM IMPLEMENTATIONS - FILESYSTEM
    ]]
    src/fs/storage_file_writ_sys.cpp

    #[[
    SYSTEM IMPLEMENTATIONS - WORKFLOW
    ]]
    src/workflow/storage_wflw_tran_sys.cpp

    #[[
    SYSTEM IMPLEMENTATIONS - AUDIT + SURVEILLANCE
    ]]
    src/audit/storage_audt_writ_sys.cpp
    src/audit/storage_srvl_log_sys.cpp

    #[[
    SYSTEM IMPLEMENTATIONS - LATTICE
    ]]
    src/lattice/storage_latc_sync_sys.cpp

    #[[
    SYSTEM IMPLEMENTATIONS - QUOTA
    ]]
    src/quota/storage_quot_chk_sys.cpp

    #[[
    SYSTEM IMPLEMENTATIONS - VOTE
    ]]
    src/vote/storage_vote_prc_sys.cpp

    #[[
    SYSTEM IMPLEMENTATIONS - CURATOR
    ]]
    src/curator/storage_cur_prc_sys.cpp
)
add_library(ase::storage ALIAS ${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "ase_storage"
    SUFFIX ".module"
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        ase::ecs          # Layer 1 - ECS framework
        ase::log          # Layer 1 - Logging
        ase::utils        # Layer 0 - String operations (strops.hpp)
        ase::types        # Layer 0 - Type helpers (is_not_found)
        ase::hub          # Hub API 2.0 (client identity via Mirror Tags)
        OpenSSL::Crypto   # HMAC-SHA256 for keycard validation
)

if(ASE_BUILD_TESTS)
    add_subdirectory(tests)
endif()

include(GNUInstallDirs)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
