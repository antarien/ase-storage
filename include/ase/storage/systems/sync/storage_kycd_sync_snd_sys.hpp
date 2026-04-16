#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_kycd_sync_snd_sys.hpp
 * @brief       StorageKycdSyncSndSystem - Engine→Replica keycard sync sender
 * @description Dispatches BIN_MSG_SESSION_KYCD_SYNC frames over the Engine's
 *              binary WebSocket client to Replica. The frame carries the
 *              fields Replica needs to materialise the per-client
 *              SessionStaIdnComponent + SessionStaAthzComponent + tag
 *              (installed by the ase-session Phase 3 drain/link systems).
 *              Idempotence is enforced via StorageKycdSntTag: validated
 *              sessions are sent exactly once per validation episode, and
 *              loss of StorageKycdVldTag triggers a matching invalidate
 *              frame and tag removal.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Transmission
 * @created     2026-04-16
 * @modified    2026-04-16
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
 * @brief StorageKycdSyncSndSystem - Binary-WS session identity sync sender
 *
 * @schedule Transmission (20Hz) - sender cadence in line with Replica
 *           transmission systems (ARCH_ASE_REP_SRV §4.1).
 * @reads    StorageStaIdnComponent, StorageStaKycdComponent, StorageStaRelmComponent,
 *           StorageKycdVldTag, StorageKycdSntTag.
 * @writes   StorageKycdSntTag (added after validate send, removed after invalidate).
 *           Binary frames via websocket::WebSocketResourceManager::send().
 */
class StorageKycdSyncSndSystem : public ecs::System {
public:
    const char* name() const override { return "StorageKycdSyncSndSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
