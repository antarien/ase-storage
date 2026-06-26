/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_edge_kycd_res_drn_sys.cpp
 * @brief       StorageEdgeKycdResDrnSystem - Parses Replica keycard documents into the gate session
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Reception
 * @created     2026-06-24
 * @modified    2026-06-24
 * @version     1.0.0
 *
 * CAUSAL CHAIN (EDGE_KYCD_RES_DRN: Replica keycard document → gate session)
 *
 *   [Replica ReplicaEdgeKycdSystem ships BIN_MSG_EDGE_KYCD_RES over Binary WS]
 *          │
 *          │ L2 KernelWbskDspcSystem routes id 34 → LANE_KYC
 *          ▼
 *   ┌───────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageEdgeKycdResDrnSystem (dist tier)         │
 *   │                                                               │
 *   │  POP:     drain LANE_KYC frames                               │
 *   │  PARSE:   STATUS_OK → scan the keycard document (user_id,     │
 *   │           clearance, permission, revoked, codewords[])        │
 *   │  PUBLISH: owner = hashed_string(user_id):                     │
 *   │           SES_CLEARANCE, SES_KYCD_PERM, SES_KYCD_CWRD_COUNT,  │
 *   │           SES_KYCD_CWRD_<owner>_<i> (exact gate projection)   │
 *   └───────────────────────────────────────────────────────────────┘
 *          │
 *          │ next install.sh poll/retry: the edge A/ACS gate reads the
 *          │ published session and authorizes the download
 *          ▼
 *   [ase-pl-edge-webserver acl_gate enforces clearance + codeword + permission]
 *
 * HUB Pattern (Active - publishes the gate session)
 *
 * READS (from Hub):
 *   (none)
 *
 * WRITES (to Hub):
 *   SES_CLEARANCE         - keycard clearance, owner = hashed_string(user_id)
 *   SES_KYCD_PERM         - keycard permission bitflags, same owner
 *   SES_KYCD_CWRD_COUNT   - number of recovered codewords
 *   SES_KYCD_CWRD_<owner>_<i> - per-index codeword debug-labels (i in [0, count))
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
#include <ase/storage/systems/keycard/storage_edge_kycd_res_drn_sys.hpp>
// Module constants (EDGE_KYCD_* frame layout, codeword key buffers)
#include <ase/storage/types.hpp>
// Hub API for owner-keyed session publication
#include <ase/hub/api.hpp>
// Utils (L0 — safe C-string operations)
#include <ase/utils/strops.hpp>
// Transport inbound lane (L1 via ctx — the L2 demux fills LANE_KYC; this module
// NEVER touches the socket). L3→L1 erlaubt.
#include <ase/transport/inbound_queue_resource_manager.hpp>
#include <ase/transport/types.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>
#include <cstring>

