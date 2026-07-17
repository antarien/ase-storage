#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_wflw_pst_sys.hpp
 * @brief       StorageWflwPstSystem - durable workflow-label persist (frame 112)
 * @description Drains StorageBufWflwComponent + StorageWflwPstPendTag buffers into
 *              BIN_MSG_EDGE_WFLW_PERSIST frames on the outbound queue: the Replica
 *              REPLACE-upserts each document into storage_workflow_labels keyed
 *              {realm,path} — the durable, attributed transition record.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Preservation
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
 * @brief StorageWflwPstSystem - frame-112 workflow-label persist drain
 *
 * @schedule Preservation - sibling of StorageAudtWritSystem
 * @reads    StorageBufWflwComponent + StorageWflwPstPendTag
 * @writes   BIN_MSG_EDGE_WFLW_PERSIST frames onto the L1 outbound queue
 * @depends  transport::OutboundQueueResourceManager in ctx (absent on tiers
 *           without an outbound lane — buffers then stay queued, never dropped)
 */
class StorageWflwPstSystem : public ecs::System {
public:
    const char* name() const override { return "StorageWflwPstSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
