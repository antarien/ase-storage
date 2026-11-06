#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_kycd_rev_sys.hpp
 * @brief       StorageKycdRevSystem - files the durable revoke record of a revoked keycard
 * @description Walks keycards marked StorageKycdRevTag that carry no StorageKycdRevPstTag yet,
 *              files one record entity (StorageBufKycdComponent with op KYCD_PST_OP_REVOKE,
 *              StorageBufKycdAccComponent, StorageKycdPstPendTag) and marks the keycard
 *              StorageKycdRevPstTag so the record is filed once. StorageKycdPstEmitSystem
 *              ships the record on frame 35.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Preservation
 * @parity      server_only
 * @design      DSGN_021
 * @usecase     usecase:101599 - revoke customer keycard
 * @actor       Account Protection Warden (ACCPROWARD)
 * @created     2026-04-05
 * @modified    2026-11-05
 * @version     2.0.0
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
 * @brief StorageKycdRevSystem - revoke record filing at 1Hz
 *
 * @schedule Preservation - run_after StorageKycdExpSystem
 * @reads    StorageStaKycdComponent + StorageKycdRevTag, excluding StorageKycdRevPstTag
 * @writes   record entity (StorageBufKycdComponent + StorageBufKycdAccComponent +
 *           StorageKycdPstPendTag), StorageKycdRevPstTag on the keycard
 */
class StorageKycdRevSystem : public ecs::System {
public:
    const char* name() const override { return "StorageKycdRevSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
