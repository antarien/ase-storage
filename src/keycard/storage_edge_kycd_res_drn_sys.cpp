/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_edge_kycd_res_drn_sys.cpp
 * @brief       StorageEdgeKycdResDrnSystem - Parses Replica keycard documents into the gate session
 * @description Drains the keycard answer frames of the Replica from LANE_KYC, checks the envelope
 *              against the owners' widths and publishes clearance, permission and the four codeword
 *              hold verdicts on the owner of the recovered user_id. Frame code, envelope width,
 *              payload width and the OK status are the hub rows of their owners, read on GLOBAL in
 *              tick; the lane route is registered there with the read code.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Reception
 * @design      DSGN_021
 * @usecase     usecase:100049 - publish a hub value
 * @actor       Protocol Channel Steward (PROTCHASTE)
 * @created     2026-06-24
 * @modified    2026-11-05
 * @version     1.1.0
 *
 * CAUSAL CHAIN (EDGE_KYCD_RES_DRN: Replica keycard document → gate session)
 *
 *   [Replica ReplicaEdgeKycdSystem ships frame 34 over Binary WS]
 *          │
 *          │ L2 KernelWbskDspcSystem routes the code → LANE_KYC
 *          ▼
 *   ┌───────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageEdgeKycdResDrnSystem (dist tier)         │
 *   │                                                               │
 *   │  ROUTE:   register the hub frame code onto LANE_KYC           │
 *   │  POP:     drain LANE_KYC frames                               │
 *   │  PARSE:   STATUS_OK → scan the keycard document (user_id,     │
 *   │           clearance, permission, revoked, codewords[])        │
 *   │  PUBLISH: owner = hashed_string(user_id):                     │
 *   │           SES_CLEARANCE, SES_KYCD_PERM,                       │
 *   │           SES_KYCD_HOLDS_<cw> A/ACS hold verdicts (no string) │
 *   └───────────────────────────────────────────────────────────────┘
 *          │
 *          │ next install.sh poll/retry: the edge A/ACS gate reads the
 *          │ published session and authorizes the download
 *          ▼
 *   [ase-pl-edge-webserver acl_gate enforces clearance + codeword + permission]
 *
 * HUB Pattern (Active - reads the frame contract, publishes the gate session)
 *
 * READS (from Hub):
 *   REP_CONST_BIN_MSG_EDGE_KYCD_RES      (GLOBAL) - frame code of the keycard answer
 *   NET_CONST_RSN_MEM_READ_RES_PREFIX_SZ (GLOBAL) - envelope width in bytes
 *   NET_CONST_RSN_MEM_READ_PAYLOAD_MAX   (GLOBAL) - largest keycard document in bytes
 *   NET_CONST_RSN_MEM_STATUS_OK          (GLOBAL) - status byte of a found keycard
 *
 * WRITES (to Hub):
 *   SES_CLEARANCE         - keycard clearance, owner = hashed_string(user_id)
 *   SES_KYCD_PERM         - keycard permission bitflags, same owner
 *   SES_KYCD_HOLDS_<cw>   - owner-scoped A/ACS hold verdict (1.0 iff the recovered
 *                           keycard holds the exact edge codeword; exact-string compare
 *                           server-internal, the codeword STRING never re-enters the Hub)
 *
 * FLYWEIGHT Pattern (inbound lane)
 *   The keycard document never enters a Component. It is popped from the L1
 *   transport InboundQueueResourceManager (LANE_KYC, via ctx) into a bounded
 *   stack buffer, scanned, and projected onto owner-keyed Hub values. The dist
 *   host links NO data client — it publishes only what the Replica returned.
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
#include <ase/storage/systems/keycard/storage_edge_kycd_res_drn_sys.hpp>
// Module constants (codeword hashes, codeword and owner buffers)
#include <ase/storage/types.hpp>
// Bounded field extraction out of the JSON payload (module-internal, pure byte scans)
#include <ase/storage/storage_kycd_payload.hpp>
// Hub API for the frame contract on GLOBAL and the owner-keyed session publication
#include <ase/hub/api.hpp>
// Types (L0 - sentinel and range predicates)
#include <ase/types/types.hpp>
// Utils (L0 — safe C-string operations)
#include <ase/utils/strops.hpp>
// Transport inbound lane (L1 via ctx — the L2 demux fills LANE_KYC; this module
// never touches the socket). L3 to L1 is allowed.
#include <ase/transport/inbound_queue_resource_manager.hpp>
#include <ase/transport/types.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>
#include <cstring>

