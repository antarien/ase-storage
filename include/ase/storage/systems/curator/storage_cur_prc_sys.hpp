#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_cur_prc_sys.hpp
 * @brief       StorageCurPrcSystem - Curator request processing (business logic)
 * @description Processes incoming curation requests (rate, status, notes).
 *              Mutates StorageStaCurCurComponent entities and manages status Tags.
 *              NO persistence, NO broadcast — pure data mutation.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @created     2026-04-06
 * @modified    2026-04-06
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
 * @brief StorageCurPrcSystem - Process curator action requests
 *
 * @schedule Integration - After StorageAcssChkSystem (ACL must pass first)
 * @reads    StorageReqCurComponent + StorageCurReqTag
 * @writes   StorageStaCurCurComponent fields + status Tags
 *           (StorageCurUnratedTag, StorageCurApprovedTag,
 *            StorageCurRejectedTag, StorageCurReworkTag)
 * @depends  curator_routes.cpp creates StorageReqCurComponent entities
 */
class StorageCurPrcSystem : public ecs::System {
public:
    const char* name() const override { return "StorageCurPrcSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
