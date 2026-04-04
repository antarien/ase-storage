cmake_minimum_required(VERSION 3.22)

project(ase-storage
    VERSION 0.0.1
    DESCRIPTION "A/ACS-based asset storage with Realms, Keycards, ACL, Audit"
    LANGUAGES CXX
)

add_library(ase-storage STATIC
    # Keycard Pipeline (absorbed from ase-auth)
    src/keycard/storage_kcd_drn_sys.cpp
    src/keycard/storage_kcd_vld_sys.cpp
    src/keycard/storage_kcd_lnk_sys.cpp
    src/keycard/storage_kcd_exp_sys.cpp
    src/keycard/storage_kcd_rev_sys.cpp
    # ACL + Storage
    src/acl/storage_acl_chk_sys.cpp
    src/fs/storage_fs_wrt_sys.cpp
    src/workflow/storage_wfl_trans_sys.cpp
    src/acl/storage_cnc_flt_sys.cpp
    # Async + Persistence
    src/audit/storage_aud_wrt_sys.cpp
    src/lattice/storage_lat_sync_sys.cpp
    src/quota/storage_quo_chk_sys.cpp
    src/vote/storage_vot_prc_sys.cpp
    src/audit/storage_srv_log_sys.cpp
)

target_include_directories(ase-storage PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(ase-storage PUBLIC
    ase-ecs
    ase-log
    ase-mongodb
)

target_compile_features(ase-storage PUBLIC cxx_std_20)
