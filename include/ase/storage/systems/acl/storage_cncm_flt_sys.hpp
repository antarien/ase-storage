#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_cncm_flt_sys.hpp
 * @brief       StorageCncmFltSystem - Realm concealment filter
 * @description Filters realm listings based on concealment tags for non-members
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
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
 * @brief StorageCncmFltSystem - Hides concealed realms from non-members
 *
 * @schedule Integration - run_after StorageAcssChkSystem
 * @reads    StorageRelmConcealTag, StorageStaRelmComponent
 * @writes   Filters realm list responses (returns not_found instead of access_denied)
 * @depends  Realm entities with StorageRelmConcealTag
 */
class StorageCncmFltSystem : public ecs::System {
public:
    const char* name() const override { return "StorageCncmFltSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
