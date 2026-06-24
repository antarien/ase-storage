#pragma once

/**
 * ASE MODULE DEFINITION
 *
 * @file        storage_module.hpp
 * @brief       StorageModule - A/ACS platform storage with Realms, Keycards, ACL
 * @description Registers all 15 storage systems across 4 schedules.
 *              Applies DSGN_109 patterns to real-world engine asset management.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @created     2026-04-05
 * @modified    2026-04-05
 * @version     1.0.0
 *
 * Systems by schedule:
 *
 * Initialization:
 *   StorageIniSystem (creates manager, seeds realms, loads JWT secret)
 *
 * Ingestion (Developer Keycard Pipeline):
 *   StorageKycdDrnSystem (drains developer API tokens from ResourceManager)
 *   StorageKycdVldSystem (validates developer JWT, tags entities)
 *     run_after: StorageKycdDrnSystem
 *   StorageKycdLnkSystem (links validated developer keycards to client entities)
 *     run_after: StorageKycdVldSystem
 *
 * Integration (ACL + Storage):
 *   StorageAcssChkSystem (10-step Mandatory Access Control)
 *   StorageFileWritSystem (filesystem CRUD via ResourceManager)
 *     run_after: StorageAcssChkSystem
 *   StorageWflwTranSystem (workflow label transitions)
 *     run_after: StorageFileWritSystem
 *   StorageCncmFltSystem (realm concealment filter)
 *     run_after: StorageAcssChkSystem
 *
 * Preservation:
 *   StorageKycdExpSystem (checks keycard expiration)
 *   StorageKycdRevSystem (checks keycard revocation)
 *     run_after: StorageKycdExpSystem
 *   StorageAudtWritSystem (batch-writes audit entries to MongoDB)
 *   StorageLatcSyncSystem (synchronizes lattice links)
 *
 * Observation:
 *   StorageQuotChkSystem (quota monitoring per realm)
 *   StorageVotePrcSystem (Vote of Confidence evaluation)
 *   StorageSrvlLogSystem (surveillance anomaly detection)
 *
 * ECS MODULE/PLUGIN DEFINITION COMPLIANCE
 *
 * [ ] name() returns correct name (ase-{module} or ase-pl-{plugin})
 * [ ] build() registers all systems in correct schedules
 * [ ] Startup systems registered first (run once at start)
 * [ ] Initialization systems registered (entity creation)
 * [ ] Integration/FixedUpdate systems registered with run_after() ordering
 * [ ] Replication/Transmission systems registered (network sync)
 * [ ] Persistence systems registered (database writes)
 * [ ] Shutdown systems registered (cleanup)
 * [ ] All system includes present
 * [ ] No circular dependencies
 */

#include <ase/ecs/app.hpp>
#include <ase/storage/version.hpp>

// Initialization
#include <ase/storage/systems/keycard/storage_ini_sys.hpp>

// Ingestion (Keycard Pipeline)
#include <ase/storage/systems/keycard/storage_kycd_drn_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_ntfy_drn_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_req_drn_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_vld_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_lnk_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_cwrd_pub_sys.hpp>

// Integration (ACL + Storage)
#include <ase/storage/systems/acl/storage_acss_chk_sys.hpp>
#include <ase/storage/systems/acl/storage_cncm_flt_sys.hpp>
#include <ase/storage/systems/fs/storage_file_writ_sys.hpp>
#include <ase/storage/systems/workflow/storage_wflw_tran_sys.hpp>

// Initialization (Edge Distribution)
#include <ase/storage/systems/edge/storage_edge_ini_sys.hpp>

// Preservation
#include <ase/storage/systems/keycard/storage_kycd_exp_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_rev_sys.hpp>
#include <ase/storage/systems/audit/storage_audt_writ_sys.hpp>
#include <ase/storage/systems/lattice/storage_latc_sync_sys.hpp>

// Curator
#include <ase/storage/systems/curator/storage_cur_prc_sys.hpp>

