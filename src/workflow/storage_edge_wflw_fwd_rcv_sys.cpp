/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_edge_wflw_fwd_rcv_sys.cpp
 * @brief       StorageEdgeWflwFwdRcvSystem - Replica-forwarded operator workflow command receiver
 * @description Pops the operator workflow commands the Replica forwards on LANE_WFLW and files
 *              each one as a StorageReqWflwFwdComponent marked promote, status or refused. A
 *              promote also grants the Replica-verified operator and files the release-workflow
 *              request StorageWflwStgeSystem stages. StorageEdgeWflwRplySystem answers every
 *              filed command in the same tick. The frame code, the frame head width and the two
 *              operation bytes are hub rows of their owners, read on GLOBAL in tick.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Reception
 * @design      DSGN_021
 * @usecase     usecase:100317 - relay operator command
 * @actor       Operator Command Dispatcher (OPERCOMDIS)
 * @created     2026-07-12
 * @modified    2026-11-05
 * @version     00.00.39.00039 [core]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * CAUSAL CHAIN (EDGE_WFLW_FWD: Replica operator command → filed command + workflow drive)
 *
 *   [ReplicaEdgeWflwSystem verified the operator YK-JWT, ships frame BIN_MSG_EDGE_WFLW_FWD]
 *          │
 *          │ L2 KernelWbskDspcSystem routes the frame code → LANE_WFLW
 *          ▼
 *   ┌───────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageEdgeWflwFwdRcvSystem (dist tier)         │
 *   │                                                               │
 *   │  ROUTE:   frame code from the hub onto LANE_WFLW              │
 *   │  POP:     drain LANE_WFLW frames                              │
 *   │  FILE:    StorageReqWflwFwdComponent + Prom / Stat / Rjct     │
 *   │  PROMOTE: operator grant + StorageReqWflwTranComponent        │
 *   │           + StorageWflwRcvTag (the request Stge stages)       │
 *   └───────────────────────────────────────────────────────────────┘
 *          │
 *          │ StorageEdgeWflwRplySystem answers every filed command this tick
 *          │ promote: StorageWflwStge/Gate/Tran/Pst drive the transition
 *          ▼
 *   [the Replica relays the verdict to the ase-cli connection]
 *
 * HUB Pattern (Active - reads the forward contract, grants the operator session)
 *
 * READS (from Hub):
 *   NET_CONST_BIN_MSG_EDGE_WFLW_FWD   (GLOBAL) - frame code of the forwarded command
 *   REP_CONST_EDGE_WFLW_FWD_HDR       (GLOBAL) - fixed head of the frame in bytes
 *   REP_CONST_EDGE_WFLW_OP_PROMOTE    (GLOBAL) - operation byte of a promote
 *   REP_CONST_EDGE_WFLW_OP_STATUS     (GLOBAL) - operation byte of a status read
 *   SES_KYCD_PERM, SES_CLEARANCE (owner = hashed_string(by)) - before the promote grant
 *
 * WRITES (to Hub):
 *   SES_KYCD_PERM, SES_CLEARANCE (owner = hashed_string(by)) - the Replica-verified operator
 *   grant the transition gate reads; the commands themselves are module components
 *
 * FLYWEIGHT Pattern (inbound lane)
 *   The forward frame is popped from the L1 transport InboundQueueResourceManager
 *   (LANE_WFLW, via ctx) into a bounded stack buffer. The dist host links NO data client.
 *
 * ECS SYSTEM IMPLEMENTATION COMPLIANCE
 *
 * [ ] Layer dependencies checked (only depend on lower layers)
 * [ ] Existing functions checked (ase-math, ase-utils, ase-containers)
 * [ ] Abbreviations defined in types.hpp or documentation
 * [ ] types.hpp created with all constants and enums
 * [ ] STATELESS? No member variables?
 * [ ] Views created on demand, not stored?
 * [ ] NO direct calls to other systems?
 * [ ] Communication only via Components?
 * [ ] Helpers in anonymous namespace (NOT static!)?
 * [ ] Math functions from ase-math (Layer 0)?
 * [ ] NO file-level static/constexpr?
 * [ ] Registered in Module with correct Schedule?
 * [ ] Filename matches convention?
 * [ ] Class name derived correctly from filename?
 * [ ] Using Deferred Deletion Pattern? (Tag + Batch Destroy)
 * [ ] NO destroy() on other entities during iteration?
 * [ ] Cleanup System in Schedule::Conclusion?
 * [ ] NO local arrays/vectors for collection?
 * [ ] Safe deletion (first collect, then delete)?
 * [ ] Not deleting other entities during iteration?
 * [ ] Not invalidating references during iteration?
 * [ ] 1 File = 1 System?
 * [ ] Folder structure matches convention?
 * [ ] components/, systems/, src/ have IDENTICAL subfolder structure?
 * [ ] Layer dependencies respected (no upward dependencies)?
 * [ ] NO inline nlohmann::json + .dump() in broadcast systems?
 * [ ] Serializer functions in anonymous namespace?
 * [ ] *NetBctReqSystem + *NetBctSndSystem pattern?
 * [ ] Math functions from ase-math? (lerp, clamp, noise)
 * [ ] Containers from ase-containers? (RingBuffer)
 * [ ] Types from ase-types? (Result, Option)
 * [ ] Utils from ase-utils? (UUID, hash)
 * [ ] No duplicate functionality across modules?
 * [ ] ONLY primitive types: int, float, uint32_t, bool, etc.
 * [ ] ONLY ase-math for math (NO std::min, std::max, std::clamp!)
 * [ ] ONLY ase-containers for containers (NO std::vector, std::map, std::unordered_map!)
 * [ ] ONLY ase-types for Result/Option (NO std::optional, std::expected!)
 * [ ] std:: FORBIDDEN except: <cstdint>, <cmath> basics, <cassert>
 * [ ] NO ARRAYS! (use Entity-per-Item + Tags!)
 * [ ] CAUSAL CHAIN documented (Input → Processing → Output)
 * [ ] HUB Pattern documented (READS/WRITES)
 * [ ] hub::get() for reads
 * [ ] hub::set() for writes
 * [ ] Method order: on_start → tick → on_stop
 * [ ] ALL THREE METHODS implemented
 * [ ] on_start/on_stop: log::debug with system name
 * [ ] log::warn() if value EXISTS but invalid (e.g., health < 0, temp > 1000)
 * [ ] log::error() for EVERY NOT_FOUND check (see ase-log/log.hpp ERR::CAT::*)
 * [ ] Unused params: (void)dt; or commented parameter name
 * [ ] NO switch/case statements? (use Tag-filtered Views or lookup tables!)
 * [ ] NO if-else chains for type dispatch? (use separate Systems per type!)
 * [ ] NO instanceof/dynamic_cast checks? (use Tags for entity classification!)
 * [ ] NO factory patterns with type enums? (use Component composition!)
 * [ ] NO inheritance hierarchies? (use Component composition!)
 * [ ] NO virtual dispatch for game logic? (only ecs::System base class allowed!)
 * [ ] NO singleton patterns? (use Manager Tags on entities!)
 * [ ] NO state machines with switch? (use Tag-based state + separate Systems!)
 * [ ] ALL behavior driven by Component DATA, not hardcoded logic?
 * [ ] NO hardcoded entity types? (types defined by Component composition!)
 * [ ] NO hardcoded processing order? (order via Schedule + run_after!)
 * [ ] NO hardcoded value ranges? (ranges in types.hpp constants!)
 * [ ] NO hardcoded special cases? (special cases = Tags + dedicated Systems!)
 * [ ] Formulas use Component fields, not magic numbers?
 * [ ] New behavior = new Component + new System, NOT if-else in existing code?
 * [ ] NO `find_*()` with View/Query? (use DUAL-PATTERN)
 * [ ] NO `check_*()`/`has_*()`/`is_*()` with View/Query? (use DUAL-PATTERN)
 * [ ] NO `get_*()` with View/Query? (use DUAL-PATTERN)
 * [ ] NO struct in namespace {}? (use Component)
 * [ ] NO collect-then-process? (use single-pass)
 * [ ] NO View/Query in Helper? (only pure math)
 * [ ] NO `bool has_*` for type categories in Components? (use Tags!)
 * [ ] NO `bool is_*` for type categories in Components? (use Tags!)
 * [ ] NO `uint8_t *_type` field with if-chain dispatch? (use Tag-filtered Views!)
 * [ ] Type determined by Tag composition, not boolean field?
 * [ ] N-item support via Entity-per-Item + Tags, not type booleans?
 * [ ] Tag-filtered Views per type, not if-chain in single loop?
 * [ ] NO Entity-per-Character pattern when loading strings?
 * [ ] String loading uses char[N] fixed arrays or Pointer Pattern?
 * [ ] String hashing via entt::hashed_string for lookup keys?
 * [ ] String data stored as single attribute, not per-character entities?
 * [ ] NO std::shared_ptr in Components? (use Flyweight Pattern!)
 * [ ] NO void* in Components? (use Flyweight Pattern!)
 * [ ] NO static std::unordered_map for resource storage? (use ResourceManager via ctx!)
 * [ ] External resources (shared_ptr, handles) accessed via registry.ctx().get<ResourceManager&>()?
 * [ ] ResourceManager registered in on_start() via registry.ctx().emplace<ResourceManager&>()?
 * [ ] Components store ONLY uint32_t IDs referencing external resources?
 */

