#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_edge_tier_sys.hpp
 * @brief       StorageEdgeTierSystem - license tier of the edge_binaries realm from the hub
 * @description Writes the license tier of the edge_binaries realm from the hub row
 *              ENG_CONST_TIER_ENTERPRISE. The realm is seeded in Initialization, where the hub
 *              constants do not exist yet; this system sets its tier in Integration, after
 *              HubIniCnstSystem laid the constants and before the A/ACS policy gates weigh the
 *              quota of a write against it.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @parity      server_only
 * @design      DSGN_021
 * @usecase     usecase:100265 - check access clearance
 * @actor       Provider Credential Keeper (PROVCREKEE)
 * @created     2026-11-05
 * @modified    2026-11-05
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
 * @brief StorageEdgeTierSystem - edge realm license tier from ENG_CONST_TIER_ENTERPRISE
 *
 * Each tick it reads ENG_CONST_TIER_ENTERPRISE on GLOBAL and writes it into the tier byte of
 * the one realm StorageRelmEdgeTag marks. The value has one owner, the hub row; the realm
 * record carries what the row says, so the quota step of StorageAcssPolSystem compares a realm
 * tier and a tier code that come from the same place.
 *
 * @schedule Integration - run_after HubIniCnstSystem, before StorageAcssPolSystem
 * @reads    ENG_CONST_TIER_ENTERPRISE (Hub, GLOBAL), StorageRelmEdgeTag
 * @writes   StorageStaRelmComponent::tier (edge realm)
 * @depends  StorageEdgeIniSystem (seeds the realm), HubIniCnstSystem (lays the constant)
 */
class StorageEdgeTierSystem : public ecs::System {
public:
    const char* name() const override { return "StorageEdgeTierSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