#include <entt/core/hashed_string.hpp>

namespace ase::storage {
using namespace entt::literals;  // For "_hs hashed strings (Hub)

// Anonymous namespace for helper FUNCTIONS (NOT static!)
// NO STRUCTS HERE! NO View/Query operations in helpers! Only pure byte/char math!
// The payload scans of this system live in storage_kycd_payload.hpp: pulling fields out of a
// JSON document is a concern of its own beside draining a lane and publishing a session.
namespace {

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageEdgeKycdResDrnSystem::on_start(ecs::Registry& registry) {
    log::debug("[StorageEdgeKycdResDrnSystem] Started");

    // The shared inbound lane is emplaced here when the L2 demux on_start has not run yet
    // (order-independent: whoever runs first creates it). The route itself needs the frame
    // code, a hub row that stands only from the Initialization tick on, so tick registers it.
    if (registry.ctx().find<transport::InboundQueueResourceManager>() == nullptr) {
        registry.ctx().emplace<transport::InboundQueueResourceManager>();
    }
}

void StorageEdgeKycdResDrnSystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    auto* queue = registry.ctx().find<transport::InboundQueueResourceManager>();
    if (queue == nullptr) return;

    const float code_f = hub::get(registry, hub::GLOBAL, "REP_CONST_BIN_MSG_EDGE_KYCD_RES"_hs);
    if (ase::types::is_not_found(code_f)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageEdgeKycdResDrnSystem",
                   "REP_CONST_BIN_MSG_EDGE_KYCD_RES");
        return;
    }
    if (!ase::types::is_in_rng_f(code_f, 0.0f, static_cast<float>(UINT8_MAX))) {
        log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "StorageEdgeKycdResDrnSystem", hub::GLOBAL,
                  "REP_CONST_BIN_MSG_EDGE_KYCD_RES", code_f, 0.0f, static_cast<float>(UINT8_MAX));
        return;
    }
    const uint8_t code = static_cast<uint8_t>(code_f);
    // Writing the same route again is a map assignment and changes nothing once it stands.
    queue->register_route(code, transport::LANE_KYC);
    if (!queue->has_inbound(transport::LANE_KYC)) return;

    // The envelope carries code, req_id, status and payload_len; a stated width below their sum
    // or above the lane cannot frame an answer.
    const float fields = static_cast<float>(sizeof(uint8_t) + sizeof(uint64_t) + sizeof(uint8_t) +
                                            sizeof(uint32_t));
    const float hdr_f = hub::get(registry, hub::GLOBAL, "NET_CONST_RSN_MEM_READ_RES_PREFIX_SZ"_hs);
    if (ase::types::is_not_found(hdr_f)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageEdgeKycdResDrnSystem",
                   "NET_CONST_RSN_MEM_READ_RES_PREFIX_SZ");
        return;
    }
    if (!ase::types::is_in_rng_f(hdr_f, fields, static_cast<float>(transport::LANE_BUF_SZ))) {
        log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "StorageEdgeKycdResDrnSystem", hub::GLOBAL,
                  "NET_CONST_RSN_MEM_READ_RES_PREFIX_SZ", hdr_f, fields,
                  static_cast<float>(transport::LANE_BUF_SZ));
        return;
    }
    const uint32_t hdr = static_cast<uint32_t>(hdr_f);

    const float payload_max_f = hub::get(registry, hub::GLOBAL, "NET_CONST_RSN_MEM_READ_PAYLOAD_MAX"_hs);
    if (ase::types::is_not_found(payload_max_f)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageEdgeKycdResDrnSystem",
                   "NET_CONST_RSN_MEM_READ_PAYLOAD_MAX");
        return;
    }
    if (!ase::types::is_in_rng_f(payload_max_f, 1.0f, static_cast<float>(transport::LANE_BUF_SZ - hdr))) {
        log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "StorageEdgeKycdResDrnSystem", hub::GLOBAL,
                  "NET_CONST_RSN_MEM_READ_PAYLOAD_MAX", payload_max_f, 1.0f,
                  static_cast<float>(transport::LANE_BUF_SZ - hdr));
        return;
    }
    const uint32_t payload_max = static_cast<uint32_t>(payload_max_f);

    const float status_ok_f = hub::get(registry, hub::GLOBAL, "NET_CONST_RSN_MEM_STATUS_OK"_hs);
    if (ase::types::is_not_found(status_ok_f)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageEdgeKycdResDrnSystem",
                   "NET_CONST_RSN_MEM_STATUS_OK");
        return;
    }
    if (!ase::types::is_in_rng_f(status_ok_f, 0.0f, static_cast<float>(UINT8_MAX))) {
        log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "StorageEdgeKycdResDrnSystem", hub::GLOBAL,
                  "NET_CONST_RSN_MEM_STATUS_OK", status_ok_f, 0.0f, static_cast<float>(UINT8_MAX));
        return;
    }
    const uint8_t status_ok = static_cast<uint8_t>(status_ok_f);

    char buf[transport::LANE_BUF_SZ] = {};
    uint32_t msg_len = 0;

    while (queue->pop_inbound(transport::LANE_KYC, buf, transport::LANE_BUF_SZ, msg_len)) {
        // THE FRAME CHECKS CARRY INPUT_REJECTED. Every one of them discards the frame
        // (`msg_len = 0; continue;`): nothing is clamped and nothing falls back to a default, so
        // the three correcting categories (VALUE_OUT_OF_RANGE, VALUE_NEGATIVE, VALUE_INVALID)
        // would state a repair that never happens. INPUT_REJECTED says what does happen: the
        // caller sent something unserviceable and the engine works correctly. The form without
        // `owner` fits, because the trigger is a foreign caller and not a hub owner. The value_id
        // names WHICH promise of the contract broke; the raw numbers have no categorized
        // overload without an owner.
        if (msg_len < hdr) {
            log::warn(log::WRN::CAT::INPUT_REJECTED, "StorageEdgeKycdResDrnSystem",
                      "EDGE_KYCD_RES_frame_len");
            msg_len = 0;
            continue;
        }

        const uint8_t msg_type = static_cast<uint8_t>(buf[0]);
        if (msg_type != code) {
            log::warn(log::WRN::CAT::INPUT_REJECTED, "StorageEdgeKycdResDrnSystem",
                      "EDGE_KYCD_RES_msg_type");
            msg_len = 0;
            continue;
        }

        uint64_t req_id = 0;
        std::memcpy(&req_id, buf + sizeof(uint8_t), sizeof(uint64_t));
        const uint8_t status = static_cast<uint8_t>(buf[sizeof(uint8_t) + sizeof(uint64_t)]);
        uint32_t payload_len = 0;
        std::memcpy(&payload_len, buf + sizeof(uint8_t) + sizeof(uint64_t) + sizeof(uint8_t),
                    sizeof(uint32_t));

        // A NOT_FOUND / ERROR response publishes NOTHING: the gate stays 401 so a
        // missing or backend-failed keycard never silently grants a download. Each
        // guard is a single comparison (bounds checks, not a value range test).
        if (status != status_ok) {
            log::info("[StorageEdgeKycdResDrnSystem] EDGE_KYCD_RES status={} req_id={} — no session published",
                      static_cast<uint32_t>(status), req_id);
            msg_len = 0;
            continue;
        }
        if (payload_len < 1u) {
            log::warn(log::WRN::CAT::INPUT_REJECTED, "StorageEdgeKycdResDrnSystem",
                      "EDGE_KYCD_RES_payload_len");
            msg_len = 0;
            continue;
        }
        if (hdr + payload_len > msg_len) {
            log::warn(log::WRN::CAT::INPUT_REJECTED, "StorageEdgeKycdResDrnSystem",
                      "EDGE_KYCD_RES_payload_end");
            msg_len = 0;
            continue;
        }
        // A document above the owner's width is dropped whole: a cut JSON document could lose
        // the revoked flag or a codeword and publish a grant the Replica never stated.
        if (payload_len > payload_max) {
            log::warn(log::WRN::CAT::INPUT_REJECTED, "StorageEdgeKycdResDrnSystem",
                      "EDGE_KYCD_RES_payload_max");
            msg_len = 0;
            continue;
        }

        const char* doc = buf + hdr;

        // The Replica FIND returns a JSON array '[{...}]'; the keycard document is
        // the first (single) object. All field lookups scan the whole payload, so
        // the array brackets need no explicit unwrap — the keyed scan finds each
        // field wherever it sits.

        // Recover the recipient user_id — the owner the gate session is keyed on.
        // Without it there is no owner to publish against (no silent default).
        char user_id[MAX_OWNER_ID] = {};
        if (!parse_str_field(doc, payload_len, "\"user_id\"", user_id, sizeof(user_id))) {
            // THE THREE DOCUMENT FIELDS LOG ACCESS_DENIED AT ERROR LEVEL. A missing user_id,
            // clearance or permission keeps the gate at 401 or on the denying floor, which is a
            // security event; ACCESS_DENIED is an error category whose help text places the
            // decision in the error stream. req_id stays out of the line: it is a uint64_t and
            // the categorized forms carry a uint32_t owner, so a cut 64-bit identifier would be a
            // different identifier. Without owner here, because user_id is exactly what is
            // missing: the line states truthfully that the identity could not be resolved.
            log::error(log::ERR::CAT::ACCESS_DENIED, "StorageEdgeKycdResDrnSystem",
                       "EDGE_KYCD_RES_user_id");
            msg_len = 0;
            continue;
        }
        const uint32_t owner = entt::hashed_string{user_id}.value();

        // A revoked keycard never grants: publish NOTHING so the gate stays 401.
        uint32_t revoked = 0;
        if (parse_num_field(doc, payload_len, "\"revoked\"", revoked) && revoked != 0u) {
            log::info("[StorageEdgeKycdResDrnSystem] keycard revoked owner={} user='{}' — no session published",
                      owner, user_id);
            msg_len = 0;
            continue;
        }

        // Clearance + permission axes (gate steps 4 + 6). A missing axis is corrupt
        // state — log it and default to the denying floor so the gate refuses
        // rather than grants on partial data. From here on the identity is resolved,
        // and it is the axis the category asks for, so the line carries the owner.
        uint32_t clearance = 0;
        if (!parse_num_field(doc, payload_len, "\"clearance\"", clearance)) {
            log::error(log::ERR::CAT::ACCESS_DENIED, "StorageEdgeKycdResDrnSystem", owner,
                       "EDGE_KYCD_RES_clearance");
            clearance = 0;
        }
        uint32_t permission = 0;
        if (!parse_num_field(doc, payload_len, "\"permission\"", permission)) {
            log::error(log::ERR::CAT::ACCESS_DENIED, "StorageEdgeKycdResDrnSystem", owner,
                       "EDGE_KYCD_RES_permission");
            permission = 0;
        }

        hub::set(registry, owner, "SES_CLEARANCE"_hs, static_cast<float>(clearance));
        hub::set(registry, owner, "SES_KYCD_PERM"_hs, static_cast<float>(permission));

        // Reset the owner-scoped edge A/ACS hold-verdicts before the pass so a revoked keycard
        // re-resolution cannot leave a stale grant (the gate reads only these booleans).
        hub::set(registry, owner, "SES_KYCD_HOLDS_BINARY"_hs, 0.0f);
        hub::set(registry, owner, "SES_KYCD_HOLDS_SIG"_hs, 0.0f);
        hub::set(registry, owner, "SES_KYCD_HOLDS_SBOM"_hs, 0.0f);
        hub::set(registry, owner, "SES_KYCD_HOLDS_METADATA"_hs, 0.0f);

        // Codeword set (gate step 5): the '"codewords":[...]' array. Walk each quoted element
        // after the array open-bracket and compare it EXACTLY (server-internal) against the fixed
        // edge codewords, setting the matching owner-scoped hold-verdict boolean.
        uint32_t count = 0;
        const int32_t arr_at = find_token(doc, payload_len, "\"codewords\"");
        if (arr_at >= 0) {
            uint32_t i = static_cast<uint32_t>(arr_at) +
                         ase::utils::str_len("\"codewords\"", payload_len);
            // advance to the array open-bracket
            while (i < payload_len && doc[i] != '[' && doc[i] != ']') ++i;
            if (i < payload_len && doc[i] == '[') {
                ++i;
                while (i < payload_len && doc[i] != ']' && count < KYCD_DECODE_CWRD_MAX) {
                    if (doc[i] == '"') {
                        ++i;  // opening quote
                        char cw[MAX_CODEWORD_LEN] = {};
                        uint32_t o = 0;
                        while (i < payload_len && doc[i] != '"' && o + 1u < sizeof(cw)) {
                            cw[o++] = doc[i++];
                        }
                        cw[o] = '\0';
                        if (i < payload_len && doc[i] == '"') ++i;  // closing quote
                        if (o > 0u) {
                            // Hashed at the point the codeword is parsed out of the payload:
                            // that is where the string comes into being, so it is also where
                            // its identity is fixed. The codeword itself never re-enters the
                            // Hub; only the fixed hold-verdict booleans do.
                            const uint32_t cwrd_hash = entt::hashed_string::value(cw, o);

                            // Compare the recovered codeword against the fixed
                            // edge-distribution codewords and, on a match, set the matching
                            // owner-scoped A/ACS hold-verdict boolean. The codeword itself
                            // never re-enters the Hub (server-internal, A/ACS step 5): only
                            // these fixed, contract-registered booleans reach the L4 edge gate.
                            //
                            // The comparison is on HASHES, against the compile-time constants
                            // in types.hpp. Identity is a lookup, and a lookup compares hashes,
                            // never characters (WRFL_ASE_STRING_HANDLING Section 3).
                            //
                            // FOUR INDEPENDENT ifs, not an else-chain: a codeword set is a SET,
                            // the four axes are orthogonal grants, and an else would make them
                            // mutually exclusive - a different access policy than this gate has.
                            if (cwrd_hash == EDGE_CWRD_BINARY_HASH) {
                                hub::set(registry, owner, "SES_KYCD_HOLDS_BINARY"_hs, 1.0f);
                            }
                            if (cwrd_hash == EDGE_CWRD_SIG_HASH) {
                                hub::set(registry, owner, "SES_KYCD_HOLDS_SIG"_hs, 1.0f);
                            }
                            if (cwrd_hash == EDGE_CWRD_SBOM_HASH) {
                                hub::set(registry, owner, "SES_KYCD_HOLDS_SBOM"_hs, 1.0f);
                            }
                            if (cwrd_hash == EDGE_CWRD_METADATA_HASH) {
                                hub::set(registry, owner, "SES_KYCD_HOLDS_METADATA"_hs, 1.0f);
                            }
                            ++count;
                        }
                    } else {
                        ++i;  // commas / whitespace between elements
                    }
                }
            }
        }

        log::info("[StorageEdgeKycdResDrnSystem] session published owner={} user='{}' clrn={} perm={} cwrds={} (req_id={})",
                  owner, user_id, clearance, permission, count, req_id);

        msg_len = 0;
    }
}

void StorageEdgeKycdResDrnSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageEdgeKycdResDrnSystem] Stopped");
}

}  // namespace ase::storage
