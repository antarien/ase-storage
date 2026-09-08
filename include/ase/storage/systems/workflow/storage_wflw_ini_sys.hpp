#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_wflw_ini_sys.hpp
 * @brief       StorageWflwIniSystem - Adopt an unmanaged on-disk build as a draft
 * @description Publish deposits builds straight into the realm, so the first promote meets an
 *              asset that no ACL rule governs yet. An asset without a rule IS the draft stage:
 *              this system creates that rule, data-driven, and leaves the transition itself to
 *              StorageWflwTranSystem. A promote for a path that is not on disk at all is denied
 *              and audited here - there is nothing to adopt.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @created     2026-08-29
 * @modified    2026-08-29
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

class StorageWflwIniSystem : public ecs::System {
public:
    const char* name() const override { return "StorageWflwIniSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
