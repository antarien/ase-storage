#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_relm_proj_sync_sys.hpp
 * @brief       StorageRelmProjSyncSystem - Creates one A/ACS realm per Engine project
 * @description Reads the Engine project hub (ENG_PROJ_COUNT + ENG_PROJ_<i> names + <proj_id>_owner
 *              names, exactly as ProjectHubDrnSystem materializes them — via the Hub L1 API, no
 *              L3→L3 include) and, for each project, creates a StorageStaRelmComponent whose id is
 *              the project id and whose owner is the project-owner user id string. This is the
 *              per-project realm data model the A/ACS ladder (StorageAcssChkSystem) needs so that a
 *              credential path customers/<org>/projects/<pid>/... resolves to a realm: the project
 *              owner passes owner-preset (grant), a non-owner is concealed (realm_not_found). Plus a
 *              PROTECTION_PROTECTED ACL rule for the credential subtree. Idempotent (keyed by realm id).
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
 * @created     2026-07-04
 * @modified    2026-07-04
 * @version     1.0.0
 *
 * ECS SYSTEM HEADER COMPLIANCE
 *
 * [ ] STATELESS - No member variables
 * [ ] Views created on demand, not stored
 * [ ] NO direct calls to other systems
 * [ ] Communication only via Components
 * [ ] Helpers in anonymous namespace (in .cpp, NOT static functions!)
 * [ ] Math functions from ase-math (Layer 0)
 * [ ] NO file-level static/constexpr (constants → types.hpp)
 * [ ] Registered in Module with correct Schedule
 * [ ] Filename matches convention
 * [ ] Class name derived from filename
 * [ ] ALL THREE METHODS DECLARED: on_start, tick, on_stop
 */

#include <ase/ecs/system.hpp>

namespace ase::storage {

/**
 * @brief StorageRelmProjSyncSystem - per-project A/ACS realm creation from the Engine project hub.
 *
 * For each live Engine project (ENG_PROJ_* Hub keys + <proj_id>_owner almanach name) creates, if
 * absent, a StorageStaRelmComponent{id=proj_id, owner=<owner user id>, default_protection=PROTECTED,
 * tier} with StorageRelmPersonalTag + StorageRelmActiveTag + StorageRelmConcealTag, plus a
 * StorageAcssRuleComponent for the credential subtree. Enables the ladder's owner-preset + concealment.
 *
 * @schedule Ingestion - after the project hub is populated (HubRcvDrnSystem), before the Integration ladder
 * @reads    Hub ENG_PROJ_COUNT/ENG_PROJ_<i>/ENG_PROJ_TIER + almanach names (Hub L1, no ase-projects include)
 * @writes   StorageStaRelmComponent, realm Tags, StorageAcssRuleComponent
 */
class StorageRelmProjSyncSystem : public ecs::System {
public:
    const char* name() const override { return "StorageRelmProjSyncSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
