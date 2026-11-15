#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_edge_wflw_rply_sys.hpp
 * @brief       StorageEdgeWflwRplySystem - answers the Replica-forwarded operator workflow commands
 * @description Answers every StorageReqWflwFwdComponent StorageEdgeWflwFwdRcvSystem filed in the
 *              same tick on the reply frame (114): promote_requested for a filed promote, the
 *              live STG_WFLW_RES / STG_WFLW_STAGE verdict for a status read, and the reason for a
 *              refused frame. The Replica relays the reply to the originating ase-cli connection.
 *              The reply code, the reply envelope width, the three status bytes and the verdict
 *              width are hub rows of their owners, read on GLOBAL in tick.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Reception
 * @usecase     usecase:100317 - relay operator command
 * @created     2026-11-05
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
 * @brief StorageEdgeWflwRplySystem - reply to the forwarded operator workflow commands
 *
 * One tag-filtered view per kind of command (StorageWflwPromTag, StorageWflwStatTag,
 * StorageWflwRjctTag) builds the verdict document behind the reply envelope and pushes
 * [code][cli_conn:u32][status:u8][json_len:u32][json] onto the transport outbound queue;
 * every command entity is destroyed after the pass.
 *
 * @schedule Reception - run_after StorageEdgeWflwFwdRcvSystem, which files the commands
 * @reads    StorageReqWflwFwdComponent + Prom / Stat / Rjct tag,
 *           STG_WFLW_STAGE / STG_WFLW_RES (Hub, owner-keyed, for status)
 * @writes   transport::OutboundQueueResourceManager (BIN_MSG_EDGE_WFLW_RES verdict)
 * @depends  REP_CONST_EDGE_WFLW_* and NET_CONST_BIN_MSG_EDGE_WFLW_RES in hub_constants.json
 */
class StorageEdgeWflwRplySystem : public ecs::System {
public:
    const char* name() const override { return "StorageEdgeWflwRplySystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
