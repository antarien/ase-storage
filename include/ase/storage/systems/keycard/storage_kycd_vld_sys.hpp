#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_kycd_vld_sys.hpp
 * @brief       StorageKycdVldSystem - Validates keycard JWT via HMAC-SHA256
 * @description Validates signature, extracts claims, tags valid or rejected
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
 * @brief StorageKycdVldSystem - HMAC-SHA256 keycard verification
 *
 * @schedule Ingestion - run_after StorageKycdDrnSystem
 * @reads    StorageKycdPendTag + StorageStaTknComponent
 * @writes   StorageKycdVldTag or StorageKycdRjctTag + StorageStaIdnComponent
 * @depends  StorageKycdDrnSystem creates entities with StorageKycdPendTag
 */
class StorageKycdVldSystem : public ecs::System {
public:
    const char* name() const override { return "StorageKycdVldSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
