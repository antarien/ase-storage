/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_edge_wflw_rply_sys.cpp
 * @brief       StorageEdgeWflwRplySystem - answers the Replica-forwarded operator workflow commands
 * @description Answers every command StorageEdgeWflwFwdRcvSystem filed this tick on the reply
 *              frame: promote_requested, the live workflow verdict, or the refusal reason. The
 *              reply code, the envelope width, the three status bytes and the verdict width are
 *              the hub rows of their owners, read on GLOBAL in tick; the verdict is built in
 *              place behind the envelope, bounded by the owner's width.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Reception
 * @design      DSGN_021
 * @usecase     usecase:100317 - relay operator command
 * @actor       Operator Command Dispatcher (OPERCOMDIS)
 * @created     2026-11-05
 * @modified    2026-11-05
 * @version     1.0.0
 *
 * CAUSAL CHAIN (filed operator command → EDGE_WFLW_RES reply)
 *
 *   [StorageEdgeWflwFwdRcvSystem filed StorageReqWflwFwdComponent + Prom / Stat / Rjct]
 *          │
 *          ▼
 *   ┌───────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageEdgeWflwRplySystem (dist tier)           │
 *   │                                                               │
 *   │  PROM:  promote_requested with path and target label          │
 *   │  STAT:  STG_WFLW_RES / STG_WFLW_STAGE under the path owner    │
 *   │  RJCT:  the refusal reason, bad-request status                │
 *   │  SEND:  reply frame (114) onto the outbound queue             │
 *   │  RETIRE: every command entity after the pass                  │
 *   └───────────────────────────────────────────────────────────────┘
 *          │
 *          │ the Replica relays the reply to the ase-cli connection
 *          ▼
 *   [ase-cli prints the machine-parseable verdict line]
 *
 * HUB Pattern (Active - reads the reply contract and the workflow verdict)
 *
 * READS (from Hub):
 *   NET_CONST_BIN_MSG_EDGE_WFLW_RES          (GLOBAL) - frame code of the reply
 *   REP_CONST_EDGE_WFLW_RES_HDR              (GLOBAL) - reply envelope width in bytes
 *   REP_CONST_EDGE_WFLW_JSON_MAX             (GLOBAL) - largest verdict document in bytes
 *   REP_CONST_EDGE_WFLW_STATUS_OK            (GLOBAL) - reply status of a served command
 *   REP_CONST_EDGE_WFLW_STATUS_BAD_REQUEST   (GLOBAL) - reply status of a refused command
 *   REP_CONST_EDGE_WFLW_STATUS_NOT_FOUND     (GLOBAL) - reply status of a path without workflow state
 *   STG_WFLW_RES   (owner = hashed_string(path)) - workflow verdict (status)
 *   STG_WFLW_STAGE (owner = hashed_string(path)) - workflow stage ordinal (status)
 *
 * WRITES (to Hub):
 *   none - the reply leaves on the transport outbound queue
 *
 * FLYWEIGHT Pattern (outbound lane)
 *   The reply frame is pushed onto the L1 transport OutboundQueueResourceManager (via ctx).
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
#include <ase/storage/systems/workflow/storage_edge_wflw_rply_sys.hpp>
// The filed command and the three marks of what it is
#include <ase/storage/components/request/storage_req_wflw_fwd_comp.hpp>
#include <ase/storage/components/tag/storage_wflw_prom_tag.hpp>
#include <ase/storage/components/tag/storage_wflw_stat_tag.hpp>
#include <ase/storage/components/tag/storage_wflw_rjct_tag.hpp>
// Hub API ONLY (header isolation): the reply contract and the workflow verdict go through api.hpp
#include <ase/hub/api.hpp>
// Types SSOT (is_not_found / is_in_rng sentinel + range checks on hub::get reads)
#include <ase/types/types.hpp>
// Transport lanes (L1 via ctx - the outbound queue the Replica link drains)
#include <ase/transport/outbound_queue_resource_manager.hpp>
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

// Append the WFLW_RES_* / WFLW_STAGE_* name to a bounded C-string (bounded lookup tables; the
// ordinals' SSOT is ase-storage types.hpp). This reply is the only place the names are spoken.
// Index range-checked via the types SSOT; out-of-range collapses to the safe floor. Data-driven
// lookup, not an if-chain of hardcoded ordinals.
void wflw_res_name(char* out, uint32_t max, int32_t i) {
    const char* names[6] = {"pending", "applied", "denied_edge",
                            "denied_gate", "denied_perm", "not_found"};
    if (!ase::types::is_in_rng_i32(i, 0, 5)) i = 0;
    ase::utils::str_append(out, max, names[i]);
}

