#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_wflw_perm_sys.hpp
 * @brief       StorageWflwPermSystem - Enforce the PERM_PROMOTE axis on transition requests
 * @description The A/ACS permission gate of the workflow: the requester's keycard session must
 *              hold PERM_PROMOTE. Clearance was gated at the route (operator mint-gate);
 *              permission is enforced here. A missing key - no live session on this dist -
 *              means NO permissions, fail-closed, never a silent grant. Requests that fail are
 *              denied, audited and completed here; the rest reach StorageWflwTranSystem.
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

class StorageWflwPermSystem : public ecs::System {
public:
    const char* name() const override { return "StorageWflwPermSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
