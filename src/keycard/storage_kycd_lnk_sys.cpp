/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_kycd_lnk_sys.cpp
 * @brief       StorageKycdLnkSystem - Links validated keycard to client entity
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
 * @created     2026-04-05
 * @modified    2026-04-18
 * @version     1.2.0
 *
 * CAUSAL CHAIN (Keycard Link)
 *
 *   [StorageKycdVldSystem validates token]
 *          │
 *          │ Entities with StorageKycdVldTag + StorageStaIdnComponent
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageKycdLnkSystem          │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageKycdVldTag + StorageStaIdnComp  │
 *   │    - StorageStaKycdComponent (auth entity)  │
 *   │    - NET_CLAI_ID hub value (client mirror)  │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - StorageStaIdnComponent (client entity) │
 *   │    - StorageKycdVldTag (client entity)      │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Client entity: identity + valid tag
 *          ▼
 *   StorageAcssChkSystem uses identity for ACL checks.
 *   Session hub publication lives now in ase-session (Replica-scoped):
 *   the Engine→Replica keycard-sync bridge (Phase 3) will populate the
 *   session-side SessionStaIdnComponent + SessionStaAthzComponent, and
 *   SessionIdnHubWritSystem will publish SES_* hub values from there.
 *
 * HUB Pattern
 *
 * READS (from Hub):
 *   NET_CLAI_ID — per-client session identity anchor
 *
 * WRITES (to Hub):
 *   (none — session hub publication moved to ase-session, Replica-scoped)
 *
 * FLYWEIGHT PATTERN (Active - StorageResourceManager via ctx)
 *   Identity data transferred from token entity to client entity.
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
#include <ase/storage/systems/keycard/storage_kycd_lnk_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_sta_idn_comp.hpp>
#include <ase/storage/components/state/storage_sta_kycd_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/tag/storage_tag_kycd_vld.hpp>
#include <ase/storage/components/tag/storage_tag_kycd_rjct.hpp>
// Hub API for client identity mirror (Hub API 2.0: no L3→L3 network import)
#include <ase/hub/api.hpp>
// Types (L0 — is_not_found sentinel check)
#include <ase/types/types.hpp>
// Logging
#include <ase/log/log.hpp>

#include <entt/core/hashed_string.hpp>

using namespace entt::literals;
namespace types = ase::types;

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

void StorageKycdLnkSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdLnkSystem] Started");
}

void StorageKycdLnkSystem::tick(ecs::Registry& registry, float /*dt*/) {
    // Link each validated keycard entity to its network client entity
    // Hub API 2.0: find clients via hub::HubNetClaiRdyTag (Mirror Tag) + hub::get() for client_id
    auto auth_view = registry.view<StorageStaIdnComponent, StorageKycdVldTag>(
        entt::exclude<StorageKycdRjctTag>);
    for (auto auth_entity : auth_view) {
        auto& auth_idn = auth_view.get<StorageStaIdnComponent>(auth_entity);
        uint32_t target_client_id = auth_idn.client_id;

        // Find matching client via Hub Mirror Tag (WRFL_ASE_HUB_ENTITY_DISCOVERY)
        auto client_view = registry.view<hub::HubNetClaiRdyTag>();
        for (auto client_entity : client_view) {
            float net_id = hub::get(registry, static_cast<uint32_t>(client_entity), "NET_CLAI_ID"_hs);
            if (types::is_not_found(net_id)) continue;
            if (static_cast<uint32_t>(net_id) != target_client_id) continue;

            // Copy identity to client entity
            auto& client_idn = registry.emplace_or_replace<StorageStaIdnComponent>(client_entity);
            client_idn.client_id = auth_idn.client_id;
            client_idn.authenticated_at = auth_idn.authenticated_at;
            client_idn.active_keycard = static_cast<uint32_t>(auth_entity);
            for (uint32_t ci = 0; ci < 64; ++ci) {
                client_idn.user_id[ci] = auth_idn.user_id[ci];
                client_idn.email[ci] = auth_idn.email[ci];
                client_idn.display_name[ci] = auth_idn.display_name[ci];
            }

            registry.emplace_or_replace<StorageKycdVldTag>(client_entity);
            log::debug("[StorageKycdLnk] +StorageKycdVldTag client={} user='{}'",
                       static_cast<uint32_t>(client_entity), auth_idn.user_id);
            float lnk_count = hub::get(registry, hub::GLOBAL, "STG_KYCD_LNK_COUNT"_hs, 0.0f);
            if (types::is_not_found(lnk_count)) lnk_count = 0.0f;
            hub::set(registry, hub::GLOBAL, "STG_KYCD_LNK_COUNT"_hs, lnk_count + 1.0f);

            // Publish SES_* hub contract keys for the newly-validated session
            // (ARCH_ASE_REP_LYR unified hub-centric pattern). Broadcast is
            // automatic via HubValDtyTag on value-change; ReplicationHubBctSystem
            // relays the delta to Replica and downstream consumers.
            uint32_t owner = static_cast<uint32_t>(client_entity);
            uint32_t user_id_hash = entt::hashed_string{auth_idn.user_id}.value();
            hub::set_debug_label(registry, user_id_hash, auth_idn.user_id);

            // user_id_hash is held as raw uint32_t on the session-entity's
            // hub mirror. Hub::set would transport it as float and lose
            // precision for hashes > 2^24 (ARCH_ASE_HUB_API.md). Consumers
            // read this field directly from the component.
            if (auto* mirror = registry.try_get<hub::HubStaClaiIdntComponent>(client_entity);
                mirror != nullptr) {
                mirror->user_id_hash = user_id_hash;
            }
            hub::set(registry, owner, "SES_IS_AUTHENTICATED"_hs, 1.0f);

            auto* kycd = registry.try_get<StorageStaKycdComponent>(auth_entity);
            if (kycd != nullptr) {
                hub::set(registry, owner, "SES_CLEARANCE"_hs, static_cast<float>(kycd->clrn));
                hub::set(registry, owner, "SES_EXP_AT"_hs,    static_cast<float>(kycd->expires_at));
                if (kycd->relm_ref != 0) {
                    auto relm_entity = static_cast<entt::entity>(kycd->relm_ref);
                    auto* relm = registry.try_get<StorageStaRelmComponent>(relm_entity);
                    uint32_t realm_hash = (relm != nullptr)
                                          ? entt::hashed_string{relm->id}.value()
                                          : kycd->relm_ref;
                    if (relm != nullptr) {
                        hub::set_debug_label(registry, realm_hash, relm->id);
                    }
                    hub::set(registry, owner, "SES_REALM_ID"_hs, static_cast<float>(realm_hash));
                }
            }

            log::debug("[StorageKycdLnk] SES_* published owner={} user='{}' user_hash={} clearance={}",
                       owner, auth_idn.user_id, user_id_hash,
                       kycd != nullptr ? kycd->clrn : 0);

            log::info("[StorageKycdLnk] Linked keycard to client {}", target_client_id);
            break;
        }

        // Deferred Deletion Pattern: mark temp entity for cleanup
        registry.erase<StorageKycdVldTag>(auth_entity);
        registry.emplace<StorageKycdRjctTag>(auth_entity);
    }
}

void StorageKycdLnkSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdLnkSystem] Stopped");
}

}  // namespace ase::storage
