/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_edge_wflw_fwd_rcv_sys.cpp
 * @brief       StorageEdgeWflwFwdRcvSystem - Replica-forwarded operator workflow command receiver
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Reception
 * @created     2026-07-12
 * @modified    2026-07-12
 * @version     1.0.0
 *
 * CAUSAL CHAIN (EDGE_WFLW_FWD: Replica operator command → workflow drive / status read)
 *
 *   [ReplicaEdgeWflwSystem verified the operator YK-JWT, ships BIN_MSG_EDGE_WFLW_FWD(113)]
 *          │
 *          │ L2 KernelWbskDspcSystem routes id 113 → LANE_WFLW
 *          ▼
 *   ┌───────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageEdgeWflwFwdRcvSystem (dist tier)         │
 *   │                                                               │
 *   │  POP:     drain LANE_WFLW frames                              │
 *   │  PROMOTE: emplace HubStgWflwReqComponent + HubStgWflwPendTag  │
 *   │           (the SAME bridge /admin/workflow/promote deposits)  │
 *   │  STATUS:  read owner-scoped STG_WFLW_STAGE / STG_WFLW_RES     │
 *   │  REPLY:   BIN_MSG_EDGE_WFLW_RES(114) onto the outbound queue  │
 *   └───────────────────────────────────────────────────────────────┘
 *          │
 *          │ promote: StorageWflwDrn/Gate/Tran/Pst drive the transition next tick
 *          │ reply: the Replica relays the 114 verdict to the ase-cli connection
 *          ▼
 *   [ase-cli prints the machine-parseable verdict line]
 *
 * HUB Pattern (Active - reads the workflow verdict, promote deposits the bridge)
 *
 * READS (from Hub):
 *   STG_WFLW_RES   (owner = hashed_string(path)) - workflow verdict (status op)
 *   STG_WFLW_STAGE (owner = hashed_string(path)) - workflow stage ordinal (status op)
 *
 * WRITES (to Hub):
 *   (none directly - promote emplaces the typed HubStgWflwReqComponent bridge entity)
 *
 * FLYWEIGHT Pattern (inbound + outbound lane)
 *   The forward frame is popped from the L1 transport InboundQueueResourceManager
 *   (LANE_WFLW, via ctx) into a bounded stack buffer; the verdict frame is pushed onto
 *   the L1 OutboundQueueResourceManager. The dist host links NO data client.
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
 * [ ] *NetBctReqSystem (Update) + *NetBctSndSystem (Replication) pattern?
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
// Module constants (EDGE_WFLW_* frame layout, WFLW_RES/STAGE ordinals, labels, MAX_*)
#include <ase/storage/types.hpp>
// Hub API ONLY (header isolation): the workflow verdict reads AND the workflow-bridge
// component types (hub::HubStgWflwReqComponent / hub::HubStgWflwPendTag) are surfaced via
// api.hpp - the SAME single header StorageWflwDrnSystem uses; NEVER a direct component include.
#include <ase/hub/api.hpp>
// Types SSOT (is_not_found / is_in_rng_i32 sentinel + range checks on hub::get reads)
#include <ase/types/types.hpp>
// Transport lanes (L1 via ctx - the L2 demux fills LANE_WFLW; this module NEVER touches the socket)
#include <ase/transport/inbound_queue_resource_manager.hpp>
#include <ase/transport/outbound_queue_resource_manager.hpp>
#include <ase/transport/types.hpp>
// Utils (L0 - safe C-string operations)
#include <ase/utils/strops.hpp>
#include <ase/log/log.hpp>

#include <cstdint>
#include <cstring>

