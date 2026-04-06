#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_kycd_rev_sys.hpp
 * @brief       StorageKycdRevSystem - Tags revoked keycards
 * @description Checks revocation list, replaces VldTag with RevTag
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
 * @brief StorageKycdRevSystem - Checks keycard revocation at 1Hz
 *
 * @schedule Preservation - run_after StorageKycdExpSystem
 * @reads    StorageKycdVldTag + StorageStaKycdComponent (revoked flag in ECS)
 * @writes   StorageKycdRevTag (replaces StorageKycdVldTag)
 * @depends  Revocation data loaded from MongoDB at boot, updated via API
 */
class StorageKycdRevSystem : public ecs::System {
public:
    const char* name() const override { return "StorageKycdRevSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
