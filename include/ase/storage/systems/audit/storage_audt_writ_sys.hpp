#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_audt_writ_sys.hpp
 * @brief       StorageAudtWritSystem - Batch-writes audit entries to MongoDB
 * @description Collects audit entities with StorageAudtPendTag and persists at 1Hz
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Preservation
 * @created     2026-04-05
 * @modified    2026-04-05
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
 * @brief StorageAudtWritSystem - Access-decision persistence at 1Hz
 *
 * @schedule Preservation - Ships collected access decisions, then retires them
 * @reads    StorageAudtPendTag + StorageBufAudtComponent
 * @writes   frame 122 onto the outbound queue, destroys the audit entity
 * @depends  StorageAcssChkSystem, StorageWflwTranSystem, StorageWflwGateSystem,
 *           StorageWflwClnSystem and StorageEdgeAudtDrnSystem create one audit
 *           entity per access decision; StorageSrvlLogSystem must run BEFORE
 *           this one, because this system is the sole owner of that lifetime
 *           and Observation (72) is too late to read what Preservation (71)
 *           has already retired.
 */
class StorageAudtWritSystem : public ecs::System {
public:
    const char* name() const override { return "StorageAudtWritSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
