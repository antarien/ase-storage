#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_edge_wflw_fwd_rcv_sys.hpp
 * @brief       StorageEdgeWflwFwdRcvSystem - Replica-forwarded operator workflow command receiver
 * @description Dist-tier receiver for the operator release-workflow WS console lane. Pops the
 *              forwarded command frame off the transport workflow lane (the Replica already
 *              verified the operator YK-JWT) and files each one as a StorageReqWflwFwdComponent
 *              marked promote, status or refused. For a promote it also grants the operator and
 *              files the module's own promote request (StorageReqWflwTranComponent marked
 *              StorageWflwRcvTag; StorageWflwStge/Gate/Tran/Pst then drive it).
 *              StorageEdgeWflwRplySystem answers every filed command in the same tick.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Reception
 * @usecase     usecase:100317 - relay operator command
 * @created     2026-07-12
 * @modified    2026-11-05
 * @version     00.00.39.00039 [core]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
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
 * Each tick it reads the frame code NET_CONST_BIN_MSG_EDGE_WFLW_FWD and writes its route onto
 * the transport workflow lane (LANE_WFLW), so the L2 KernelWbskDspcSystem routes it here. It
 * pops every queued forward frame [code][cli_conn:u32][op:u8][path][label][by], parsed at the
 * head width REP_CONST_EDGE_WFLW_FWD_HDR; the operation byte is compared with
 * REP_CONST_EDGE_WFLW_OP_PROMOTE and REP_CONST_EDGE_WFLW_OP_STATUS. Every frame becomes one
 * StorageReqWflwFwdComponent marked StorageWflwPromTag, StorageWflwStatTag or
 * StorageWflwRjctTag.
 *
 * @schedule Reception - alongside the kernel WS inbound demux, before StorageEdgeWflwRplySystem
 * @reads    transport::InboundQueueResourceManager LANE_WFLW (forwarded command frame),
 *           NET_CONST_BIN_MSG_EDGE_WFLW_FWD and REP_CONST_EDGE_WFLW_FWD_HDR / _OP_* (Hub, GLOBAL),
 *           SES_KYCD_PERM / SES_CLEARANCE (Hub, operator-keyed, before the grant)
 * @writes   StorageReqWflwFwdComponent + StorageWflwPromTag / StorageWflwStatTag /
 *           StorageWflwRjctTag (filed command), StorageReqWflwTranComponent +
 *           StorageWflwRcvTag (promote request), SES_KYCD_PERM / SES_CLEARANCE (Hub, the
 *           Replica-verified operator grant)
 * @depends  the dist links NO data client; promote drives the local workflow, the reply system answers
 */
class StorageEdgeWflwFwdRcvSystem : public ecs::System {
public:
    const char* name() const override { return "StorageEdgeWflwFwdRcvSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