// Observation
#include <ase/storage/systems/quota/storage_quot_chk_sys.hpp>
#include <ase/storage/systems/vote/storage_vote_prc_sys.hpp>
#include <ase/storage/systems/audit/storage_srvl_log_sys.hpp>

namespace ase::storage {

struct StorageModule {
    static constexpr const char* name() { return "ase-storage"; }
    static constexpr const char* version() { return MODULE_VERSION; }

    void build(ecs::App& app) {
        // Initialization: manager entity, JWT secret, data dir, realm seeding
        app.add_system<StorageIniSystem>(ecs::Schedule::Initialization);
        // Edge-distribution realm seeding runs after the manager + data dir exist
        app.add_system_with<StorageEdgeIniSystem>(ecs::Schedule::Initialization)
            .run_after("StorageIniSystem");

        // Ingestion (60Hz): Developer Keycard pipeline (drain → validate → link)
        app.add_system<StorageKycdDrnSystem>(ecs::Schedule::Ingestion);
        // SDK Hub-bridge drain: convert SES_KYCD_NTF_* Hub keys into
        // StorageReqKycdComponent on the request entity before the main
        // keycard-req drain sees it (same tick).
        app.add_system_with<StorageKycdNtfyDrnSystem>(ecs::Schedule::Ingestion)
            .run_after("HubRcvDrnSystem");
        // HTTP-posted keycard issuance drain runs after the notify bridge so it
        // sees StorageReqKycdComponent + HubStgKycdPendTag together.
        app.add_system_with<StorageKycdReqDrnSystem>(ecs::Schedule::Ingestion)
            .run_after("StorageKycdNtfyDrnSystem");
        app.add_system_with<StorageKycdVldSystem>(ecs::Schedule::Ingestion)
            .run_after("StorageKycdDrnSystem");
        app.add_system_with<StorageKycdLnkSystem>(ecs::Schedule::Ingestion)
            .run_after("StorageKycdVldSystem");
        // Publish each authenticated session's keycard codewords to the Hub so
        // the edge-distribution A/ACS gate (ase-pl-edge-webserver) can enforce
        // them. Runs after the link system populates the validated session.
        app.add_system_with<StorageKycdCwrdPubSystem>(ecs::Schedule::Ingestion)
            .run_after("StorageKycdLnkSystem");

        // Integration (60Hz): ACL → file ops → workflow → concealment
        app.add_system<StorageAcssChkSystem>(ecs::Schedule::Integration);
        app.add_system_with<StorageFileWritSystem>(ecs::Schedule::Integration)
            .run_after("StorageAcssChkSystem");
        app.add_system_with<StorageWflwTranSystem>(ecs::Schedule::Integration)
            .run_after("StorageFileWritSystem");
        app.add_system_with<StorageCncmFltSystem>(ecs::Schedule::Integration)
            .run_after("StorageAcssChkSystem");

        // Integration (60Hz): Curator request processing (after ACL)
        app.add_system_with<StorageCurPrcSystem>(ecs::Schedule::Integration)
            .run_after("StorageAcssChkSystem");

        // Preservation (1Hz): expiry, revocation, audit write, lattice sync
        app.add_system<StorageKycdExpSystem>(ecs::Schedule::Preservation);
        app.add_system_with<StorageKycdRevSystem>(ecs::Schedule::Preservation)
            .run_after("StorageKycdExpSystem");
        app.add_system<StorageAudtWritSystem>(ecs::Schedule::Preservation);
        app.add_system<StorageLatcSyncSystem>(ecs::Schedule::Preservation);

        // Observation (1Hz): quota monitoring, vote evaluation, anomaly detection
        app.add_system<StorageQuotChkSystem>(ecs::Schedule::Observation);
        app.add_system<StorageVotePrcSystem>(ecs::Schedule::Observation);
        app.add_system<StorageSrvlLogSystem>(ecs::Schedule::Observation);
    }
};

}  // namespace ase::storage
