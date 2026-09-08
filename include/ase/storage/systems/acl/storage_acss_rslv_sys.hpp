#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_acss_rslv_sys.hpp
 * @brief       StorageAcssRslvSystem - Resolve realm and ACL rule for each pending request
 * @description The first half of the canonical Section 14.1 access ladder: authenticate the
 *              keycard, resolve the target realm (membership, public status, owner preset over
 *              path ancestors, concealment) and match the most specific ACL rule for the path.
 *              Requests that fail here are denied and audited; the rest carry their resolution
 *              into StorageAcssChkSystem, which runs the gates.
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

class StorageAcssRslvSystem : public ecs::System {
public:
    const char* name() const override { return "StorageAcssRslvSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
