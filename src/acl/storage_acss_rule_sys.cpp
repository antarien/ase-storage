/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_acss_rule_sys.cpp
 * @brief       StorageAcssRuleSystem - Match the most specific ACL rule for each visible request
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @design      DSGN_021
 * @usecase     usecase:100265 - check access clearance
 * @actor       Provider Credential Keeper (PROVCREKEE)
 * @created     2026-11-16
 * @modified    2026-11-16
 * @version     00.00.39.00039 [core]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * WELCHE REGEL REGIERT DIESEN PFAD — DIE ZWEITE NACHSCHLAG-FRAGE DER LEITER
 *
 * StorageAcssRslvSystem beantwortet, ob die Keycard gilt und das Revier sichtbar ist; dieses
 * System beantwortet, welche ACL-Regel des Reviers den angefragten Pfad regiert. Die beiden
 * Fragen haben zwei verschiedene Rechnungen - Pfad-Vorfahren gegen den Revier-Index dort,
 * Pfad-Hashes gegen die Regel-Eimer hier -, und eine Aenderung am Regelabgleich (welche von
 * zwei ueberlappenden Regeln gewinnt) trifft nur diese Datei.
 *
 * ES WEIST NICHTS AB. Ein Pfad ohne Regel behaelt die PUBLIC-Schutzstufe des Reviers, und das
 * oeffentliche Revier liefert damit seine implizite Regel (Stufe 0, keine Codewoerter): die
 * Vorgaben der Engine bleiben fuer jeden angemeldeten Nutzer lesbar, innerhalb der Leiter und
 * ohne Freigabe vor ihr. Abgewiesen wird erst auf den Toren von StorageAcssChkSystem.
 *
 * DER LAUF SETZT DIE AUFLOESUNGS-KOMPONENTE, DIE MARKE FAELLT DANACH. StorageAcssRslvTag steht
 * in der Ausschlussliste der View, ueber die der Lauf geht; setzte er sie selbst, aenderte er
 * die Menge, die er durchlaeuft (VIEW_SIGNATURE_TYPE_MUTATED, WRFL_ASE_DEFERRED_DELETION
 * Section 5). StorageStaAcssRslvComponent steht weder in der Signatur noch in der
 * Ausschlussliste und wird im Lauf gesetzt; die aufgeloesten Anfragen haelt ein an
 * STG_ACSS_REQ_BATCH gebundenes Feld, und nach dem Lauf bekommt jede ihre Marke.
 *
 * CAUSAL CHAIN (CHAIN_STORAGE_ACSS_RULE: Visible request → Rule match → Resolution)
 *
 *   StorageAcssRslvSystem (Keycard gueltig, Revier sichtbar)
 *          │
 *          │ StorageReqAcssComponent + StorageAcssRelmTag
 *          ▼
 *   ┌─────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageAcssRuleSystem                         │
 *   │                                                             │
 *   │  READS:                                                     │
 *   │    StorageReqAcssComponent (Revier, Projekt, Pfad)          │
 *   │    StorageAcssRuleComponent + StorageRuleIdnComponent       │
 *   │    StorageAcssIndexResourceManager (Regel-Eimer je Revier)  │
 *   │                                                             │
 *   │  WRITES:                                                    │
 *   │    StorageStaAcssRslvComponent (Schutzstufe, Regel, Etikett)│
 *   │    StorageAcssRslvTag (die Torleiter darf ansetzen)         │
 *   └─────────────────────────────────────────────────────────────┘
 *          │
 *          │ StorageStaAcssRslvComponent + StorageAcssRslvTag
 *          ▼
 *   StorageAcssChkSystem - die Tore
 *
 * HUB Pattern (ARCH_ASE_HUB)
 *
 * READS (from ase-storage internal):
 *   StorageReqAcssComponent  → realm, project and path of the visible request
 *   StorageAcssRuleComponent → the ACL rules of that realm, location and extension buckets
 *   StorageRuleIdnComponent  → match length, match hash and workflow label of each rule
 *
 * WRITES (to ase-storage internal):
 *   StorageStaAcssRslvComponent → protection level, matched rule and its workflow label
 *   StorageAcssRslvTag          → the gate ladder may run on this request
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
 * [ ] NO switch/case statements? (use Tag-filtered Views!)
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
#include <ase/storage/systems/acl/storage_acss_rule_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_req_acss_comp.hpp>
#include <ase/storage/components/state/storage_acss_rule_comp.hpp>
#include <ase/storage/components/state/storage_rule_idn_comp.hpp>
#include <ase/storage/components/state/storage_sta_acss_rslv_comp.hpp>
#include <ase/storage/components/tag/storage_acss_grnt_tag.hpp>
#include <ase/storage/components/tag/storage_acss_deny_tag.hpp>
#include <ase/storage/components/tag/storage_acss_relm_tag.hpp>
#include <ase/storage/components/tag/storage_acss_rslv_tag.hpp>
#include <ase/storage/storage_acss_index_resource_manager.hpp>
#include <ase/storage/storage_acss_ladder.hpp>
#include <ase/storage/types.hpp>
#include <ase/utils/strops.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>

