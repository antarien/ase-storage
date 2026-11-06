#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_kycd_pst_emit_sys.hpp
 * @brief       StorageKycdPstEmitSystem - ships keycard persist records on frame 35
 * @description Walks StorageBufKycdComponent + StorageKycdPstPendTag records, builds the keycard
 *              document and pushes [35][req_id:u64][doc_len:u32][doc] onto the outbound queue;
 *              the Replica REPLACE-upserts the document keyed by user_id. The shipped record
 *              swaps StorageKycdPstPendTag for StorageKycdPstDoneTag.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Preservation
 * @parity      server_only
 * @design      DSGN_021
 * @usecase     usecase:101599 - revoke customer keycard
 * @actor       Account Protection Warden (ACCPROWARD)
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
 * @brief StorageKycdPstEmitSystem - frame-35 keycard persist drain
 *
 * @schedule Preservation - run_after StorageKycdRevSystem
 * @reads    StorageBufKycdComponent + StorageBufKycdAccComponent + StorageKycdPstPendTag,
 *           StorageStaRelmComponent (realm id via relm_ref),
 *           REP_CONST_BIN_MSG_EDGE_KYCD_PERSIST (hub GLOBAL)
 * @writes   frame 35 onto the L1 outbound queue, StorageKycdPstDoneTag
 * @depends  transport::OutboundQueueResourceManager in ctx (absent on tiers without an
 *           outbound lane - records then stay pending, never dropped)
 */
class StorageKycdPstEmitSystem : public ecs::System {
public:
    const char* name() const override { return "StorageKycdPstEmitSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
