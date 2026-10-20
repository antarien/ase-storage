/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_kycd_ntfy_drn_sys.cpp
 * @brief       StorageKycdNtfyDrnSystem - Bridges SDK Hub keycard-notify signals
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
 * @usecase     usecase:100264 - issue customer keycard
 * @created     2026-04-16
 * @modified    2026-10-12
 * @version     1.1.0
 *
 * CAUSAL CHAIN (SDK Hub Bridge → Storage Request Component)
 *
 *   [plugins/ase-pl-webserver auth_routes POST /api/auth/keycard-issued]
 *          │
 *          │ the route verifies the HMAC, WebserverKycdDrnSystem files the hub row
 *          │ HUB_STG_KYCD_REQ (mark HUB_STG_KYCD); StorageKycdNtfySynSystem mirrors it
 *          ▼
 *   ┌─────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageKycdNtfyDrnSystem                      │
 *   │                                                             │
 *   │  READS (per request entity):                                │
 *   │    - StorageInpKycdNtfyIdnComponent  (hashes, user id)      │
 *   │    - StorageInpKycdNtfyGrntComponent (clearance, perm, exp) │
 *   │    - SES_KYCD_NTF_GRANT_* (codeword grant flags, Hub)       │
 *   │                                                             │
 *   │  WRITES:                                                    │
 *   │    - StorageReqKycdComponent on the same request entity     │
 *   │    - StorageReqKycdRelmComponent, StorageReqKycdCwrdComponent│
 *   └─────────────────────────────────────────────────────────────┘
 *          │
 *          │ Entity now carries StorageReqKycdComponent
 *          ▼
 *   StorageKycdReqDrnSystem picks it up, creates token entity,
 *   destroys request entity (row fields released with it).
 *
 * HUB Pattern (ARCH_ASE_HUB_API v2.0)
 *
 * READS (from Hub):
 *   SES_KYCD_NTF_GRANT_BINARY, _SIG, _SBOM, _METADATA (owner = the request entity)
 *
 * WRITES (to Hub):
 *   (none — component emplace is module-local)
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
#include <ase/storage/systems/keycard/storage_kycd_ntfy_drn_sys.hpp>
// Components from same module (request-side)
#include <ase/storage/components/request/storage_req_kycd_comp.hpp>
#include <ase/storage/components/request/storage_req_kycd_relm_comp.hpp>
#include <ase/storage/components/request/storage_req_kycd_cwrd_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/storage_acss_index_resource_manager.hpp>
#include <ase/storage/types.hpp>
// Hub API (the codeword grant flags on the request entity)
#include <ase/hub/api.hpp>
// Die Bruecke des SYN-Schnitts: die gespiegelten Felder der Hub-Zeile.
#include <ase/storage/components/state/storage_inp_kycd_ntfy_idn_comp.hpp>
#include <ase/storage/components/state/storage_inp_kycd_ntfy_grnt_comp.hpp>
// Types (L0 — is_not_found sentinel check)
#include <ase/types/types.hpp>
// String ops (L0 — codeword copy)
#include <ase/utils/strops.hpp>

#include <entt/core/hashed_string.hpp>
// Logging
#include <ase/log/log.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
namespace {

// No helper functions needed → all logic inline in tick()

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageKycdNtfyDrnSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdNtfyDrnSystem] Started");
}

void StorageKycdNtfyDrnSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto* idx_ptr = registry.ctx().find<StorageAcssIndexResourceManager*>();
    if (!idx_ptr || !(*idx_ptr)) {
        // SCHEDULE_ORDER, woertlich: „System runs before its dependency". Der
        // freie String sagte dasselbe in Prosa — die Kategorie macht daraus etwas Zaehlbares.
        // Die 3-Argument-Form ohne Owner ist hier die richtige: es gibt an dieser Stelle keine
        // Entity, der Befund gilt dem System, nicht einer Zeile.
        //
        // Der Systemname wechselt auf die volle Klassenform, die on_start und on_stop dieser
        // Datei ohnehin schon schreiben — sonst emittierte sie zwei Schreibweisen fuer
        // dasselbe System und spaltete ihren eigenen Logstrom.
        log::error(log::ERR::CAT::SCHEDULE_ORDER, "StorageKycdNtfyDrnSystem",
                   "StorageAcssIndexResourceManager");
        return;
    }
    auto& idx = **idx_ptr;

    /**
     * DIE STERNSEITE STEHT NICHT HIER. StorageKycdNtfySynSystem liest die Hub-Zeile und legt das
     * Gelesene in zwei Bruecken-Zeilen ab; dieses System baut daraus die Anfrage.
     *
     * DIE BRUECKE IM SICHTKOPF IST ZUGLEICH DER FILTER: eine Anfrage, deren Pflichtfelder fehlten,
     * bekommt keine Bruecke und erreicht diesen Durchlauf gar nicht erst.
     */
    auto view = registry.view<StorageInpKycdNtfyIdnComponent, StorageInpKycdNtfyGrntComponent>(
        entt::exclude<StorageReqKycdComponent>);
    for (auto [req_entity, idn_inp, grnt_inp] : view.each()) {
        uint32_t owner = static_cast<uint32_t>(req_entity);

        // The hashes and the expiry arrive whole: the row carries them as u32 and u64 fields.
        const uint32_t user_hash  = idn_inp.user_hash;
        const uint32_t clearance  = grnt_inp.clearance;
        const uint32_t realm_hash = idn_inp.realm_hash;
        const uint64_t exp_at     = grnt_inp.expires_at;

        // The user_id STRING the SDK carried on THIS request row, mirrored into the bridge. Works
        // on first-login issuance (no session identity exists yet) and operator-mint-for-other
        // alike, because the request always carries the string.
        const char* user_id_str = idn_inp.user_id;

        auto& payload = registry.emplace<StorageReqKycdComponent>(req_entity);
        size_t copy_len = 0;
        while (user_id_str[copy_len] != '\0' && copy_len < 63) {
            payload.user_id[copy_len] = user_id_str[copy_len];
            ++copy_len;
        }
        payload.user_id[copy_len] = '\0';
        payload.user_id_hash     = user_hash;
        payload.authenticated_at = exp_at > 30 ? (exp_at - 30) : 0;
        payload.expires_at       = exp_at;
        payload.clearance        = static_cast<uint8_t>(clearance);

        // Realm + permission binding (Phase 12 operator mint): resolve the realm
        // hash to its realm entity and carry it + permission bits as a sibling
        // request component. Absent for the legacy auth-gate flow (realm_hash 0).
        if (realm_hash != 0) {
            // The realm is reached by its id hash, which is exactly the key the index is
            // built on - one number against one number.
            const uint32_t realm_ref = idx.get_realm(static_cast<uint64_t>(realm_hash));
            if (realm_ref != INVALID_ENTITY) {
                auto& relm_req = registry.emplace<StorageReqKycdRelmComponent>(req_entity);
                relm_req.relm_ref = realm_ref;
                relm_req.perm = static_cast<uint16_t>(grnt_inp.perm);
            }
        }

        // Codeword grants (Phase 12 operator mint): the mint request carries the fixed edge
        // codewords as owner-scoped SES_KYCD_NTF_GRANT_<CW> flags (Hub VALUES, never the codeword
        // string over a debug-label - that is a debug-only channel). For each set flag, emplace a
        // StorageReqKycdCwrdComponent grant with the exact fixed codeword string mapped here,
        // server-internal. Codewords outside this fixed edge set travel only on the durable
        // keycard-persist path (they are not part of the edge download A/ACS gate anyway).
        const char* grant_keys[] = { "SES_KYCD_NTF_GRANT_BINARY", "SES_KYCD_NTF_GRANT_SIG",
                                     "SES_KYCD_NTF_GRANT_SBOM", "SES_KYCD_NTF_GRANT_METADATA" };
        const char* grant_cwrd[] = { EDGE_CWRD_BINARY, EDGE_CWRD_SIG, EDGE_CWRD_SBOM,
                                     EDGE_CWRD_METADATA };
        for (uint32_t gi = 0; gi < sizeof(grant_keys) / sizeof(grant_keys[0]); ++gi) {
            float grant_f = hub::get(registry, owner, entt::hashed_string(grant_keys[gi]).value());
            if (ase::types::is_not_found(grant_f)) continue;
            if (grant_f < 0.5f) continue;
            auto cw_ent = registry.create();
            auto& cwc = registry.emplace<StorageReqKycdCwrdComponent>(cw_ent);
            cwc.req_ref = static_cast<uint32_t>(req_entity);
            ase::utils::str_copy(cwc.cwrd, MAX_CODEWORD_LEN, grant_cwrd[gi]);
        }

        log::debug("[StorageKycdNtfyDrn] +StorageReqKycdComponent entity={} user='{}' clearance={} exp={}",
                   owner, user_id_str, clearance, exp_at);
    }
}

void StorageKycdNtfyDrnSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdNtfyDrnSystem] Stopped");
}

}  // namespace ase::storage
