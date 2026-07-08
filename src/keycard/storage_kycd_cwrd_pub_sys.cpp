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
 *   │    - SES_KYCD_HOLDS_{BINARY,SIG,SBOM,META}   │
 *   │      owner-scoped A/ACS hold verdicts (0/1)  │
 *   │    (codeword STRING never leaves this tier)  │
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
 *   SES_KYCD_HOLDS_<cw> — owner-scoped A/ACS hold verdict (1.0 iff the keycard holds
 *                         the exact edge codeword; exact-string compare server-internal,
 *                         the codeword STRING never crosses to the L4 gate over the Hub)
 *   SES_CLEARANCE       — keycard clearance, owner = hashed_string(user_id)
 *   SES_KYCD_PERM       — keycard permission bitflags, same owner
 *   (clearance + permission co-located with the codewords so the edge A/ACS
 *    gate reads all three axes against the SAME owner = hashed_string(user_id))
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

// Compare one held codeword against the fixed edge-distribution codewords (EXACT string, via
// str_equal which checks the null terminator — never a prefix match) and, on a match, set the
// matching owner-scoped A/ACS hold-verdict boolean. The codeword STRING is compared HERE
// (server-internal, A/ACS step 5) and NEVER crosses to the L4 edge gate: only these fixed,
// contract-registered booleans do. NEVER an FNV hash — a hash collision would be a false-grant.
void set_edge_cwrd_hold(ecs::Registry& registry, uint32_t owner, const char* cwrd) {
    if (ase::utils::str_equal(cwrd, EDGE_CWRD_BINARY, MAX_CODEWORD_LEN)) {
        hub::set(registry, owner, "SES_KYCD_HOLDS_BINARY"_hs, 1.0f);
    }
    if (ase::utils::str_equal(cwrd, EDGE_CWRD_SIG, MAX_CODEWORD_LEN)) {
        hub::set(registry, owner, "SES_KYCD_HOLDS_SIG"_hs, 1.0f);
    }
    if (ase::utils::str_equal(cwrd, EDGE_CWRD_SBOM, MAX_CODEWORD_LEN)) {
        hub::set(registry, owner, "SES_KYCD_HOLDS_SBOM"_hs, 1.0f);
    }
    if (ase::utils::str_equal(cwrd, EDGE_CWRD_METADATA, MAX_CODEWORD_LEN)) {
        hub::set(registry, owner, "SES_KYCD_HOLDS_METADATA"_hs, 1.0f);
    }
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageKycdCwrdPubSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdCwrdPub] Started");
}

void StorageKycdCwrdPubSystem::tick(ecs::Registry& registry, float /*dt*/) {
    // For each authenticated session (same set StorageKycdLnkSystem publishes
    // SES_CLEARANCE for), publish the owner-scoped A/ACS hold verdicts for the
    // codewords its keycard holds so the edge gate (ase-pl-edge-webserver) can
    // enforce them WITHOUT the codeword string ever crossing the Hub.
    auto session_view = registry.view<StorageStaIdnComponent, StorageKycdVldTag>();
    for (auto [session_entity, idn] : session_view.each()) {
        (void)session_entity;
        // owner = the EXACT carried FNV hash of user_id == the edge-gate consumer's
        // entt::hashed_string(X-ASE-User-Id).value(), so SES_CLEARANCE and
        // SES_KYCD_HOLDS_* verdicts co-locate at the owner the binary gate reads. Deriving
        // from the carried hash (not re-hashing idn.user_id) keeps the projection
        // correct even if the string was empty/dangling on the local mint path.
        uint32_t owner = idn.user_id_hash;

        // Locate this session's keycard entity (issued_to == user_id) and
        // publish each held codeword in a single pass per codeword entity.
        // The same keycard carries the two scalar A/ACS axes the edge gate
        // reads at this owner — clearance (14.1 step 4) and permission bitflags
        // (14.1 step 6) — so co-locate all three at hashed_string(user_id):
        // codewords (step 5) plus SES_CLEARANCE and SES_KYCD_PERM. Without the
        // co-location the gate's clearance/permission reads at this owner return
        // NOT_FOUND while only the codewords resolve (axis-split A/ACS = leak).
        // Reset the owner-scoped edge A/ACS hold-verdicts before the pass so a revoked or
        // re-issued keycard cannot leave a stale grant (the gate reads only these booleans).
        hub::set(registry, owner, "SES_KYCD_HOLDS_BINARY"_hs, 0.0f);
        hub::set(registry, owner, "SES_KYCD_HOLDS_SIG"_hs, 0.0f);
        hub::set(registry, owner, "SES_KYCD_HOLDS_SBOM"_hs, 0.0f);
        hub::set(registry, owner, "SES_KYCD_HOLDS_METADATA"_hs, 0.0f);

        auto kycd_view = registry.view<StorageStaKycdComponent>();
        uint32_t count = 0;
        for (auto [kycd_entity, kycd] : kycd_view.each()) {
            if (!ase::utils::str_equal(kycd.issued_to, idn.user_id, MAX_OWNER_ID)) {
                continue;
            }

            hub::set(registry, owner, "SES_CLEARANCE"_hs, static_cast<float>(kycd.clrn));
            hub::set(registry, owner, "SES_KYCD_PERM"_hs, static_cast<float>(kycd.perm));

            uint32_t kycd_ref = static_cast<uint32_t>(kycd_entity);
            auto cwrd_view = registry.view<StorageKycdCwrdComponent>();
            for (auto [cwrd_entity, cwrd] : cwrd_view.each()) {
                (void)cwrd_entity;
                if (cwrd.kycd_ref != kycd_ref) {
                    continue;
                }
                // Exact-string A/ACS compare, server-internal — the codeword string never
                // crosses to the L4 edge gate; only the fixed hold-verdict booleans do.
                set_edge_cwrd_hold(registry, owner, cwrd.cwrd);
                ++count;
            }
        }

        log::debug("[StorageKycdCwrdPub] owner={} user='{}' codewords={}",
                   owner, idn.user_id, count);
    }
}

void StorageKycdCwrdPubSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdCwrdPub] Stopped");
}

}  // namespace ase::storage