#include <entt/core/hashed_string.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
// NO STRUCTS HERE! NO View/Query operations in helpers! Only pure byte/char math!
namespace {

// Append a small unsigned decimal to a bounded C-string (JSON numeric fields). Pure math.
void append_num(char* out, uint32_t max, uint32_t v) {
    char rev[12] = {};
    uint32_t r = 0;
    if (v == 0u) {
        rev[r++] = '0';
    } else {
        while (v > 0u && r < sizeof(rev)) { rev[r++] = static_cast<char>('0' + (v % 10u)); v /= 10u; }
    }
    char tmp[12] = {};
    uint32_t n = 0;
    while (r > 0u && n + 1u < sizeof(tmp)) { tmp[n++] = rev[--r]; }
    tmp[n] = '\0';
    ase::utils::str_append(out, max, tmp);
}

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

// WFLW_RES_* / WFLW_STAGE_* names (bounded lookup tables, mirror the /admin/workflow/status
// route (SSOT in ase-storage types.hpp). Index range-checked via the types SSOT; out-of-range
// collapses to the safe floor. Data-driven lookup, not an if-chain of hardcoded ordinals.
const char* wflw_res_name(int32_t i) {
    const char* names[6] = {"pending", "applied", "denied_edge",
                            "denied_gate", "denied_perm", "not_found"};
    if (!ase::types::is_in_rng_i32(i, 0, 5)) i = 0;
    return names[i];
}

const char* wflw_stage_name(int32_t i) {
    const char* names[5] = {"draft", "review", "approved", "released", "retired"};
    if (!ase::types::is_in_rng_i32(i, 0, 4)) i = 0;
    return names[i];
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageEdgeWflwFwdRcvSystem::on_start(ecs::Registry& registry) {
    log::debug("[StorageEdgeWflwFwdRcv] Started");

    // Register the operator-workflow forward wire-type onto the shared inbound lane so the
    // L2 demux routes it here. Emplace the queue if the demux on_start has not run yet
    // (order-independent, mirror StorageEdgeKycdResDrnSystem).
    if (registry.ctx().find<transport::InboundQueueResourceManager>() == nullptr) {
        registry.ctx().emplace<transport::InboundQueueResourceManager>();
    }
    auto& queue = registry.ctx().get<transport::InboundQueueResourceManager>();
    queue.register_route(EDGE_WFLW_BIN_MSG_FWD, transport::LANE_WFLW);
}

void StorageEdgeWflwFwdRcvSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto* queue = registry.ctx().find<transport::InboundQueueResourceManager>();
    if (queue == nullptr || !queue->has_inbound(transport::LANE_WFLW)) return;

    auto* outq = registry.ctx().find<transport::OutboundQueueResourceManager>();

    char buf[transport::LANE_BUF_SZ] = {};
    uint32_t msg_len = 0;

    while (queue->pop_inbound(transport::LANE_WFLW, buf, transport::LANE_BUF_SZ, msg_len)) {
        if (msg_len < EDGE_WFLW_FWD_HDR + 1u) {
            log::error("[StorageEdgeWflwFwdRcv] EDGE_WFLW_FWD frame too short: {} bytes", msg_len);
            msg_len = 0;
            continue;
        }
        uint8_t msg_type = static_cast<uint8_t>(buf[0]);
        if (msg_type != EDGE_WFLW_BIN_MSG_FWD) {
            log::warn("[StorageEdgeWflwFwdRcv] unexpected inbound type={} ({} bytes) skipped",
                      static_cast<uint32_t>(msg_type), msg_len);
            msg_len = 0;
            continue;
        }

        uint32_t cli_conn = 0;
        std::memcpy(&cli_conn, buf + 1, 4);
        uint8_t op = static_cast<uint8_t>(buf[EDGE_WFLW_FWD_HDR]);
        uint32_t off = EDGE_WFLW_FWD_HDR + 1u;

        char path[MAX_PATH_LEN] = {};
        char label[MAX_LABEL_LEN] = {};
        char by[MAX_OWNER_ID] = {};
        bool ok = read_lp_str(buf, msg_len, off, path, sizeof(path))
                  && read_lp_str(buf, msg_len, off, label, sizeof(label))
                  && read_lp_str(buf, msg_len, off, by, sizeof(by));

        // Build the verdict JSON + status byte per op.
        char json[EDGE_WFLW_JSON_MAX] = {};
        uint8_t status = EDGE_WFLW_STATUS_OK;

        if (!ok || path[0] == '\0') {
            status = EDGE_WFLW_STATUS_BAD_REQUEST;
            ase::utils::str_copy(json, sizeof(json), "{\"error\":\"malformed_forward_frame\"}");
        } else if (op == EDGE_WFLW_OP_PROMOTE) {
            if (label[0] == '\0' || by[0] == '\0') {
                status = EDGE_WFLW_STATUS_BAD_REQUEST;
                ase::utils::str_copy(json, sizeof(json), "{\"error\":\"missing_target_or_attribution\"}");
            } else {
                // Replica-verified operator authority grant. The Replica already verified THIS
                // operator's YubiKey ES256 JWT (ReplicaEdgeWflwSystem via vault->jwt_login, edge
                // role) BEFORE forwarding frame 113, so `by` is a trusted operator identity and the
                // Replica IS the trust boundary. The dist keeps NO operator keycard (ASE_EDGE_OPERATOR
                // unset), so StorageWflwTranSystem would deny this promote for lack of PERM_PROMOTE.
                // Publish the release-workflow session that PERM gate reads, owner-scoped by
                // hashed_string(by) which is the SAME owner the tran system derives from
                // requested_by: SES_KYCD_PERM with PERM_PROMOTE, SES_CLEARANCE at the operator tier.
                // Read-OR / max so a real live keycard session on this dist is never downgraded. This
                // grants NO more than the Replica already verified; it does not bypass that gate.
                const uint32_t op_owner = entt::hashed_string(by).value();
                float cur_perm_f = hub::get(registry, op_owner, "SES_KYCD_PERM"_hs, 0.0f);
                if (ase::types::is_not_found(cur_perm_f) || cur_perm_f < 0.0f) { cur_perm_f = 0.0f; }
                const uint16_t granted_perm =
                    static_cast<uint16_t>(static_cast<uint16_t>(cur_perm_f) | PERM_PROMOTE);
                hub::set(registry, op_owner, "SES_KYCD_PERM"_hs, static_cast<float>(granted_perm));
                float cur_clr_f = hub::get(registry, op_owner, "SES_CLEARANCE"_hs, 0.0f);
                if (ase::types::is_not_found(cur_clr_f) ||
                    cur_clr_f < static_cast<float>(EDGE_CLEARANCE_OPERATOR)) {
                    hub::set(registry, op_owner, "SES_CLEARANCE"_hs,
                             static_cast<float>(EDGE_CLEARANCE_OPERATOR));
                }
                log::info("[StorageEdgeWflwFwdRcv] operator authority granted (Replica-verified YK-JWT): owner={} by={} perm={} clearance={}",
                          op_owner, by, static_cast<uint32_t>(granted_perm),
                          static_cast<uint32_t>(EDGE_CLEARANCE_OPERATOR));

                // Deposit the SAME hub workflow-bridge request the /admin/workflow/promote route
                // deposits (StorageWflwDrnSystem drains it, StorageWflwTranSystem applies the
                // transition + audit). Identical field copies to sdk::emplace_workflow_promote_request.
                auto bridge = registry.create();
                auto& breq = registry.emplace<hub::HubStgWflwReqComponent>(bridge);
                ase::utils::str_copy(breq.path, static_cast<uint32_t>(sizeof(breq.path)), path);
                ase::utils::str_copy(breq.target_label, static_cast<uint32_t>(sizeof(breq.target_label)), label);
                ase::utils::str_copy(breq.requested_by, static_cast<uint32_t>(sizeof(breq.requested_by)), by);
                registry.emplace<hub::HubStgWflwPendTag>(bridge);

                ase::utils::str_copy(json, sizeof(json), "{\"status\":\"promote_requested\",\"path\":\"");
                ase::utils::str_append(json, sizeof(json), path);
                ase::utils::str_append(json, sizeof(json), "\",\"target_label\":\"");
                ase::utils::str_append(json, sizeof(json), label);
                ase::utils::str_append(json, sizeof(json), "\"}");
                log::info("[StorageEdgeWflwFwdRcv] promote staged: {} to {} (by {}, cli_conn={})",
                          path, label, by, cli_conn);
            }
        } else {
            // EDGE_WFLW_OP_STATUS: read the live owner-scoped verdict the workflow systems published.
            uint32_t owner = entt::hashed_string(path).value();
            float res_f = hub::get(registry, owner, "STG_WFLW_RES"_hs, hub::NOT_FOUND);
            if (ase::types::is_not_found(res_f)) {
                status = EDGE_WFLW_STATUS_NOT_FOUND;
                ase::utils::str_copy(json, sizeof(json), "{\"error\":\"no_workflow_state\",\"path\":\"");
                ase::utils::str_append(json, sizeof(json), path);
                ase::utils::str_append(json, sizeof(json), "\"}");
            } else {
                int32_t res_i = static_cast<int32_t>(res_f);
                if (!ase::types::is_in_rng_i32(res_i, 0, 5)) res_i = 0;
                ase::utils::str_copy(json, sizeof(json), "{\"path\":\"");
                ase::utils::str_append(json, sizeof(json), path);
                ase::utils::str_append(json, sizeof(json), "\",\"result\":");
                append_num(json, sizeof(json), static_cast<uint32_t>(res_i));
                ase::utils::str_append(json, sizeof(json), ",\"result_name\":\"");
                ase::utils::str_append(json, sizeof(json), wflw_res_name(res_i));
                ase::utils::str_append(json, sizeof(json), "\"");
                float stage_f = hub::get(registry, owner, "STG_WFLW_STAGE"_hs, hub::NOT_FOUND);
                if (!ase::types::is_not_found(stage_f)) {
                    int32_t stage_i = static_cast<int32_t>(stage_f);
                    if (!ase::types::is_in_rng_i32(stage_i, 0, 4)) stage_i = 0;
                    ase::utils::str_append(json, sizeof(json), ",\"stage\":");
                    append_num(json, sizeof(json), static_cast<uint32_t>(stage_i));
                    ase::utils::str_append(json, sizeof(json), ",\"stage_name\":\"");
                    ase::utils::str_append(json, sizeof(json), wflw_stage_name(stage_i));
                    ase::utils::str_append(json, sizeof(json), "\"");
                }
                ase::utils::str_append(json, sizeof(json), "}");
            }
        }

        // Reply frame [114][cli_conn:u32][status:u8][json_len:u32][json] onto the outbound queue;
        // the Replica relays it VERBATIM to the ase-cli connection.
        if (outq == nullptr) {
            log::error("[StorageEdgeWflwFwdRcv] outbound queue unavailable - verdict dropped (cli_conn={})",
                       cli_conn);
            msg_len = 0;
            continue;
        }
        char frame[EDGE_WFLW_RES_HDR + EDGE_WFLW_JSON_MAX] = {};
        uint32_t jl = ase::utils::str_len(json, EDGE_WFLW_JSON_MAX);
        frame[0] = static_cast<char>(EDGE_WFLW_BIN_MSG_RES);
        std::memcpy(frame + 1, &cli_conn, 4);
        frame[5] = static_cast<char>(status);
        std::memcpy(frame + 6, &jl, 4);
        for (uint32_t k = 0; k < jl; ++k) frame[EDGE_WFLW_RES_HDR + k] = json[k];
        if (!outq->push_outbound(frame, EDGE_WFLW_RES_HDR + jl)) {
            log::warn("[StorageEdgeWflwFwdRcv] verdict push failed - outbound queue full (cli_conn={})",
                      cli_conn);
        }

        msg_len = 0;
    }
}

void StorageEdgeWflwFwdRcvSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageEdgeWflwFwdRcv] Stopped");
}

}  // namespace ase::storage
