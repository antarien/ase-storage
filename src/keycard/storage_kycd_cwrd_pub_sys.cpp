/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_kycd_cwrd_pub_sys.cpp
 * @brief       StorageKycdCwrdPubSystem - Publishes session codewords to the Hub
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
 * @created     2026-06-24
 * @modified    2026-06-24
 * @version     1.0.0
 *
 * CAUSAL CHAIN (Session Codeword Publication)
 *
 *   [StorageKycdLnkSystem links validated keycard to client]
 *          │
 *          │ Client entity: StorageStaIdnComponent + StorageKycdVldTag
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageKycdCwrdPubSystem      │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageStaIdnComponent + KycdVldTag    │
 *   │    - StorageStaKycdComponent (issued_to)    │
 *   │    - StorageKycdCwrdComponent (kycd_ref)    │
 *   │                                             │
 *   │  WRITES (to Hub):                           │
 *   │    - SES_KYCD_CWRD_COUNT (per-session owner)│
 *   │    - SES_KYCD_CWRD_<i> debug labels         │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ owner = hashed_string(user_id) — same owner the
 *          │ edge A/ACS gate reads SES_CLEARANCE on
 *          ▼
 *   ase-pl-edge-webserver acl_gate enforces codeword possession.
 *
 * HUB Pattern (Active - session codeword publication)
 *
 * READS (from Hub):
 *   (none)
 *
 * WRITES (to Hub):
 *   SES_KYCD_CWRD_COUNT — number of codewords held by the session keycard
 *   SES_KYCD_CWRD_<i>   — per-index codeword debug labels (i in [0, count))
 *
 * FLYWEIGHT PATTERN (N/A — no external resource handles)
 *   Codeword strings live in StorageKycdCwrdComponent (char[]), no ctx handles.
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
 * [ ] Cleanup System in Schedule::Last?
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
#include <ase/storage/systems/keycard/storage_kycd_cwrd_pub_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_sta_idn_comp.hpp>
#include <ase/storage/components/state/storage_sta_kycd_comp.hpp>
#include <ase/storage/components/state/storage_kycd_cwrd_comp.hpp>
#include <ase/storage/components/tag/storage_tag_kycd_vld.hpp>
// Module constants (MAX_OWNER_ID, MAX_CODEWORD_LEN)
#include <ase/storage/types.hpp>
// Hub API for cross-module session contract keys
#include <ase/hub/api.hpp>
// Utils (L0 — safe C-string operations)
#include <ase/utils/strops.hpp>
// Logging
#include <ase/log/log.hpp>

#include <entt/core/hashed_string.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
// IMPORTANT: Use anonymous namespace, NOT static keyword!
//   namespace { void helper() {...} }   // CORRECT
//   static void helper() {...}          // WRONG!
// NO STRUCTS HERE! Structs = Data = Components!
// NO View/Query operations in helpers! Only pure math!
namespace {

// Write the decimal representation of value into dst (bounded by dst_size).
// Mirrors std::to_string(value) digit-for-digit so the resulting
// "SES_KYCD_CWRD_<i>" hash matches the edge-gate consumer exactly.
// Pure math — no views, no components. The do-while always emits at least
// one digit, so value zero yields "0" without an equality check.
void append_decimal(char* dst, uint32_t dst_size, uint32_t value) {
    char tmp[11] = {};  // uint32_t max = 4294967295 → 10 digits + null
    uint32_t di = 0;
    do {
        tmp[di++] = static_cast<char>('0' + (value % 10));
        value /= 10;
    } while (value > 0 && di < 10);
    // tmp holds digits reversed — emit a forward-ordered string for str_append.
    char fwd[11] = {};
    for (uint32_t fi = 0; fi < di; ++fi) {
        fwd[fi] = tmp[di - 1 - fi];
    }
    fwd[di] = '\0';
    ase::utils::str_append(dst, dst_size, fwd);
}

// Build the indexed contract key "SES_KYCD_CWRD_<index>" into key.
void build_cwrd_key(char* key, uint32_t key_size, uint32_t index) {
    ase::utils::str_copy(key, key_size, "SES_KYCD_CWRD_");
    append_decimal(key, key_size, index);
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageKycdCwrdPubSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdCwrdPub] Started");
}

void StorageKycdCwrdPubSystem::tick(ecs::Registry& registry, float /*dt*/) {
    // For each authenticated session (same set StorageKycdLnkSystem publishes
    // SES_CLEARANCE for), publish the codewords held by its keycard so the
    // edge A/ACS gate (ase-pl-edge-webserver) can enforce them.
    auto session_view = registry.view<StorageStaIdnComponent, StorageKycdVldTag>();
    for (auto [session_entity, idn] : session_view.each()) {
        (void)session_entity;
        // owner = hashed_string(user_id) — identical to the edge-gate consumer's
        // owner derivation, so SES_CLEARANCE and SES_KYCD_CWRD_* co-locate.
        uint32_t owner = entt::hashed_string{idn.user_id}.value();

        // Locate this session's keycard entity (issued_to == user_id) and
        // publish each held codeword in a single pass per codeword entity.
        auto kycd_view = registry.view<StorageStaKycdComponent>();
        uint32_t count = 0;
        for (auto [kycd_entity, kycd] : kycd_view.each()) {
            if (!ase::utils::str_equal(kycd.issued_to, idn.user_id, MAX_OWNER_ID)) {
                continue;
            }

            uint32_t kycd_ref = static_cast<uint32_t>(kycd_entity);
            auto cwrd_view = registry.view<StorageKycdCwrdComponent>();
            for (auto [cwrd_entity, cwrd] : cwrd_view.each()) {
                (void)cwrd_entity;
                if (cwrd.kycd_ref != kycd_ref) {
                    continue;
                }
                char key[40] = {};
                build_cwrd_key(key, sizeof(key), count);
                uint32_t key_hash = entt::hashed_string{key}.value();
                hub::set_debug_label(registry, key_hash, cwrd.cwrd);
                ++count;
            }
        }

        hub::set(registry, owner, "SES_KYCD_CWRD_COUNT"_hs, static_cast<float>(count));
        log::debug("[StorageKycdCwrdPub] owner={} user='{}' codewords={}",
                   owner, idn.user_id, count);
    }
}

void StorageKycdCwrdPubSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdCwrdPub] Stopped");
}

}  // namespace ase::storage
