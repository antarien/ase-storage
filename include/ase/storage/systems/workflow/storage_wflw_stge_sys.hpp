#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_wflw_stge_sys.hpp
 * @brief       StorageWflwStgeSystem - Stages received workflow-promote requests
 * @description Takes every StorageReqWflwTranComponent request that carries
 *              StorageWflwRcvTag, whichever door filled it (the frame-113 operator command
 *              through StorageEdgeWflwFwdRcvSystem or a hub row HUB_STG_WFLW_REQ through
 *              StorageWflwDrnSystem), marks it pending, adds the released-gate tag when the
 *              target label is "released" and publishes the owner-scoped verdict PENDING.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @parity      server_only
 * @schedule    Ingestion
 * @usecase     usecase:100329 - stage workflow promotion
 * @created     2026-10-12
 * @modified    2026-10-12
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
 * @brief StorageWflwStgeSystem - one staging for both doors of the workflow promote
 *
 * @schedule Ingestion - run_after StorageWflwDrnSystem, so a hub row drained this tick is
 *           staged in the same tick; the frame-113 door runs earlier, in Reception
 * @reads    StorageReqWflwTranComponent + StorageWflwRcvTag
 * @writes   StorageWflwPendTag (+ StorageWflwGateTag when the target label is
 *           EDGE_LABEL_RELEASED), removes StorageWflwRcvTag; STG_WFLW_RES = PENDING
 *           (owner = hashed_string(path))
 * @depends  StorageEdgeWflwFwdRcvSystem and StorageWflwDrnSystem fill the requests
 */
class StorageWflwStgeSystem : public ecs::System {
public:
    const char* name() const override { return "StorageWflwStgeSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
