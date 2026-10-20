#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_wflw_drn_sys.hpp
 * @brief       StorageWflwDrnSystem - Drains Hub-bridge workflow-promote requests
 * @description Converts the hub rows of the kind HUB_STG_WFLW_REQ (a plugin files them
 *              with sdk::row_create and sdk::set_bytes) into module-local
 *              StorageReqWflwTranComponent request entities marked StorageWflwRcvTag,
 *              the same form the frame-113 door StorageEdgeWflwFwdRcvSystem fills;
 *              StorageWflwStgeSystem stages both.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
 * @usecase     usecase:100329 - stage workflow promotion
 * @created     2026-07-11
 * @modified    2026-10-12
 * @version     1.2.0
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
 * @reads    the hub rows HUB_STG_WFLW_REQ (path, target label, requester), released after
 * @writes   StorageReqWflwTranComponent + StorageWflwRcvTag on a new request entity
 * @depends  the writer of HUB_STG_WFLW_REQ in modules/ase-hub/data/hub_rows.json files the rows
 */
class StorageWflwDrnSystem : public ecs::System {
public:
    const char* name() const override { return "StorageWflwDrnSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
