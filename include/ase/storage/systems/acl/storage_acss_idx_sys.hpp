#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_acss_idx_sys.hpp
 * @brief       StorageAcssIdxSystem - rebuilds the A/ACS lookup index each tick
 * @description Flattens realms, ACL rules, lattice links, required and held codewords
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @created     2026-08-16
 * @modified    2026-08-16
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
 * @brief StorageAcssIdxSystem - reverse index over the A/ACS relations
 *
 * Runs immediately before StorageAcssChkSystem and walks each relation ONCE, so the
 * ladder can answer its parent-to-children questions by key instead of by scan. The
 * index is rebuilt from scratch every tick rather than maintained incrementally: a
 * destroyed keycard or a retired rule then cannot leave a stale grant behind, and the
 * rebuild costs one linear pass where the scans cost a product.
 *
 * @schedule Integration - Runs before StorageAcssChkSystem
 * @reads    StorageStaRelmComponent, StorageAcssRuleComponent, StorageAcssCwrdComponent,
 *           StorageLatLnkComponent, StorageStaTaskComponent, StorageStaKycdComponent,
 *           StorageKycdCwrdComponent
 * @writes   StorageAcssIndexResourceManager (registry.ctx(), not a Component)
 * @depends  StorageIniSystem must have created the realms it indexes
 */
class StorageAcssIdxSystem : public ecs::System {
public:
    const char* name() const override { return "StorageAcssIdxSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
