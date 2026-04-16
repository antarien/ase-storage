#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_kycd_req_drn_sys.hpp
 * @brief       StorageKycdReqDrnSystem - Drains keycard issuance requests from HTTP notify into token entities
 * @description Consumes request entities that carry StorageReqKycdComponent + hub::HubStgKycdPendTag,
 *              creates token entities with StorageStaIdnComponent + StorageStaKycdComponent +
 *              StorageKycdPendTag and destroys the request entities (collect-then-destroy).
 *              Hands off to the existing Pend → Vld → Lnk keycard pipeline.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
 * @created     2026-04-16
 * @modified    2026-04-16
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
 * @brief StorageKycdReqDrnSystem - Drains HTTP-posted keycard issuance requests
 *
 * @schedule Ingestion - run_after HubRcvDrnSystem so Hub-side discovery tag is visible
 * @reads    StorageReqKycdComponent + hub::HubStgKycdPendTag (request entities)
 * @writes   StorageStaIdnComponent + StorageStaKycdComponent + StorageKycdPendTag (new token entities)
 * @destroys request entities after successful drain
 * @depends  plugins/ase-pl-webserver auth_routes emplaces request components
 */
class StorageKycdReqDrnSystem : public ecs::System {
public:
    const char* name() const override { return "StorageKycdReqDrnSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