void wflw_stage_name(char* out, uint32_t max, int32_t i) {
    const char* names[5] = {"draft", "review", "approved", "released", "retired"};
    if (!ase::types::is_in_rng_i32(i, 0, 4)) i = 0;
    ase::utils::str_append(out, max, names[i]);
}

// The document a verdict is replaced by when it does not fit the owner's width. The width
// itself is bounded below by this document, so the replacement always fits.
const char* too_wide_verdict() { return "{\"error\":\"verdict_exceeds_width\"}"; }

// True when the verdict was cut at the owner's width: it would reach ase-cli as a document
// that does not parse. Pure byte check.
bool verdict_cut(const char* json, uint32_t json_max) {
    const uint32_t jl = ase::utils::str_len(json, json_max);
    return jl == 0u || json[jl - 1u] != '}';
}

// Write [code][cli_conn:u32][status:u8][json_len:u32] in front of the verdict that stands at
// frame + res_hdr and return the frame length. Pure byte math.
uint32_t seal_reply(char* frame, uint8_t code, uint32_t cli_conn, uint8_t status, uint32_t res_hdr,
                    uint32_t json_max) {
    const uint32_t jl = ase::utils::str_len(frame + res_hdr, json_max);
    frame[0] = static_cast<char>(code);
    std::memcpy(frame + sizeof(uint8_t), &cli_conn, sizeof(uint32_t));
    frame[sizeof(uint8_t) + sizeof(uint32_t)] = static_cast<char>(status);
    std::memcpy(frame + sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint8_t), &jl, sizeof(uint32_t));
    return res_hdr + jl;
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageEdgeWflwRplySystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageEdgeWflwRplySystem] Started");
}

void StorageEdgeWflwRplySystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    auto pending = registry.view<StorageReqWflwFwdComponent>();
    if (pending.size() == 0u) return;

    // Without the outbound queue or the reply contract no command can be answered; the
    // commands leave with the report, because no later pass answers them either.
    auto* outq = registry.ctx().find<transport::OutboundQueueResourceManager>();
    bool ready = outq != nullptr;
    if (!ready) {
        log::error(log::ERR::CAT::RESOURCE_UNAVAIL, "StorageEdgeWflwRplySystem",
                   static_cast<uint32_t>(pending.size()), "EDGE_WFLW_outbound_queue");
    }

    const float byte_max = static_cast<float>(UINT8_MAX);
    const float lane = static_cast<float>(transport::LANE_BUF_SZ);
    // The reply envelope carries code, cli_conn, status and json_len.
    const float fields = static_cast<float>(sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint8_t) +
                                            sizeof(uint32_t));
    const float json_min =
        static_cast<float>(ase::utils::str_len(too_wide_verdict(), transport::LANE_BUF_SZ) + 1u);

    const float code_f = hub::get(registry, hub::GLOBAL, "NET_CONST_BIN_MSG_EDGE_WFLW_RES"_hs);
    if (ase::types::is_not_found(code_f)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageEdgeWflwRplySystem", "NET_CONST_BIN_MSG_EDGE_WFLW_RES");
        ready = false;
    }
    if (ready && !ase::types::is_in_rng_f(code_f, 0.0f, byte_max)) {
        log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "StorageEdgeWflwRplySystem", hub::GLOBAL,
                  "NET_CONST_BIN_MSG_EDGE_WFLW_RES", code_f, 0.0f, byte_max);
        ready = false;
    }
    const float hdr_f = hub::get(registry, hub::GLOBAL, "REP_CONST_EDGE_WFLW_RES_HDR"_hs);
    if (ase::types::is_not_found(hdr_f)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageEdgeWflwRplySystem", "REP_CONST_EDGE_WFLW_RES_HDR");
        ready = false;
    }
    if (ready && !ase::types::is_in_rng_f(hdr_f, fields, lane - json_min)) {
        log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "StorageEdgeWflwRplySystem", hub::GLOBAL,
                  "REP_CONST_EDGE_WFLW_RES_HDR", hdr_f, fields, lane - json_min);
        ready = false;
    }
    const float json_max_f = hub::get(registry, hub::GLOBAL, "REP_CONST_EDGE_WFLW_JSON_MAX"_hs);
    if (ase::types::is_not_found(json_max_f)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageEdgeWflwRplySystem", "REP_CONST_EDGE_WFLW_JSON_MAX");
        ready = false;
    }
    if (ready && !ase::types::is_in_rng_f(json_max_f, json_min, lane - hdr_f)) {
        log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "StorageEdgeWflwRplySystem", hub::GLOBAL,
                  "REP_CONST_EDGE_WFLW_JSON_MAX", json_max_f, json_min, lane - hdr_f);
        ready = false;
    }
    const float ok_f = hub::get(registry, hub::GLOBAL, "REP_CONST_EDGE_WFLW_STATUS_OK"_hs);
    if (ase::types::is_not_found(ok_f)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageEdgeWflwRplySystem", "REP_CONST_EDGE_WFLW_STATUS_OK");
        ready = false;
    }
    if (ready && !ase::types::is_in_rng_f(ok_f, 0.0f, byte_max)) {
        log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "StorageEdgeWflwRplySystem", hub::GLOBAL,
                  "REP_CONST_EDGE_WFLW_STATUS_OK", ok_f, 0.0f, byte_max);
        ready = false;
    }
    const float bad_f = hub::get(registry, hub::GLOBAL, "REP_CONST_EDGE_WFLW_STATUS_BAD_REQUEST"_hs);
    if (ase::types::is_not_found(bad_f)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageEdgeWflwRplySystem", "REP_CONST_EDGE_WFLW_STATUS_BAD_REQUEST");
        ready = false;
    }
    if (ready && !ase::types::is_in_rng_f(bad_f, 0.0f, byte_max)) {
        log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "StorageEdgeWflwRplySystem", hub::GLOBAL,
                  "REP_CONST_EDGE_WFLW_STATUS_BAD_REQUEST", bad_f, 0.0f, byte_max);
        ready = false;
    }
    const float nf_f = hub::get(registry, hub::GLOBAL, "REP_CONST_EDGE_WFLW_STATUS_NOT_FOUND"_hs);
    if (ase::types::is_not_found(nf_f)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageEdgeWflwRplySystem", "REP_CONST_EDGE_WFLW_STATUS_NOT_FOUND");
        ready = false;
    }
    if (ready && !ase::types::is_in_rng_f(nf_f, 0.0f, byte_max)) {
        log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "StorageEdgeWflwRplySystem", hub::GLOBAL,
                  "REP_CONST_EDGE_WFLW_STATUS_NOT_FOUND", nf_f, 0.0f, byte_max);
        ready = false;
    }
    if (!ready) {
        registry.destroy(pending.begin(), pending.end());
        return;
    }

    const uint8_t code = static_cast<uint8_t>(code_f);
    const uint32_t hdr = static_cast<uint32_t>(hdr_f);
    const uint32_t json_max = static_cast<uint32_t>(json_max_f);
    const uint8_t st_ok = static_cast<uint8_t>(ok_f);

    // The verdict is built in place behind the reply envelope, bounded by the owner's width.
    char frame[transport::LANE_BUF_SZ] = {};
    char* json = frame + hdr;

    for (auto [ent, fwd] : registry.view<StorageReqWflwFwdComponent, StorageWflwPromTag>().each()) {
        ase::utils::str_copy(json, json_max, "{\"status\":\"promote_requested\",\"path\":\"");
        ase::utils::str_append_json_safe(json, json_max, fwd.path);
        ase::utils::str_append(json, json_max, "\",\"target_label\":\"");
        ase::utils::str_append_json_safe(json, json_max, fwd.target_label);
        ase::utils::str_append(json, json_max, "\"}");
        if (verdict_cut(json, json_max)) {
            log::warn(log::WRN::CAT::INPUT_TRUNCATED, "StorageEdgeWflwRplySystem", static_cast<uint32_t>(ent),
                      "REP_CONST_EDGE_WFLW_JSON_MAX", "the promote verdict exceeds the owner's width");
            ase::utils::str_copy(json, json_max, too_wide_verdict());
        }
        if (!outq->push_outbound(frame, seal_reply(frame, code, fwd.cli_conn, st_ok, hdr, json_max))) {
            log::error(log::ERR::CAT::CAPACITY_REACHED, "StorageEdgeWflwRplySystem", fwd.cli_conn,
                       "EDGE_WFLW_outbound_capacity");
        }
    }

    for (auto [ent, fwd] : registry.view<StorageReqWflwFwdComponent, StorageWflwStatTag>().each()) {
        // The live owner-scoped verdict the workflow systems published under the path.
        const uint32_t owner = entt::hashed_string(fwd.path).value();
        const float res_f = hub::get(registry, owner, "STG_WFLW_RES"_hs, hub::NOT_FOUND);
        const bool known = !ase::types::is_not_found(res_f);
        if (!known) {
            ase::utils::str_copy(json, json_max, "{\"error\":\"no_workflow_state\",\"path\":\"");
            ase::utils::str_append_json_safe(json, json_max, fwd.path);
            ase::utils::str_append(json, json_max, "\"}");
        }
        if (known) {
            int32_t res_i = static_cast<int32_t>(res_f);
            if (!ase::types::is_in_rng_i32(res_i, 0, 5)) res_i = 0;
            ase::utils::str_copy(json, json_max, "{\"path\":\"");
            ase::utils::str_append_json_safe(json, json_max, fwd.path);
            ase::utils::str_append(json, json_max, "\",\"result\":");
            append_num(json, json_max, static_cast<uint32_t>(res_i));
            ase::utils::str_append(json, json_max, ",\"result_name\":\"");
            wflw_res_name(json, json_max, res_i);
            ase::utils::str_append(json, json_max, "\"");
            const float stage_f = hub::get(registry, owner, "STG_WFLW_STAGE"_hs, hub::NOT_FOUND);
            if (!ase::types::is_not_found(stage_f)) {
                int32_t stage_i = static_cast<int32_t>(stage_f);
                if (!ase::types::is_in_rng_i32(stage_i, 0, 4)) stage_i = 0;
                ase::utils::str_append(json, json_max, ",\"stage\":");
                append_num(json, json_max, static_cast<uint32_t>(stage_i));
                ase::utils::str_append(json, json_max, ",\"stage_name\":\"");
                wflw_stage_name(json, json_max, stage_i);
                ase::utils::str_append(json, json_max, "\"");
            }
            ase::utils::str_append(json, json_max, "}");
        }
        if (verdict_cut(json, json_max)) {
            log::warn(log::WRN::CAT::INPUT_TRUNCATED, "StorageEdgeWflwRplySystem", static_cast<uint32_t>(ent),
                      "REP_CONST_EDGE_WFLW_JSON_MAX", "the status verdict exceeds the owner's width");
            ase::utils::str_copy(json, json_max, too_wide_verdict());
        }
        const uint8_t status = known ? st_ok : static_cast<uint8_t>(nf_f);
        if (!outq->push_outbound(frame, seal_reply(frame, code, fwd.cli_conn, status, hdr, json_max))) {
            log::error(log::ERR::CAT::CAPACITY_REACHED, "StorageEdgeWflwRplySystem", fwd.cli_conn,
                       "EDGE_WFLW_outbound_capacity");
        }
    }

    for (auto [ent, fwd] : registry.view<StorageReqWflwFwdComponent, StorageWflwRjctTag>().each()) {
        ase::utils::str_copy(json, json_max, "{\"error\":\"");
        ase::utils::str_append_json_safe(json, json_max, fwd.reason);
        ase::utils::str_append(json, json_max, "\"}");
        if (verdict_cut(json, json_max)) {
            log::warn(log::WRN::CAT::INPUT_TRUNCATED, "StorageEdgeWflwRplySystem", static_cast<uint32_t>(ent),
                      "REP_CONST_EDGE_WFLW_JSON_MAX", "the refusal exceeds the owner's width");
            ase::utils::str_copy(json, json_max, too_wide_verdict());
        }
        if (!outq->push_outbound(frame, seal_reply(frame, code, fwd.cli_conn, static_cast<uint8_t>(bad_f),
                                                   hdr, json_max))) {
            log::error(log::ERR::CAT::CAPACITY_REACHED, "StorageEdgeWflwRplySystem", fwd.cli_conn,
                       "EDGE_WFLW_outbound_capacity");
        }
    }

    // Deferred deletion after every pass: each command was answered exactly once.
    registry.destroy(pending.begin(), pending.end());
}

void StorageEdgeWflwRplySystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageEdgeWflwRplySystem] Stopped");
}

}  // namespace ase::storage
