/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_kycd_rev_sys.cpp
 * @brief       StorageKycdRevSystem - Tags revoked keycards
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Preservation
 * @created     2026-04-05
 * @modified    2026-06-24
 * @version     1.0.0
 *
 * CAUSAL CHAIN (Keycard Revocation)
 *
 *   [Admin revokes keycard via API]
 *          │
 *          │ StorageStaKycdComponent entity flagged in ECS
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageKycdRevSystem          │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageStaKycdComponent + KycdRevTag   │
 *   │    - SES_KYCD_PERSIST_SEQ (owner watermark) │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - SES_KYCD_PERSIST_* (op=REVOKE, Hub)    │
 *   │    - StorageKycdRevPstTag (emit-once mark)  │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Replica updates the durable keycard document (revoked)
 *          ▼
 *   StorageAcssChkSystem rejects revoked keycards in-memory meanwhile
 *
 * HUB Pattern (Active - emits owner-keyed durable-revoke signal)
 *
 * READS (from Hub):
 *   SES_KYCD_PERSIST_SEQ - monotonic emit counter per owner (read-modify-write)
 *
 * WRITES (to Hub):
 *   SES_KYCD_PERSIST_SEQ       - bumped once per revoked keycard
 *   SES_KYCD_PERSIST_OP        - KYCD_PST_OP_REVOKE for the revoked keycard
 *   SES_KYCD_PERSIST_KYCD_HASH - durable primary key (label = SHA-256 hex)
 *
 * FLYWEIGHT PATTERN (N/A - no ResourceManager access)
 *   Revocation rides the SAME durable Hub path issuance uses, keyed by kycd_hash,
 *   so issuance and revoke address one durable store (Phase 12 H-3).
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
#include <ase/storage/systems/keycard/storage_kycd_rev_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_sta_kycd_comp.hpp>
#include <ase/storage/components/tag/storage_tag_kycd_rev.hpp>
#include <ase/storage/components/tag/storage_tag_kycd_rev_pst.hpp>
// Module constants (KYCD_PST_OP_REVOKE)
#include <ase/storage/types.hpp>
// Hub API for owner-keyed durable-revoke signal
#include <ase/hub/api.hpp>
// Types (L0 — is_not_found sentinel check)
#include <ase/types/types.hpp>
// Logging
#include <ase/log/log.hpp>

#include <entt/core/hashed_string.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
namespace {

// No helper functions needed → all logic inline in tick()

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageKycdRevSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdRev] Started");
}

void StorageKycdRevSystem::tick(ecs::Registry& registry, float /*dt*/) {
    // The HTTP revoke route (POST /api/realms/:rid/keycards/:id/revoke) emplaces
    // StorageKycdRevTag on the keycard entity. The 10-step A/ACS check
    // (ARCH_ASE_STORAGE.md 14.1 step 1) already denies any keycard carrying that
    // tag in-memory. This system makes the revocation DURABLE: it emits an
    // owner-keyed SES_KYCD_PERSIST_* signal with op=KYCD_PST_OP_REVOKE so the
    // Replica updates the SAME keycard MongoDB document the mint upserted (keyed
    // by kycd_hash) — issuance and revoke address one durable store, not a record
    // the mint never wrote. StorageKycdRevPstTag makes the emit exactly-once.
    auto view = registry.view<StorageStaKycdComponent, StorageKycdRevTag>(
        entt::exclude<StorageKycdRevPstTag>);
    for (auto [entity, kycd] : view.each()) {
        // A keycard without a SHA-256 digest has no durable primary key — the
        // Replica cannot locate its document to mark revoked. Mark done so it is
        // not re-scanned, and log the malformed state as an error.
        if (kycd.kycd_hash[0] == '\0') {
            log::error("[StorageKycdRev] revoked keycard entity={} has empty kycd_hash — not durably revocable",
                       static_cast<uint32_t>(entity));
            registry.emplace<StorageKycdRevPstTag>(entity);
            continue;
        }

        uint32_t owner = entt::hashed_string{kycd.issued_to}.value();
        float seq_f = hub::get(registry, owner, "SES_KYCD_PERSIST_SEQ"_hs, 0.0f);
        if (ase::types::is_not_found(seq_f)) {
            seq_f = 0.0f;
        }
        uint32_t seq = static_cast<uint32_t>(seq_f) + 1;
        hub::set(registry, owner, "SES_KYCD_PERSIST_SEQ"_hs, static_cast<float>(seq));
        hub::set(registry, owner, "SES_KYCD_PERSIST_OP"_hs, static_cast<float>(KYCD_PST_OP_REVOKE));

        uint32_t kycd_hash_id = entt::hashed_string{kycd.kycd_hash}.value();
        hub::set(registry, owner, "SES_KYCD_PERSIST_KYCD_HASH"_hs, static_cast<float>(kycd_hash_id));
        // keycard hash STRING is NOT registered via set_debug_label (a debug-only channel).

        registry.emplace<StorageKycdRevPstTag>(entity);
        log::info("[StorageKycdRev] owner={} hash='{}' seq={} durable revoke → Replica",
                  owner, kycd.kycd_hash, seq);
    }
}

void StorageKycdRevSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdRev] Stopped");
}

}  // namespace ase::storage
