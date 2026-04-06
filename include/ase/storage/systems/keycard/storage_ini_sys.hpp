#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_ini_sys.hpp
 * @brief       StorageIniSystem - Storage module initialization
 * @description Creates manager entity, registers StorageResourceManager,
 *              loads JWT secret, sets data directory, seeds initial realms.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Initialization
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
 * @brief StorageIniSystem - Creates manager entity, seeds realms
 *
 * Creates StorageMgrTag entity, registers StorageResourceManager in ctx(),
 * loads AUTH_JWT_SECRET and ASE_DATA_DIR from environment,
 * seeds initial realms (public, org/adg) with realm entities + tags.
 *
 * @schedule Initialization - Runs once at boot before any other storage system
 * @reads    Environment variables AUTH_JWT_SECRET, ASE_DATA_DIR
 * @writes   StorageMgrTag, StorageStaRelmComponent, realm type/status Tags
 * @depends  Nothing - first storage system to run
 */
class StorageIniSystem : public ecs::System {
public:
    const char* name() const override { return "StorageIniSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