#include <entt/core/hashed_string.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
// NO STRUCTS HERE! NO View/Query operations in helpers! Only pure byte/char math!
namespace {

// Append the decimal representation of value into dst (bounded by dst_size).
// Pure math — no views, no components. The do-while always emits at least one
// digit. Mirrors StorageKycdCwrdPubSystem::append_decimal so the rebuilt
// "SES_KYCD_CWRD_<owner>_<i>" key matches the gate consumer exactly.
void append_decimal(char* dst, uint32_t dst_size, uint32_t value) {
    char tmp[11] = {};  // uint32_t max = 4294967295 → 10 digits + null
    uint32_t di = 0;
    do {
        tmp[di++] = static_cast<char>('0' + (value % DECIMAL_RADIX));
        value /= DECIMAL_RADIX;
    } while (value > 0 && di < 10);
    char fwd[11] = {};
    for (uint32_t fi = 0; fi < di; ++fi) {
        fwd[fi] = tmp[di - 1 - fi];
    }
    fwd[di] = '\0';
    ase::utils::str_append(dst, dst_size, fwd);
}

// Build the per-owner indexed codeword key "SES_KYCD_CWRD_<owner>_<index>",
// byte-identical to StorageKycdCwrdPubSystem::build_cwrd_key so the edge A/ACS
// gate reads the recovered codeword set unchanged.
void build_cwrd_key(char* key, uint32_t key_size, uint32_t owner, uint32_t index) {
    ase::utils::str_copy(key, key_size, "SES_KYCD_CWRD_");
    append_decimal(key, key_size, owner);
    ase::utils::str_append(key, key_size, "_");
    append_decimal(key, key_size, index);
}

// Find the first occurrence of needle in [doc, doc+len). Returns the index of the
// match start, or -1 when absent. Pure byte scan (no std::string, no strstr).
int32_t find_token(const char* doc, uint32_t len, const char* needle) {
    uint32_t nlen = ase::utils::str_len(needle, EDGE_KYCD_PAYLOAD_MAX);
    if (nlen == 0u || nlen > len) return -1;
    for (uint32_t i = 0; i + nlen <= len; ++i) {
        uint32_t j = 0;
        while (j < nlen && doc[i + j] == needle[j]) ++j;
        if (j == nlen) return static_cast<int32_t>(i);
    }
    return -1;
}

// Parse the unsigned integer value of a '"key":<digits>' field into out_val.
// Returns true when the key + a digit run is found. Bounded scan; tolerates
// whitespace between the colon and the first digit.
bool parse_num_field(const char* doc, uint32_t len, const char* quoted_key,
                     uint32_t& out_val) {
    int32_t at = find_token(doc, len, quoted_key);
    if (at < 0) return false;
    uint32_t i = static_cast<uint32_t>(at) + ase::utils::str_len(quoted_key, EDGE_KYCD_PAYLOAD_MAX);
    while (i < len && (doc[i] == ' ' || doc[i] == ':')) ++i;
    bool any = false;
    uint32_t v = 0;
    while (i < len && doc[i] >= '0' && doc[i] <= '9') {
        v = v * 10u + static_cast<uint32_t>(doc[i] - '0');
        any = true;
        ++i;
    }
    if (!any) return false;
    out_val = v;
    return true;
}

// Copy the string value of a '"key":"<value>"' field into out (bounded). Returns
// true when the key + a quoted value is found. Bounded scan.
bool parse_str_field(const char* doc, uint32_t len, const char* quoted_key,
                     char* out, uint32_t out_size) {
    if (out_size == 0u) return false;
    out[0] = '\0';
    int32_t at = find_token(doc, len, quoted_key);
    if (at < 0) return false;
    uint32_t i = static_cast<uint32_t>(at) + ase::utils::str_len(quoted_key, EDGE_KYCD_PAYLOAD_MAX);
    while (i < len && (doc[i] == ' ' || doc[i] == ':')) ++i;
    if (i >= len || doc[i] != '"') return false;
    ++i;  // opening quote
    uint32_t o = 0;
    while (i < len && doc[i] != '"' && o + 1u < out_size) {
        out[o++] = doc[i++];
    }
    out[o] = '\0';
    return o > 0u;
}

// Publish one recovered codeword string at the per-index gate key. Pure Hub
// write keyed by the rebuilt owner-scoped label hash.
void publish_codeword(ecs::Registry& registry, uint32_t owner, uint32_t index,
                      const char* cwrd) {
    char key[KYCD_CWRD_KEY_BUF] = {};
    build_cwrd_key(key, sizeof(key), owner, index);
    hub::set_debug_label(registry, entt::hashed_string{key}.value(), cwrd);
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageEdgeKycdResDrnSystem::on_start(ecs::Registry& registry) {
    log::debug("[StorageEdgeKycdResDrn] Started");

    // Register the edge keycard-authz response wire-type onto the shared inbound
    // lane so the L2 demux routes it here. Emplace the queue if the demux on_start
    // has not run yet (order-independent: whoever runs first creates it).
    if (registry.ctx().find<transport::InboundQueueResourceManager>() == nullptr) {
        registry.ctx().emplace<transport::InboundQueueResourceManager>();
    }
    auto& queue = registry.ctx().get<transport::InboundQueueResourceManager>();
    queue.register_route(EDGE_KYCD_BIN_MSG_RES, transport::LANE_KYC);
}

void StorageEdgeKycdResDrnSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto* queue = registry.ctx().find<transport::InboundQueueResourceManager>();
    if (queue == nullptr || !queue->has_inbound(transport::LANE_KYC)) return;

    char buf[transport::LANE_BUF_SZ] = {};
    uint32_t msg_len = 0;

    while (queue->pop_inbound(transport::LANE_KYC, buf, transport::LANE_BUF_SZ, msg_len)) {
        if (msg_len < EDGE_KYCD_RES_HDR) {
            log::error("[StorageEdgeKycdResDrn] EDGE_KYCD_RES frame too short: {} bytes", msg_len);
            msg_len = 0;
            continue;
        }

        uint8_t msg_type = static_cast<uint8_t>(buf[0]);
        if (msg_type != EDGE_KYCD_BIN_MSG_RES) {
            log::warn("[StorageEdgeKycdResDrn] unexpected inbound type={} ({} bytes) skipped",
                      static_cast<uint32_t>(msg_type), msg_len);
            msg_len = 0;
            continue;
        }

        uint64_t req_id = 0;
        std::memcpy(&req_id, buf + 1, 8);
        uint8_t status = static_cast<uint8_t>(buf[9]);
        uint32_t payload_len = 0;
        std::memcpy(&payload_len, buf + 10, 4);

        // A NOT_FOUND / ERROR response publishes NOTHING: the gate stays 401 so a
        // missing or backend-failed keycard never silently grants a download. Each
        // guard is a single comparison (bounds checks, not a value range test).
        if (status != EDGE_KYCD_STATUS_OK) {
            log::info("[StorageEdgeKycdResDrn] EDGE_KYCD_RES status={} req_id={} — no session published",
                      static_cast<uint32_t>(status), req_id);
            msg_len = 0;
            continue;
        }
        if (payload_len < 1u) {
            log::error("[StorageEdgeKycdResDrn] EDGE_KYCD_RES OK with empty payload (req_id={})", req_id);
            msg_len = 0;
            continue;
        }
        uint32_t payload_end = EDGE_KYCD_RES_HDR + payload_len;
        if (payload_end > msg_len) {
            log::error("[StorageEdgeKycdResDrn] EDGE_KYCD_RES truncated payload (req_id={} need={} have={})",
                       req_id, payload_end, msg_len);
            msg_len = 0;
            continue;
        }
        if (payload_len > EDGE_KYCD_PAYLOAD_MAX) {
            payload_len = EDGE_KYCD_PAYLOAD_MAX;
        }

        const char* doc = buf + EDGE_KYCD_RES_HDR;

        // The Replica FIND returns a JSON array '[{...}]'; the keycard document is
        // the first (single) object. All field lookups scan the whole payload, so
        // the array brackets need no explicit unwrap — the keyed scan finds each
        // field wherever it sits.

        // Recover the recipient user_id — the owner the gate session is keyed on.
        // Without it there is no owner to publish against (no silent default).
        char user_id[MAX_OWNER_ID] = {};
        if (!parse_str_field(doc, payload_len, "\"user_id\"", user_id, sizeof(user_id))) {
            log::error("[StorageEdgeKycdResDrn] EDGE_KYCD_RES document missing user_id (req_id={})", req_id);
            msg_len = 0;
            continue;
        }
        uint32_t owner = entt::hashed_string{user_id}.value();

        // A revoked keycard never grants: publish NOTHING so the gate stays 401.
        uint32_t revoked = 0;
        if (parse_num_field(doc, payload_len, "\"revoked\"", revoked) && revoked != 0u) {
            log::info("[StorageEdgeKycdResDrn] keycard revoked owner={} user='{}' — no session published",
                      owner, user_id);
            msg_len = 0;
            continue;
        }

        // Clearance + permission axes (gate steps 4 + 6). A missing axis is corrupt
        // state — log it and default to the denying floor so the gate refuses
        // rather than grants on partial data.
        uint32_t clearance = 0;
        if (!parse_num_field(doc, payload_len, "\"clearance\"", clearance)) {
            log::error("[StorageEdgeKycdResDrn] EDGE_KYCD_RES document missing clearance (req_id={})", req_id);
            clearance = 0;
        }
        uint32_t permission = 0;
        if (!parse_num_field(doc, payload_len, "\"permission\"", permission)) {
            log::error("[StorageEdgeKycdResDrn] EDGE_KYCD_RES document missing permission (req_id={})", req_id);
            permission = 0;
        }

        hub::set(registry, owner, "SES_CLEARANCE"_hs, static_cast<float>(clearance));
        hub::set(registry, owner, "SES_KYCD_PERM"_hs, static_cast<float>(permission));

        // Codeword set (gate step 5): the '"codewords":[...]' array. Walk each
        // quoted element after the array open-bracket, publishing one per-index
        // label, exactly the projection StorageKycdCwrdPubSystem emits locally.
        uint32_t count = 0;
        int32_t arr_at = find_token(doc, payload_len, "\"codewords\"");
        if (arr_at >= 0) {
            uint32_t i = static_cast<uint32_t>(arr_at) +
                         ase::utils::str_len("\"codewords\"", EDGE_KYCD_PAYLOAD_MAX);
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
                            publish_codeword(registry, owner, count, cw);
                            ++count;
                        }
                    } else {
                        ++i;  // commas / whitespace between elements
                    }
                }
            }
        }
        hub::set(registry, owner, "SES_KYCD_CWRD_COUNT"_hs, static_cast<float>(count));

        log::info("[StorageEdgeKycdResDrn] session published owner={} user='{}' clrn={} perm={} cwrds={} (req_id={})",
                  owner, user_id, clearance, permission, count, req_id);

        msg_len = 0;
    }
}

void StorageEdgeKycdResDrnSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageEdgeKycdResDrn] Stopped");
}

}  // namespace ase::storage