#include <entt/core/hashed_string.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
// NO View/Query operations in helpers! Only pure math!
namespace {

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageAcssRuleSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageAcssRule] Started");
}

void StorageAcssRuleSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto* idx_ptr = registry.ctx().find<StorageAcssIndexResourceManager*>();
    if (!idx_ptr || !(*idx_ptr)) {
        // SCHEDULE_ORDER: der Erzeuger (StorageAcssIdxSystem) hat den ctx-Halter noch nicht angelegt.
        log::error(log::ERR::CAT::SCHEDULE_ORDER, "StorageAcssRuleSystem",
                   "StorageAcssIndexResourceManager");
        return;
    }
    auto& idx = **idx_ptr;

    // The requests this walk resolved, bound to STG_ACSS_REQ_BATCH. When the field is full
    // the walk ends; the remaining requests come back into the view next tick.
    ecs::Entity resolved[STG_ACSS_REQ_BATCH];
    uint32_t resolved_n = 0;

    // Only requests whose keycard and realm stand, and only once: a resolved request carries
    // StorageAcssRslvTag and leaves this view.
    auto req_view = registry.view<StorageReqAcssComponent, StorageAcssRelmTag>(
        entt::exclude<StorageAcssGrntTag, StorageAcssDenyTag, StorageAcssRslvTag>);
    for (auto entity : req_view) {
        if (resolved_n >= STG_ACSS_REQ_BATCH) {
            break;
        }
        auto& req = req_view.get<StorageReqAcssComponent>(entity);

        uint8_t  required_protection = PROTECTION_PUBLIC;
        uint32_t matched_rule = INVALID_ENTITY;
        uint32_t rule_label_hash = 0;
        // The request's own prefixes and extensions, hashed ONCE. Three separate gates
        // in the ladder ask "does this path lie under X" - the ACL rules here, the lattice
        // share and the need-to-know scope there - and all read these same numbers.
        uint32_t pfx_hash[MAX_PATH_LEN + 1] = {};
        uint32_t ext_hash[ACSS_MAX_PATH_PARTS] = {};
        uint32_t ext_len[ACSS_MAX_PATH_PARTS] = {};
        const uint32_t path_len = ase::utils::str_len(req.path, MAX_PATH_LEN);
        path_prefix_hashes(req.path, path_len, pfx_hash);
        const uint32_t ext_n = path_extension_hashes(req.path, path_len, ext_hash,
                                                     ext_len, ACSS_MAX_PATH_PARTS);

        // Pattern semantics (types.hpp ACSS_MATCH_SUFFIX_BONUS): an extension rule
        // ("*.sig" governs companion artifacts that sit BESIDE binaries, where no
        // location can reach them) carries StorageAcssRuleSufxTag; every other rule
        // is a location rule. The MOST SPECIFIC rule wins (extension over location,
        // longer literal over shorter) — never first-iteration order, which is
        // storage-order dependent and would let a broad location rule swallow the
        // "*.sig" codeword requirement.
        //
        // Each rule is one 32-bit equality against the hashes above. The two kinds are
        // matched by two different computations, so they are two loops over two
        // Tag-separated buckets - never one loop that reads a discriminator and branches.
        uint32_t best_score = 0;
        const uint32_t loc_rule_count = idx.get_location_rule_count(req.relm_ref);
        for (uint32_t rule_index = 0; rule_index < loc_rule_count; ++rule_index) {
            const uint32_t acl_id = idx.get_location_rule(req.relm_ref, rule_index);
            if (acl_id == INVALID_ENTITY) { continue; }
            const auto acl_ent = static_cast<ecs::Entity>(acl_id);
            auto* rule_ptr = registry.try_get<StorageAcssRuleComponent>(acl_ent);
            if (rule_ptr == nullptr) { continue; }
            auto& rule = *rule_ptr;
            // rule.relm_ref == req.relm_ref holds by construction of the bucket.
            if (rule.proj_ref != req.proj_ref && rule.proj_ref != 0) { continue; }
            auto* rule_idn = registry.try_get<StorageRuleIdnComponent>(acl_ent);
            if (rule_idn == nullptr) {
                log::error(log::ERR::CAT::COMPONENT_MISSING, "StorageAcssRuleSystem",
                           acl_id, "StorageRuleIdnComponent");
                continue;
            }
            // The path must be at least as long as the pattern and must begin with
            // it: one 32-bit equality against the prefix hash of exactly that length.
            if (rule_idn->match_len < 1u) { continue; }
            if (rule_idn->match_len > path_len) { continue; }
            if (pfx_hash[rule_idn->match_len] != rule_idn->match_hash) { continue; }
            const uint32_t score = 1u + rule_idn->match_len;
            if (score <= best_score) { continue; }
            best_score = score;
            required_protection = rule.protection_level;
            matched_rule = acl_id;
            rule_label_hash = rule_idn->label_hash;
        }

        const uint32_t ext_rule_count = idx.get_extension_rule_count(req.relm_ref);
        for (uint32_t rule_index = 0; rule_index < ext_rule_count; ++rule_index) {
            const uint32_t acl_id = idx.get_extension_rule(req.relm_ref, rule_index);
            if (acl_id == INVALID_ENTITY) { continue; }
            const auto acl_ent = static_cast<ecs::Entity>(acl_id);
            auto* rule_ptr = registry.try_get<StorageAcssRuleComponent>(acl_ent);
            if (rule_ptr == nullptr) { continue; }
            auto& rule = *rule_ptr;
            if (rule.proj_ref != req.proj_ref && rule.proj_ref != 0) { continue; }
            auto* rule_idn = registry.try_get<StorageRuleIdnComponent>(acl_ent);
            if (rule_idn == nullptr) {
                log::error(log::ERR::CAT::COMPONENT_MISSING, "StorageAcssRuleSystem",
                           acl_id, "StorageRuleIdnComponent");
                continue;
            }
            if (rule_idn->match_len < 1u) { continue; }
            bool match = false;
            for (uint32_t i = 0; i < ext_n; ++i) {
                if (ext_hash[i] == rule_idn->match_hash &&
                    ext_len[i] == rule_idn->match_len) { match = true; break; }
            }
            if (!match) { continue; }
            // An extension rule scores on the FULL pattern length, wildcard included,
            // which is one more than the literal it stores.
            const uint32_t score = ACSS_MATCH_SUFFIX_BONUS + rule_idn->match_len + 1u;
            if (score <= best_score) { continue; }
            best_score = score;
            required_protection = rule.protection_level;
            matched_rule = acl_id;
            rule_label_hash = rule_idn->label_hash;
        }

        /**
         * DIE AUFLOESUNG WIRD ZUM ZUSTAND. Drei Zahlen genuegen der Torleiter; alles andere
         * leitet sie aus Dingen ab, die ohnehin auf der Anfrage stehen. Die Komponente steht
         * nicht in der View und wird hier gesetzt; StorageAcssRslvTag faellt nach dem Lauf.
         */
        auto& rslv = registry.emplace<StorageStaAcssRslvComponent>(entity);
        rslv.matched_rule = matched_rule;
        rslv.rule_label_hash = rule_label_hash;
        rslv.required_protection = required_protection;
        resolved[resolved_n] = entity;
        ++resolved_n;
    }

    // NACH DEM LAUF: jede Aufloesung bekommt ihre Marke, die Torleiter darf ansetzen.
    for (uint32_t ri = 0; ri < resolved_n; ++ri) {
        registry.emplace<StorageAcssRslvTag>(resolved[ri]);
    }
}

void StorageAcssRuleSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageAcssRule] Stopped");
}

}  // namespace ase::storage
