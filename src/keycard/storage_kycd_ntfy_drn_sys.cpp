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
 * @actor       Provider Credential Keeper (PROVCREKEE)
 * @created     2026-04-16
 * @modified    2026-11-16
 * @version     00.00.39.00039 [core]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * CAUSAL CHAIN (SDK Hub Bridge → Storage Request Component)
 *
 *   [plugins/ase-pl-webserver auth_routes POST /api/auth/keycard-issued]
 *          │
 *          │ the route verifies the HMAC, WebserverKycdDrnSystem files the hub row
 *          │ HUB_STG_KYCD_REQ (mark HUB_STG_KYCD); StorageKycdNtfySynSystem reads it
 *          │ into the two input components
 *          ▼
 *   ┌─────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageKycdNtfyDrnSystem                      │
 *   │                                                             │
 *   │  READS (per request entity):                                │
 *   │    - StorageInpKycdNtfyIdnComponent  (hashes, user id)      │
 *   │    - StorageInpKycdNtfyGrntComponent (clearance, perm, exp) │
 *   │    - SES_KYCD_NTF_GRANT_* (codeword grant flags, Hub)       │
 *   │                                                             │
 *   │  WRITES (after the walk):                                   │
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
// Die Eingaenge des SYN-Schnitts: StorageKycdNtfySynSystem fuellt sie aus der Hub-Zeile.
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
     * DIE STERNSEITE STEHT NICHT HIER. StorageKycdNtfySynSystem liest die Hub-Zeile und fuellt
     * das Gelesene in zwei Eingangs-Components; dieses System baut daraus die Anfrage.
     *
     * DER EINGANG IM SICHTKOPF IST ZUGLEICH DER FILTER: eine Anfrage, deren Pflichtfelder
     * fehlten, bekommt keinen Eingang und erreicht diesen Durchlauf gar nicht erst.
     *
     * DER LAUF SAMMELT NUR, ER BAUT NICHTS. StorageReqKycdComponent steht in der
     * Ausschlussliste dieser View; sie im Lauf zu setzen aendert die Menge, die der Lauf
     * durchgeht (VIEW_SIGNATURE_TYPE_MUTATED, WRFL_ASE_DEFERRED_DELETION Section 5). Gebaut
     * wird danach, ueber die gesammelten Eintraege. Die Grenze ist KYCD_REQ_BATCH: was der
     * Lauf nicht mehr fasst, behaelt seinen Eingang und kommt im naechsten Takt dran.
     */
    ecs::Entity drained[KYCD_REQ_BATCH];
    uint32_t drained_n = 0;
    auto view = registry.view<StorageInpKycdNtfyIdnComponent, StorageInpKycdNtfyGrntComponent>(
        entt::exclude<StorageReqKycdComponent>);
    for (auto req_entity : view) {
        if (drained_n >= KYCD_REQ_BATCH) break;
        drained[drained_n] = req_entity;
        ++drained_n;
    }

    for (uint32_t di = 0; di < drained_n; ++di) {
        const ecs::Entity req_entity = drained[di];
        const auto& idn_inp  = registry.get<StorageInpKycdNtfyIdnComponent>(req_entity);
        const auto& grnt_inp = registry.get<StorageInpKycdNtfyGrntComponent>(req_entity);
        uint32_t owner = static_cast<uint32_t>(req_entity);

        // The hashes and the expiry arrive whole: the row carries them as u32 and u64 fields.
        const uint32_t user_hash  = idn_inp.user_hash;
        const uint32_t clearance  = grnt_inp.clearance;
        const uint32_t realm_hash = idn_inp.realm_hash;
        const uint64_t exp_at     = grnt_inp.expires_at;

        // The user_id STRING the SDK carried on THIS request row, read into the input
        // component by the sync system. Works on first-login issuance (no session identity
        // exists yet) and operator-mint-for-other alike, because the request always carries
        // the string.
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
        // hash to its realm entity and carry it + permission bits as a second request
        // component on the same entity. Absent for the legacy auth-gate flow (realm_hash 0).
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
        //
        // EACH KEY IS READ BY ITS OWN LITERAL. The four keys stood in a char array and were hashed
        // at run time: no reader census of the hub could see this system as their reader, and the
        // array broke the no-array rule. One read per key, checked where it is read, each with the
        // codeword it grants.
        const float grant_binary = hub::get(registry, owner, "SES_KYCD_NTF_GRANT_BINARY"_hs);
        if (!ase::types::is_not_found(grant_binary) && grant_binary >= 0.5f) {
            auto cw_ent = registry.create();
            auto& cwc = registry.emplace<StorageReqKycdCwrdComponent>(cw_ent);
            cwc.req_ref = static_cast<uint32_t>(req_entity);
            ase::utils::str_copy(cwc.cwrd, MAX_CODEWORD_LEN, EDGE_CWRD_BINARY);
        }
        const float grant_sig = hub::get(registry, owner, "SES_KYCD_NTF_GRANT_SIG"_hs);
        if (!ase::types::is_not_found(grant_sig) && grant_sig >= 0.5f) {
            auto cw_ent = registry.create();
            auto& cwc = registry.emplace<StorageReqKycdCwrdComponent>(cw_ent);
            cwc.req_ref = static_cast<uint32_t>(req_entity);
            ase::utils::str_copy(cwc.cwrd, MAX_CODEWORD_LEN, EDGE_CWRD_SIG);
        }
        const float grant_sbom = hub::get(registry, owner, "SES_KYCD_NTF_GRANT_SBOM"_hs);
        if (!ase::types::is_not_found(grant_sbom) && grant_sbom >= 0.5f) {
            auto cw_ent = registry.create();
            auto& cwc = registry.emplace<StorageReqKycdCwrdComponent>(cw_ent);
            cwc.req_ref = static_cast<uint32_t>(req_entity);
            ase::utils::str_copy(cwc.cwrd, MAX_CODEWORD_LEN, EDGE_CWRD_SBOM);
        }
        const float grant_metadata = hub::get(registry, owner, "SES_KYCD_NTF_GRANT_METADATA"_hs);
        if (!ase::types::is_not_found(grant_metadata) && grant_metadata >= 0.5f) {
            auto cw_ent = registry.create();
            auto& cwc = registry.emplace<StorageReqKycdCwrdComponent>(cw_ent);
            cwc.req_ref = static_cast<uint32_t>(req_entity);
            ase::utils::str_copy(cwc.cwrd, MAX_CODEWORD_LEN, EDGE_CWRD_METADATA);
        }

        log::debug("[StorageKycdNtfyDrn] +StorageReqKycdComponent entity={} user='{}' clearance={} exp={}",
                   owner, user_id_str, clearance, exp_at);
    }
}

void StorageKycdNtfyDrnSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdNtfyDrnSystem] Stopped");
}

}  // namespace ase::storage
