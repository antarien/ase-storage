#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_quot_chk_sys.hpp
 * @brief       StorageQuotChkSystem - Quota monitoring per realm
 * @description Checks storage usage against tier limits, warns at 80 percent
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Observation
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
 * @brief StorageQuotChkSystem - Tier-based quota enforcement
 *
 * @schedule Observation - 1Hz quota checks
 * @reads    StorageStaRelmComponent (tier), StorageResourceManager (realm usage)
 * @writes   log::warn when usage exceeds 80 percent of tier limit
 * @depends  Realm entities with tier data
 */
class StorageQuotChkSystem : public ecs::System {
public:
    const char* name() const override { return "StorageQuotChkSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