// INCLUDES - ONLY THESE ARE ALLOWED!
// FORBIDDEN: <vector>, <map>, <unordered_map>, <optional>, <algorithm>
// ALLOWED:   <cstdint>, <cmath>, <cassert>, ase-* headers

// Own header FIRST
#include <ase/storage/systems/workflow/storage_edge_wflw_fwd_rcv_sys.hpp>
// Module constants (PERM_PROMOTE, EDGE_CLEARANCE_OPERATOR, MAX_*)
#include <ase/storage/types.hpp>
// The filed command and the three marks of what it is
#include <ase/storage/components/request/storage_req_wflw_fwd_comp.hpp>
#include <ase/storage/components/tag/storage_wflw_prom_tag.hpp>
#include <ase/storage/components/tag/storage_wflw_stat_tag.hpp>
#include <ase/storage/components/tag/storage_wflw_rjct_tag.hpp>
// The promote request this receiver files, marked received for StorageWflwStgeSystem
#include <ase/storage/components/request/storage_req_wflw_tran_comp.hpp>
#include <ase/storage/components/tag/storage_wflw_rcv_tag.hpp>
// Hub API ONLY (header isolation): the forward contract and the operator session grant go
// through api.hpp; NEVER a direct include.
#include <ase/hub/api.hpp>
// Types SSOT (is_not_found / is_in_rng sentinel + range checks on hub::get reads)
#include <ase/types/types.hpp>
// Transport lanes (L1 via ctx - the L2 demux fills LANE_WFLW; this module NEVER touches the socket)
#include <ase/transport/inbound_queue_resource_manager.hpp>
#include <ase/transport/types.hpp>
// Utils (L0 - safe C-string operations)
#include <ase/utils/strops.hpp>
#include <ase/log/log.hpp>

