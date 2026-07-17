#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_wflw_gate_sys.hpp
 * @brief       StorageWflwGateSystem - released-gate companion-artifact check
 * @description Tag-filtered precondition for promotions targeting "released":
 *              the asset and ALL FOUR companion artifacts (.sig, .sha256,
 *              .spdx.json, .smoke) must exist in the realm. Pass removes
 *              StorageWflwGateTag; fail denies + audits + destroys the request.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
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
 * @brief StorageWflwGateSystem - released-gate artifact precondition
 *
 * @schedule Integration - run_after StorageFileWritSystem, before StorageWflwTranSystem
 * @reads    StorageReqWflwTranComponent + StorageWflwPendTag + StorageWflwGateTag,
 *           StorageStaRelmComponent (realm ref for the audit record)
 * @writes   removes StorageWflwGateTag on pass; on fail STG_WFLW_RES = DENIED_GATE,
 *           StorageBufAudtComponent + StorageAudtPendTag, request destroyed
 * @depends  StorageResourceManager in ctx (file_exists), StorageWflwDrnSystem staged
 */
class StorageWflwGateSystem : public ecs::System {
public:
    const char* name() const override { return "StorageWflwGateSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
