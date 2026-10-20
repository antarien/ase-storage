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
 * @usecase     usecase:100265 - check access clearance
 * @created     2026-04-05
 * @modified    2026-10-13
 * @version     1.4.0
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
 *   │    - session index (rows HUB_CLAI_IDNT)     │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - StorageStaIdnComponent (client entity) │
 *   │    - StorageKycdVldTag (client entity)      │
 *   │    - StorageKycdLnkCntTag (client entity)   │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Client entity: identity + valid tag
 *          ▼
 *   StorageAcssChkSystem uses identity for ACL checks.
 *   StorageKycdLnkCntSystem turns the count markers into the link counter.
 *   THIS system is the session hub publisher — it writes the SES_* family
 *   directly with owner = client entity. ase-session holds no publisher of
 *   its own; it is a lifecycle-marker skeleton.
 *
 *   The values leave this module by TWO independent senders, neither of which
 *   this system chooses: HubNetBctReqSystem carries key creation to the browser,
 *   ReplicaHubSndSystem carries the full dirty set to the orchestrator peer over
 *   the binary WS. hub::set itself is storage, not transport.
 *
 *   The session row HUB_CLAI_IDNT belongs to ase-network, and so does its field
 *   HUB_CLAI_IDNT_USER_HASH. This system never fills it: NetworkHubSyncSystem
 *   reads SES_USER_ID_HI/_LO from the same owner and writes the field itself.
 *
 * HUB Pattern
 *
 * READS (from Hub):
 *   (none) - the session is found through StorageAcssIndexResourceManager, which
 *   StorageIdnIdxSystem fills from the rows HUB_CLAI_IDNT at the start of the frame.
 *
 * WRITES (to Hub, owner = client entity):
 *   SES_IS_AUTHENTICATED  1.0f once the keycard is linked
 *   SES_CLEARANCE         keycard clearance level
 *   SES_EXP_AT            keycard expiry (Unix seconds)
 *   SES_REALM_ID_HI/_LO   realm FNV-1a32 hash in exact-carry halves
 *   SES_USER_ID_HI/_LO    user_id FNV-1a32 hash in exact-carry halves
 *
 *   StorageKycdSesClnSystem retires this exact family when the session dies.
 *   Adding a key here without adding it there strands it in the hub forever.
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
#include <ase/storage/systems/keycard/storage_kycd_lnk_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_sta_idn_comp.hpp>
#include <ase/storage/components/state/storage_sta_sess_comp.hpp>
#include <ase/storage/components/state/storage_sta_kycd_comp.hpp>
#include <ase/storage/components/state/storage_kycd_grnt_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/storage_acss_index_resource_manager.hpp>
#include <ase/storage/components/tag/storage_kycd_vld_tag.hpp>
#include <ase/storage/components/tag/storage_kycd_rjct_tag.hpp>
#include <ase/storage/components/tag/storage_kycd_lnk_cnt_tag.hpp>
// Hub API - the ONLY allowed hub header: set
#include <ase/hub/api.hpp>
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

// No helper functions needed → all logic inline in tick()

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageKycdLnkSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdLnkSystem] Started");
}

void StorageKycdLnkSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto* idx_ptr = registry.ctx().find<StorageAcssIndexResourceManager*>();
    if (!idx_ptr || !(*idx_ptr)) {
        // SCHEDULE_ORDER: der Erzeuger (StorageIdnIdxSystem) hat den ctx-Halter noch nicht angelegt.
        log::error(log::ERR::CAT::SCHEDULE_ORDER, "StorageKycdLnkSystem",
                   "StorageAcssIndexResourceManager");
        return;
    }
    auto& idx = **idx_ptr;

    // Link each validated keycard entity to the session entity of its network client.
    // The session is found by client id in the index StorageIdnIdxSystem built from the
    // rows HUB_CLAI_IDNT.
    auto auth_view = registry.view<StorageStaIdnComponent, StorageKycdVldTag>(
        entt::exclude<StorageKycdRjctTag>);
    for (auto auth_entity : auth_view) {
        auto& auth_idn = auth_view.get<StorageStaIdnComponent>(auth_entity);
        // The session row is fetched, not joined into the view: this loop emplaces
        // onto the client entity, and every pool the view iterates is a pool that
        // must not grow while it does. One O(1) lookup on the same entity instead.
        auto* auth_sess = registry.try_get<StorageStaSessComponent>(auth_entity);
        if (auth_sess == nullptr) {
            log::error(log::ERR::CAT::COMPONENT_MISSING, "StorageKycdLnkSystem",
                       static_cast<uint32_t>(auth_entity), "StorageStaSessComponent");
            continue;
        }
        uint32_t target_client_id = auth_sess->client_id;

        // The client is reached by its client id. StorageIdnIdxSystem indexed every open
        // session at the start of this frame, so the answer is one lookup instead of a
        // walk over every client per authenticated session (WS-K.2c).
        const uint32_t client_id = idx.get_client(target_client_id);
        if (client_id != INVALID_ENTITY) {
            const auto client_entity = static_cast<ecs::Entity>(client_id);

            // Copy identity to client entity
            auto& client_idn = registry.emplace_or_replace<StorageStaIdnComponent>(client_entity);
            auto& client_sess = registry.emplace_or_replace<StorageStaSessComponent>(client_entity);
            client_sess.client_id = auth_sess->client_id;
            client_sess.authenticated_at = auth_sess->authenticated_at;
            client_sess.active_keycard = static_cast<uint32_t>(auth_entity);
            // Carry the EXACT FNV user_hash so the codeword projection (cwrd_pub
            // reads idn.user_id_hash) lands at the gate owner even if the linked
            // session entity's user_id string is ever empty/dangling.
            client_idn.user_id_hash = auth_idn.user_id_hash;
            for (uint32_t ci = 0; ci < 64; ++ci) {
                client_idn.user_id[ci] = auth_idn.user_id[ci];
                client_idn.email[ci] = auth_idn.email[ci];
                client_idn.display_name[ci] = auth_idn.display_name[ci];
            }

            registry.emplace_or_replace<StorageKycdVldTag>(client_entity);
            // One marker per link; StorageKycdLnkCntSystem counts it into the module's own
            // counter row. This system writes the star and therefore does no arithmetic.
            registry.emplace_or_replace<StorageKycdLnkCntTag>(client_entity);
            log::debug("[StorageKycdLnk] +StorageKycdVldTag client={} user='{}'",
                       static_cast<uint32_t>(client_entity), auth_idn.user_id);

            // Publish the SES_* hub contract keys for the newly-validated
            // session. hub::set writes HubDatValComponent and sets HubValDtyTag —
            // nothing more. WHICH sender picks the dirty value up is decided by
            // the reader, not here: HubNetBctReqSystem takes key creation to the
            // browser, ReplicaHubSndSystem takes the full dirty set to the peer
            // over the binary WS. There is no flag on hub::set and no generic
            // relay system in between.
            uint32_t owner = static_cast<uint32_t>(client_entity);
            uint32_t user_id_hash = entt::hashed_string{auth_idn.user_id}.value();
            // user_id STRING is NOT registered via set_debug_label (a debug-only channel); the
            // session identity lives in StorageStaIdnComponent, consumers read the string there.

            hub::set(registry, owner, "SES_IS_AUTHENTICATED"_hs, 1.0f);

            // The hash travels in two exact-carry halves — each below 2^16 and
            // therefore lossless in a float32 hub value. Same split as
            // SES_KYCD_NTF_USER_ID_HI/_LO; useSessionStore.ts
            // reassembles (HI << 16) | LO, and NetworkHubSyncSystem reassembles it the
            // same way into the u32 field HUB_CLAI_IDNT_USER_HASH of the session row,
            // which the server-side readers take as the exact number.
            hub::set(registry, owner, "SES_USER_ID_HI"_hs,
                     static_cast<float>(user_id_hash >> 16));
            hub::set(registry, owner, "SES_USER_ID_LO"_hs,
                     static_cast<float>(user_id_hash & 0xFFFFu));

            auto* kycd = registry.try_get<StorageStaKycdComponent>(auth_entity);
            auto* grnt = registry.try_get<StorageKycdGrntComponent>(auth_entity);
            if (kycd != nullptr && grnt != nullptr) {
                hub::set(registry, owner, "SES_CLEARANCE"_hs, static_cast<float>(grnt->clrn));
                hub::set(registry, owner, "SES_EXP_AT"_hs,    static_cast<float>(grnt->expires_at));
                if (kycd->relm_ref != 0) {
                    auto relm_entity = static_cast<entt::entity>(kycd->relm_ref);
                    auto* relm = registry.try_get<StorageStaRelmComponent>(relm_entity);
                    if (relm == nullptr) {
                        // The keycard names a realm row that is gone. Its entity number is no
                        // realm hash: publishing it would put an identity nobody issued on a
                        // GRANT axis. No realm is published instead, and every realm-scoped
                        // delivery treats the session as outside every realm until a keycard
                        // with a live realm is linked.
                        log::warn(log::WRN::CAT::REFERENCE_UNRESOLVED, "StorageKycdLnkSystem",
                                  kycd->relm_ref, "kycd_realm_row");
                    } else {
                        const uint32_t realm_hash = entt::hashed_string{relm->id}.value();
                        // realm id STRING is NOT registered via set_debug_label (debug-only
                        // channel). The realm is a GRANT axis: the Replica delivers a cell, a
                        // mark, a project row only to a session of the same realm. A single
                        // float carries an integer exactly only up to 2^24, so two realms
                        // whose hashes round to the same float would be one realm to every
                        // reader (D0692). Two exact-carry halves, the same split as
                        // SES_USER_ID_HI/_LO above.
                        hub::set(registry, owner, "SES_REALM_ID_HI"_hs,
                                 static_cast<float>(realm_hash >> 16));
                        hub::set(registry, owner, "SES_REALM_ID_LO"_hs,
                                 static_cast<float>(realm_hash & 0xFFFFu));
                    }
                }
            }

            log::debug("[StorageKycdLnk] SES_* published owner={} user='{}' user_hash={} clearance={}",
                       owner, auth_idn.user_id, user_id_hash,
                       grnt != nullptr ? grnt->clrn : 0);

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