#include <cstdint>
#include <cstring>

#include <entt/core/hashed_string.hpp>

namespace ase::storage {
using namespace entt::literals;  // For "_hs hashed strings (Hub)

// Anonymous namespace for helper FUNCTIONS (NOT static!)
// NO STRUCTS HERE! NO View/Query operations in helpers! Only pure byte/char math!
namespace {

// Read a u16-length-prefixed string at `off` in [buf, buf+len). Copies at most out_size-1
// bytes into out (NUL-terminated) and advances off past the field. Returns false on a
// truncated/over-long field. Pure byte scan.
bool read_lp_str(const char* buf, uint32_t len, uint32_t& off, char* out, uint32_t out_size) {
    out[0] = '\0';
    if (off + 2u > len) return false;
    uint16_t sl = 0;
    std::memcpy(&sl, buf + off, 2);
    off += 2u;
    if (off + sl > len) return false;
    uint32_t n = sl;
    if (n > out_size - 1u) n = out_size - 1u;
    for (uint32_t i = 0; i < n; ++i) out[i] = buf[off + i];
    out[n] = '\0';
    off += sl;
    return true;
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageEdgeWflwFwdRcvSystem::on_start(ecs::Registry& registry) {
    log::debug("[StorageEdgeWflwFwdRcvSystem] Started");

    // Emplace the inbound queue if the demux on_start has not run yet (order-independent). The
    // route itself is written in tick, where the frame code of its owner can be read.
    if (registry.ctx().find<transport::InboundQueueResourceManager>() == nullptr) {
        registry.ctx().emplace<transport::InboundQueueResourceManager>();
    }
}

void StorageEdgeWflwFwdRcvSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto* queue = registry.ctx().find<transport::InboundQueueResourceManager>();
    if (queue == nullptr) return;

    const float byte_max = static_cast<float>(UINT8_MAX);
    const float code_f = hub::get(registry, hub::GLOBAL, "NET_CONST_BIN_MSG_EDGE_WFLW_FWD"_hs);
    if (ase::types::is_not_found(code_f)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageEdgeWflwFwdRcvSystem", "NET_CONST_BIN_MSG_EDGE_WFLW_FWD");
        return;
    }
    if (!ase::types::is_in_rng_f(code_f, 0.0f, byte_max)) {
        log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "StorageEdgeWflwFwdRcvSystem", hub::GLOBAL,
                  "NET_CONST_BIN_MSG_EDGE_WFLW_FWD", code_f, 0.0f, byte_max);
        return;
    }
    const uint8_t code = static_cast<uint8_t>(code_f);
    // Writing the same route again is a map assignment and changes nothing once it stands.
    queue->register_route(code, transport::LANE_WFLW);
    if (!queue->has_inbound(transport::LANE_WFLW)) return;

