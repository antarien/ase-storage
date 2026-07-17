#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_wflw_drn_sys.hpp
 * @brief       StorageWflwDrnSystem - Drains Hub-bridge workflow-promote requests
 * @description Converts HubStgWflwReqComponent bridge entities (deposited by
 *              sdk::emplace_workflow_promote_request from the dist admin route)
 *              into module-local StorageReqWflwTranComponent request entities.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
 * @created     2026-07-11
 * @modified    2026-07-11
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
 * @brief StorageWflwDrnSystem - Hub-bridge drain for workflow-promote requests
 *
 * @schedule Ingestion - run_after HubRcvDrnSystem (mirror StorageKycdNtfyDrnSystem)
 * @reads    hub::HubStgWflwReqComponent + hub::HubStgWflwPendTag (bridge entities)
 * @writes   StorageReqWflwTranComponent + StorageWflwPendTag (+ StorageWflwGateTag
 *           when the target label is EDGE_LABEL_RELEASED); STG_WFLW_RES = PENDING
 * @depends  sdk::emplace_workflow_promote_request deposits the bridge entities
 */
class StorageWflwDrnSystem : public ecs::System {
public:
    const char* name() const override { return "StorageWflwDrnSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
