#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_latc_sync_sys.hpp
 * @brief       StorageLatcSyncSystem - keeps the lattice links between realms current
 * @description Retires every lattice link whose lifetime has elapsed, so a bridge granted for a
 *              limited time (ARCH_ASE_STORAGE 8.3, the seven-day game-jam link) ends on its own
 *              and the ACL ladder only ever sees links that still stand.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Preservation
 * @parity      server_only
 * @usecase     usecase:101533 - withdraw a lapsed lattice bond
 * @actor       Horizontal Layer Steward (HLSTEWARD)
 * @created     2026-04-05
 * @modified    2026-10-29
 * @version     1.1.0
 *
 * PARITY DECISION
 *   The system reads the wall clock from StorageResourceManager in registry.ctx(), a server
 *   resource. The links themselves (StorageLatLnkComponent) and the expiry tag are shared and
 *   reach the client as data.
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
 * @brief StorageLatcSyncSystem - retires lattice links whose lifetime has elapsed
 *
 * @schedule Preservation - 1 Hz, the cadence ARCH_ASE_STORAGE 15 gives the lattice sync
 * @reads    StorageLatLnkComponent (expires_at), StorageLnkIdnComponent (the realms it bridges),
 *           StorageResourceManager (wall clock)
 * @writes   StorageLnkExpTag
 * @depends  StorageAcssChkSystem refuses the same link by the same clock (expires_at)
 */
class StorageLatcSyncSystem : public ecs::System {
public:
    const char* name() const override { return "StorageLatcSyncSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