    // The frame head carries code and cli_conn; the operation byte follows it inside the lane.
    const float fields = static_cast<float>(sizeof(uint8_t) + sizeof(uint32_t));
    const float lane = static_cast<float>(transport::LANE_BUF_SZ);
    const float hdr_f = hub::get(registry, hub::GLOBAL, "REP_CONST_EDGE_WFLW_FWD_HDR"_hs);
    if (ase::types::is_not_found(hdr_f)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageEdgeWflwFwdRcvSystem", "REP_CONST_EDGE_WFLW_FWD_HDR");
        return;
    }
    if (!ase::types::is_in_rng_f(hdr_f, fields, lane - 1.0f)) {
        log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "StorageEdgeWflwFwdRcvSystem", hub::GLOBAL,
                  "REP_CONST_EDGE_WFLW_FWD_HDR", hdr_f, fields, lane - 1.0f);
        return;
    }
    const float promote_f = hub::get(registry, hub::GLOBAL, "REP_CONST_EDGE_WFLW_OP_PROMOTE"_hs);
    if (ase::types::is_not_found(promote_f)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageEdgeWflwFwdRcvSystem", "REP_CONST_EDGE_WFLW_OP_PROMOTE");
        return;
    }
    if (!ase::types::is_in_rng_f(promote_f, 0.0f, byte_max)) {
        log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "StorageEdgeWflwFwdRcvSystem", hub::GLOBAL,
                  "REP_CONST_EDGE_WFLW_OP_PROMOTE", promote_f, 0.0f, byte_max);
        return;
    }
    const float status_f = hub::get(registry, hub::GLOBAL, "REP_CONST_EDGE_WFLW_OP_STATUS"_hs);
    if (ase::types::is_not_found(status_f)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageEdgeWflwFwdRcvSystem", "REP_CONST_EDGE_WFLW_OP_STATUS");
        return;
    }
    if (!ase::types::is_in_rng_f(status_f, 0.0f, byte_max)) {
        log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "StorageEdgeWflwFwdRcvSystem", hub::GLOBAL,
                  "REP_CONST_EDGE_WFLW_OP_STATUS", status_f, 0.0f, byte_max);
        return;
    }
    const uint32_t hdr = static_cast<uint32_t>(hdr_f);
    const uint8_t op_promote = static_cast<uint8_t>(promote_f);
    const uint8_t op_status = static_cast<uint8_t>(status_f);

    char buf[transport::LANE_BUF_SZ] = {};
    uint32_t msg_len = 0;

    while (queue->pop_inbound(transport::LANE_WFLW, buf, transport::LANE_BUF_SZ, msg_len)) {
        if (msg_len < hdr + 1u) {
            // The frame comes from outside and is dropped; nothing corrects it later.
            log::warn(log::WRN::CAT::INPUT_REJECTED, "StorageEdgeWflwFwdRcvSystem", "EDGE_WFLW_FWD_frame_len");
            msg_len = 0;
            continue;
        }
        if (static_cast<uint8_t>(buf[0]) != code) {
            log::warn(log::WRN::CAT::INPUT_REJECTED, "StorageEdgeWflwFwdRcvSystem", "EDGE_WFLW_FWD_msg_type");
            msg_len = 0;
            continue;
        }

        uint32_t cli_conn = 0;
        std::memcpy(&cli_conn, buf + sizeof(uint8_t), sizeof(uint32_t));
        const uint8_t op = static_cast<uint8_t>(buf[hdr]);
        uint32_t off = hdr + 1u;

        char path[MAX_PATH_LEN] = {};
        char label[MAX_LABEL_LEN] = {};
        char by[MAX_OWNER_ID] = {};
        const bool ok = read_lp_str(buf, msg_len, off, path, sizeof(path))
                        && read_lp_str(buf, msg_len, off, label, sizeof(label))
                        && read_lp_str(buf, msg_len, off, by, sizeof(by));

        // Every frame becomes one filed command; its mark says what the reply answers.
        const ecs::Entity cmd = registry.create();
        auto& fwd = registry.emplace<StorageReqWflwFwdComponent>(cmd);
        fwd.cli_conn = cli_conn;
        ase::utils::str_copy(fwd.path, MAX_PATH_LEN, path);
        ase::utils::str_copy(fwd.target_label, MAX_LABEL_LEN, label);

        const bool malformed = !ok || path[0] == '\0';
        const bool promote = !malformed && op == op_promote;
        const bool status = !malformed && op == op_status;
        const bool attributed = label[0] != '\0' && by[0] != '\0';

        if (malformed) {
            ase::utils::str_copy(fwd.reason, MAX_REASON_LEN, "malformed_forward_frame");
            registry.emplace<StorageWflwRjctTag>(cmd);
        }
        if (promote && !attributed) {
            ase::utils::str_copy(fwd.reason, MAX_REASON_LEN, "missing_target_or_attribution");
            registry.emplace<StorageWflwRjctTag>(cmd);
        }
        if (promote && attributed) {
            // Replica-verified operator authority grant. The Replica already verified THIS
            // operator's YubiKey ES256 JWT (ReplicaEdgeWflwSystem via vault->jwt_login, edge
            // role) BEFORE forwarding the frame, so `by` is a trusted operator identity and the
            // Replica IS the trust boundary. The dist keeps NO operator keycard (ASE_EDGE_OPERATOR
            // unset), so StorageWflwTranSystem would deny this promote for lack of PERM_PROMOTE.
            // Publish the release-workflow session that PERM gate reads, owner-scoped by
            // hashed_string(by) which is the SAME owner the tran system derives from
            // requested_by: SES_KYCD_PERM with PERM_PROMOTE, SES_CLEARANCE at the operator tier.
            // Read-OR / max so a real live keycard session on this dist is never downgraded. This
            // grants NO more than the Replica already verified; it does not bypass that gate.
            const uint32_t op_owner = entt::hashed_string(by).value();
            float cur_perm_f = hub::get(registry, op_owner, "SES_KYCD_PERM"_hs);
            if (ase::types::is_not_found(cur_perm_f) || cur_perm_f < 0.0f) { cur_perm_f = 0.0f; }
            const uint16_t granted_perm =
                static_cast<uint16_t>(static_cast<uint16_t>(cur_perm_f) | PERM_PROMOTE);
            hub::set(registry, op_owner, "SES_KYCD_PERM"_hs, static_cast<float>(granted_perm));
            float cur_clr_f = hub::get(registry, op_owner, "SES_CLEARANCE"_hs);
            if (ase::types::is_not_found(cur_clr_f) ||
                cur_clr_f < static_cast<float>(EDGE_CLEARANCE_OPERATOR)) {
                hub::set(registry, op_owner, "SES_CLEARANCE"_hs,
                         static_cast<float>(EDGE_CLEARANCE_OPERATOR));
            }
            log::info("[StorageEdgeWflwFwdRcvSystem] operator authority granted (Replica-verified YK-JWT): owner={} by={} perm={} clearance={}",
                      op_owner, by, static_cast<uint32_t>(granted_perm),
                      static_cast<uint32_t>(EDGE_CLEARANCE_OPERATOR));

            // File the promote request in the module's own form: the request component with
            // the three strings, marked received. StorageWflwStgeSystem stages it in Ingestion
            // (pending tag, released gate, verdict), the same staging the hub-row door
            // StorageWflwDrnSystem feeds, so the staging stands once for both doors and this
            // receiver reaches no other module than the hub.
            const ecs::Entity req_ent = registry.create();
            auto& req = registry.emplace<StorageReqWflwTranComponent>(req_ent);
            ase::utils::str_copy_json_safe(req.path, MAX_PATH_LEN, path);
            ase::utils::str_copy_json_safe(req.target_label, MAX_LABEL_LEN, label);
            ase::utils::str_copy_json_safe(req.requested_by, MAX_OWNER_ID, by);
            registry.emplace<StorageWflwRcvTag>(req_ent);
            registry.emplace<StorageWflwPromTag>(cmd);
            log::info("[StorageEdgeWflwFwdRcvSystem] promote received: {} to {} (by {}, cli_conn={})",
                      path, label, by, cli_conn);
        }
        if (status) {
            registry.emplace<StorageWflwStatTag>(cmd);
        }
        if (!malformed && !promote && !status) {
            ase::utils::str_copy(fwd.reason, MAX_REASON_LEN, "unknown_operation");
            registry.emplace<StorageWflwRjctTag>(cmd);
        }

        msg_len = 0;
    }
}

void StorageEdgeWflwFwdRcvSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageEdgeWflwFwdRcvSystem] Stopped");
}

}  // namespace ase::storage
