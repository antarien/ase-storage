/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_kycd_vld_sys.cpp
 * @brief       StorageKycdVldSystem - Validates keycard JWT via HMAC-SHA256
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
 * @created     2026-04-05
 * @modified    2026-04-16
 * @version     1.1.0
 *
 * CAUSAL CHAIN (Keycard Validation)
 *
 *   [StorageKycdDrnSystem creates token entities]
 *          │
 *          │ Entities with StorageKycdPendTag
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageKycdVldSystem          │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageKycdPendTag + StorageStaTknComp │
 *   │    - StorageResourceManager (JWT validation)│
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - StorageKycdVldTag or StorageKycdRjctTag│
 *   │    - StorageStaIdnComponent (identity data) │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Validated entities with identity
 *          ▼
 *   StorageKycdLnkSystem links identity to client
 *
 * HUB Pattern (N/A - No Hub reads/writes)
 *
 * READS (from Hub):
 *   (none)
 *
 * WRITES (to Hub):
 *   (none)
 *
 * FLYWEIGHT PATTERN (Active - StorageResourceManager via ctx)
 *   Token strings and JWT validation logic in StorageResourceManager.
 *   Components store only uint32_t token_id.
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
#include <ase/storage/systems/keycard/storage_kycd_vld_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_sta_tkn_comp.hpp>
#include <ase/storage/components/state/storage_sta_idn_comp.hpp>
#include <ase/storage/components/tag/storage_tag_kycd_pend.hpp>
#include <ase/storage/components/tag/storage_tag_kycd_vld.hpp>
#include <ase/storage/components/tag/storage_tag_kycd_rjct.hpp>
#include <ase/storage/storage_resource_manager.hpp>
// Hub API (counter)
#include <ase/hub/api.hpp>
// Types (L0 — is_not_found sentinel check)
#include <ase/types/types.hpp>
// FNV-1a32 hashing for the exact session owner
#include <entt/core/hashed_string.hpp>
// Logging
#include <ase/log/log.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
// IMPORTANT: Use anonymous namespace, NOT static keyword!
//   namespace { void helper() {...} }   // CORRECT
//   static void helper() {...}          // WRONG!
// NO STRUCTS HERE! Structs = Data = Components!
// NO View/Query operations in helpers! Only pure math!
namespace {

// No helper functions needed → all logic inline in tick()

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageKycdVldSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdVldSystem] Started");
}

void StorageKycdVldSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto& mgr = *registry.ctx().get<StorageResourceManager*>();

    auto view = registry.view<StorageStaTknComponent, StorageKycdPendTag>();
    for (auto entity : view) {
        auto& tkn = view.get<StorageStaTknComponent>(entity);

        auto result = mgr.validate_jwt(tkn.token_id);

        if (result.valid) {
            auto& idn = registry.emplace<StorageStaIdnComponent>(entity);
            idn.client_id = tkn.client_id;
            idn.authenticated_at = mgr.get_wall_time_seconds();
            for (uint32_t ci = 0; ci < 64; ++ci) {
                idn.user_id[ci] = result.user_id[ci];
            }
            // Exact FNV owner from the validated JWT user_id — identical to the
            // edge gate's entt::hashed_string(X-ASE-User-Id).value(). The codeword
            // projection (cwrd_pub) reads idn.user_id_hash, never re-hashes.
            idn.user_id_hash = entt::hashed_string(idn.user_id).value();

            registry.emplace<StorageKycdVldTag>(entity);
            registry.erase<StorageKycdPendTag>(entity);
            mgr.remove_token(tkn.token_id);

            log::debug("[StorageKycdVld] +StorageKycdVldTag client_id={} user='{}'", tkn.client_id, idn.user_id);
            float acc_count = hub::get(registry, hub::GLOBAL, "STG_KYCD_VLD_ACCEPT_COUNT"_hs, 0.0f);
            if (ase::types::is_not_found(acc_count)) acc_count = 0.0f;
            hub::set(registry, hub::GLOBAL, "STG_KYCD_VLD_ACCEPT_COUNT"_hs, acc_count + 1.0f);
            log::info("[StorageKycdVld] Keycard validated for client {}", tkn.client_id);
        } else {
            registry.emplace<StorageKycdRjctTag>(entity);
            registry.erase<StorageKycdPendTag>(entity);
            mgr.remove_token(tkn.token_id);

            log::debug("[StorageKycdVld] +StorageKycdRjctTag client_id={} token_id={}", tkn.client_id, tkn.token_id);
            float rej_count = hub::get(registry, hub::GLOBAL, "STG_KYCD_VLD_REJECT_COUNT"_hs, 0.0f);
            if (ase::types::is_not_found(rej_count)) rej_count = 0.0f;
            hub::set(registry, hub::GLOBAL, "STG_KYCD_VLD_REJECT_COUNT"_hs, rej_count + 1.0f);
            log::warn("[StorageKycdVld] Keycard rejected for client {}", tkn.client_id);
        }
    }
}

void StorageKycdVldSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdVldSystem] Stopped");
}

}  // namespace ase::storage
