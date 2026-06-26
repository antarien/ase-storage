#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_edge_kycd_res_drn_sys.hpp
 * @brief       StorageEdgeKycdResDrnSystem - Parses Replica keycard documents into the gate session
 * @description Dist-tier receiver for the customer-auth edge-download lane. Pops
 *              BIN_MSG_EDGE_KYCD_RES off the transport keycard lane, parses the
 *              keycard MongoDB document the Replica found (the single auth
 *              authority), and publishes the SES_CLEARANCE + SES_KYCD_PERM +
 *              SES_KYCD_CWRD_* session the edge A/ACS gate reads — owner =
 *              hashed_string(user_id), the exact projection StorageKycdCwrdPubSystem
 *              produces. The dist host validates NOTHING itself and links NO data
 *              client; it only publishes what the Replica returned.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Reception
 * @created     2026-06-24
 * @modified    2026-06-24
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
 * @brief StorageEdgeKycdResDrnSystem - Replica → dist keycard-authz session publisher
 *
 * Registers the BIN_MSG_EDGE_KYCD_RES (id 34) wire-type onto the transport
 * keycard lane (LANE_KYC) at on_start so the L2 KernelWbskDspcSystem routes it
 * here. Each tick it pops every queued RES frame, and on a STATUS_OK frame parses
 * the keycard document the Replica FOUND (kycd_hash + user_id + clearance +
 * permission + codewords[]) and publishes the gate session at owner =
 * hashed_string(user_id): SES_CLEARANCE, SES_KYCD_PERM, SES_KYCD_CWRD_COUNT and
 * the per-index SES_KYCD_CWRD_<owner>_<i> debug-labels — byte-for-byte the keys
 * StorageKycdCwrdPubSystem emits, so the edge A/ACS gate authorizes the download
 * identically whether the keycard was minted locally or recovered from the
 * Replica. A NOT_FOUND / ERROR frame publishes NOTHING (the gate stays 401), so a
 * missing keycard never silently grants. The dist host parses what the Replica
 * returned; it links NO data client and validates NOTHING itself.
 *
 * @schedule Reception - alongside the kernel WS inbound demux
 * @reads    transport::InboundQueueResourceManager LANE_KYC (BIN_MSG_EDGE_KYCD_RES)
 * @writes   SES_CLEARANCE / SES_KYCD_PERM / SES_KYCD_CWRD_COUNT (Hub, owner-keyed)
 *           SES_KYCD_CWRD_<owner>_<i> (Hub debug-labels — the codeword set)
 * @depends  the dist edge routes (trigger_keycard_fetch) ship the matching
 *           BIN_MSG_EDGE_KYCD_REQ directly onto the transport outbound queue
 */
class StorageEdgeKycdResDrnSystem : public ecs::System {
public:
    const char* name() const override { return "StorageEdgeKycdResDrnSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
