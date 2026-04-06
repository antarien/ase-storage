#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_srvl_log_sys.hpp
 * @brief       StorageSrvlLogSystem - Surveillance anomaly detection
 * @description Scans audit entries for suspicious patterns and rapid failures
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
 * @brief StorageSrvlLogSystem - Anomaly detection in access patterns
 *
 * @schedule Observation - 1Hz scan of recent audit entries
 * @reads    StorageBufAudtComponent (recent denied entries)
 * @writes   log::warn for suspicious patterns (rapid failures, bulk downloads)
 * @depends  StorageAcssChkSystem creates audit entities
 */
class StorageSrvlLogSystem : public ecs::System {
public:
    const char* name() const override { return "StorageSrvlLogSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
