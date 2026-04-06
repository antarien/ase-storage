#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_kycd_drn_sys.hpp
 * @brief       StorageKycdDrnSystem - Drains keycard tokens from ResourceManager buffer
 * @description Creates token entities with StorageStaTknComponent + StorageKycdPendTag
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
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
 * @brief StorageKycdDrnSystem - Drains keycard tokens from buffer
 *
 * @schedule Ingestion - Runs with network drain systems
 * @reads    StorageResourceManager drain buffer (via ctx())
 * @writes   StorageStaTknComponent + StorageKycdPendTag (creates entities)
 * @depends  WebRTC callbacks fill the StorageResourceManager buffer
 */
class StorageKycdDrnSystem : public ecs::System {
public:
    const char* name() const override { return "StorageKycdDrnSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
