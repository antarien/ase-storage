#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_edge_wflw_fwd_rcv_sys.hpp
 * @brief       StorageEdgeWflwFwdRcvSystem - Replica-forwarded operator workflow command receiver
 * @description Dist-tier receiver for the operator release-workflow WS console lane.
 *              Pops BIN_MSG_EDGE_WFLW_FWD(113) off the transport workflow lane (the
 *              Replica already verified the operator YK-JWT), and for a promote deposits
 *              the SAME hub workflow-bridge request the /admin/workflow/promote route
 *              uses (StorageWflwDrn/Gate/Tran/Pst then drive it), or for a status reads
 *              the live STG_WFLW_STAGE/RES the workflow systems published. It replies
 *              BIN_MSG_EDGE_WFLW_RES(114) on the transport outbound queue, which the
 *              Replica relays to the originating ase-cli connection. Mirror of
 *              StorageEdgeKycdResDrnSystem (the customer-auth inbound lane).
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Reception
 * @created     2026-07-12
 * @modified    2026-07-12
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
 * @brief StorageEdgeWflwFwdRcvSystem - Replica → dist operator workflow command receiver
 *
 * Registers BIN_MSG_EDGE_WFLW_FWD(113) onto the transport workflow lane (LANE_WFLW) at
 * on_start so the L2 KernelWbskDspcSystem routes it here. Each tick it pops every queued
 * forward frame [113][cli_conn:u32][op:u8][path][label][by]; for EDGE_WFLW_OP_PROMOTE it
 * emplaces the hub workflow-bridge request (HubStgWflwReqComponent + HubStgWflwPendTag,
 * identical to sdk::emplace_workflow_promote_request) so the existing storage workflow
 * systems drive the transition, and for EDGE_WFLW_OP_STATUS it reads the owner-scoped
 * STG_WFLW_STAGE/RES the workflow published. It then ships BIN_MSG_EDGE_WFLW_RES(114)
 * (cli_conn echoed) onto the transport outbound queue for the Replica to relay to the CLI.
 *
 * @schedule Reception - alongside the kernel WS inbound demux (mirror StorageEdgeKycdResDrnSystem)
 * @reads    transport::InboundQueueResourceManager LANE_WFLW (BIN_MSG_EDGE_WFLW_FWD),
 *           STG_WFLW_STAGE / STG_WFLW_RES (Hub, owner-keyed, for status)
 * @writes   HubStgWflwReqComponent + HubStgWflwPendTag (promote bridge entity),
 *           transport::OutboundQueueResourceManager (BIN_MSG_EDGE_WFLW_RES verdict)
 * @depends  the dist links NO data client; promote drives the local workflow, status reads the Hub
 */
class StorageEdgeWflwFwdRcvSystem : public ecs::System {
public:
    const char* name() const override { return "StorageEdgeWflwFwdRcvSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
