/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_acss_pol_sys.cpp
 * @brief       StorageAcssPolSystem - the policy gates of the A/ACS ladder (steps 7-10)
 * @description Weighs the SUBJECT of a request whose key gates held (StorageAcssPassTag): the
 *              workflow label, the running need-to-know task and the realm quota of a write.
 *              The license tier codes the quota step compares against are the hub rows
 *              ENG_CONST_TIER_PRO and ENG_CONST_TIER_ENTERPRISE, read on GLOBAL in tick.
 *              Grants exactly here, on step 10, and files one audit row per request.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @design      DSGN_021
 * @usecase     usecase:100265 - check access clearance
 * @actor       Provider Credential Keeper (PROVCREKEE)
 * @created     2026-08-31
 * @modified    2026-11-16
 * @version     00.00.39.00039 [core]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * TWO QUESTIONS, TWO FILES
 *
 * The gate ladder weighs two things. WHO MAY (credential, realm, lattice, protection level,
 * codeword, permission) hangs on the CALLER and changes with his keycard; StorageAcssChkSystem
 * answers it. WHAT HOLDS FOR THIS SUBJECT (label, running task, quota) hangs on the ASSET and
 * changes with the state of work; this system answers it. A change to the quota tariff, such as
 * a new tier, therefore never touches the file that holds the codeword check.
 *
 * THE LADDER STAYS STRICTLY ORDERED AND GRANTS ONLY ON STEP 10. What arrives here held steps 1
 * to 6 and carries StorageAcssPassTag.
 *
 * THE WALK DECIDES, THE MARKS FALL AFTER IT. Both marks this system sets - StorageAcssGrntTag
 * and StorageAcssDenyTag - stand in the exclude list of the view it walks. Setting them inside
 * the walk would change the set it iterates (VIEW_SIGNATURE_TYPE_MUTATED,
 * WRFL_ASE_DEFERRED_DELETION Section 5). The walk files each audit row - its own entity, outside
 * the view - and records the decision in two fields bound to STG_ACSS_REQ_BATCH; after the walk
 * one block per field sets the mark. A request beyond the bound stays undecided and comes back
 * into the view next tick.
 *
 * WHAT THIS SYSTEM DERIVES ITSELF. It needs four values of the caller side: the owner preset,
 * the effective clearance, the user hash and the prefix hashes of the path. It derives them from
 * the same inputs with the same pure forms (storage_acss_ladder.hpp). `required_perm` and the
 * effective permissions are NOT derived: they belong to the key gates and no gate here reads them.
 *
 * WHY THE TIER CODES COME FROM THE HUB. The license tier is an engine-wide value with one owner,
 * the hub rows ENG_CONST_TIER_*; a module-local constant with the same number would be a second
 * place where it can be wrong. When a row is missing or out of range, the quota step cannot tell
 * the tariff of the realm, so a write is refused fail-closed with "tier_constant_missing".
 *
 * CAUSAL CHAIN (CHAIN_STORAGE_ACSS_POL: key gates held → grant or deny)
 *
 *   StorageAcssChkSystem (steps 1-6 held)
 *          │
 *          │ StorageAcssPassTag
 *          ▼
 *   ┌─────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageAcssPolSystem                          │
 *   │                                                             │
 *   │  READS:                                                     │
 *   │    StorageStaAcssRslvComponent (label, protection level)    │
 *   │    StorageAcssOwnrTag (owner preset)                        │
 *   │    StorageReqAcssComponent + StorageReqCredComponent        │
 *   │    StorageStaRelmComponent (realm id and license tier)      │
 *   │    StorageStaTaskComponent + StorageTaskIdnComponent        │
 *   │    ENG_CONST_TIER_PRO / _ENTERPRISE (Hub, GLOBAL)           │
 *   │                                                             │
 *   │  WRITES:                                                    │
 *   │    StorageAcssGrntTag or StorageAcssDenyTag                 │
 *   │    one audit row per request (StorageBufAudtComponent)      │
 *   └─────────────────────────────────────────────────────────────┘
 *          │
 *          │ request entity tagged with grant or deny
 *          ▼
 *   StorageFileWritSystem processes granted requests
 *
 * Every edge that waits for the result of the ladder points at THIS system
 * (StorageFileWritSystem, StorageCncmFltSystem, StorageCurPrcSystem): an edge that stopped at
 * StorageAcssChkSystem would see zero grants and silently do nothing.
 *
 * HUB Pattern (ARCH_ASE_HUB)
 *
 * READS (Hub, GLOBAL):
 *   ENG_CONST_TIER_PRO          → the license tier code of the paid tariff
 *   ENG_CONST_TIER_ENTERPRISE   → the license tier code of the dedicated tariff
 *
 * READS (from ase-storage internal):
 *   StorageStaAcssRslvComponent → label and protection level of the resolved rule
 *   StorageReqCredComponent     → the caller's keycard, unless the owner preset applies
 *   StorageStaRelmComponent     → realm id and license tier for the quota limit
 *
 * WRITES (to ase-storage internal):
 *   StorageAcssGrntTag / StorageAcssDenyTag → the decision, exactly one per request
 *   StorageBufAudtComponent                 → the audit row of this decision
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
#include <ase/storage/systems/acl/storage_acss_pol_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_req_acss_comp.hpp>
#include <ase/storage/components/state/storage_req_cred_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/state/storage_sta_acss_rslv_comp.hpp>
#include <ase/storage/components/state/storage_sta_task_comp.hpp>
#include <ase/storage/components/state/storage_task_idn_comp.hpp>
// The audit row is filed in THIS body, not by a helper that takes ecs::Registry&.
#include <ase/storage/components/state/storage_buf_audt_comp.hpp>
#include <ase/storage/components/state/storage_audt_outc_comp.hpp>
#include <ase/storage/components/tag/storage_audt_pend_tag.hpp>
#include <ase/storage/components/tag/storage_acss_grnt_tag.hpp>
#include <ase/storage/components/tag/storage_acss_deny_tag.hpp>
#include <ase/storage/components/tag/storage_acss_pass_tag.hpp>
#include <ase/storage/components/tag/storage_acss_ownr_tag.hpp>
#include <ase/storage/storage_resource_manager.hpp>
#include <ase/storage/storage_acss_index_resource_manager.hpp>
#include <ase/storage/storage_acss_ladder.hpp>
#include <ase/storage/types.hpp>
// Hub (ENG_CONST_TIER_* on GLOBAL)
#include <ase/hub/api.hpp>
// Types SSOT (is_not_found / is_in_rng_f on hub::get reads)
#include <ase/types/types.hpp>
#include <ase/utils/strops.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>

#include <entt/core/hashed_string.hpp>

namespace ase::storage {

using namespace entt::literals;

// Anonymous namespace for helper FUNCTIONS (NOT static!)
// NO View/Query operations in helpers! Only pure math!
namespace {

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageAcssPolSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageAcssPol] Started");
}

void StorageAcssPolSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto* mgr_ptr = registry.ctx().find<StorageResourceManager*>();
    if (!mgr_ptr || !(*mgr_ptr)) {
        return;
    }
    auto& mgr = **mgr_ptr;
    uint64_t now = mgr.get_wall_time_seconds();

    auto* idx_ptr = registry.ctx().find<StorageAcssIndexResourceManager*>();
    if (!idx_ptr || !(*idx_ptr)) {
        // SCHEDULE_ORDER: the producer (StorageAcssIdxSystem) has not laid the ctx holder yet.
        log::error(log::ERR::CAT::SCHEDULE_ORDER, "StorageAcssPolSystem",
                   "StorageAcssIndexResourceManager");
        return;
    }
    auto& idx = **idx_ptr;

    // The license tier codes of the two paid tariffs, read once per tick. A tier is a byte on
    // the realm record, so a code outside 0..UINT8_MAX could never match one.
    const float byte_max = static_cast<float>(UINT8_MAX);
    bool tier_ready = true;
    const float tier_pro_f = hub::get(registry, hub::GLOBAL, "ENG_CONST_TIER_PRO"_hs);
    if (ase::types::is_not_found(tier_pro_f)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageAcssPolSystem", "ENG_CONST_TIER_PRO");
        tier_ready = false;
    }
    if (tier_ready && !ase::types::is_in_rng_f(tier_pro_f, 0.0f, byte_max)) {
        log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "StorageAcssPolSystem", hub::GLOBAL,
                  "ENG_CONST_TIER_PRO", tier_pro_f, 0.0f, byte_max);
        tier_ready = false;
    }
    const float tier_ent_f = hub::get(registry, hub::GLOBAL, "ENG_CONST_TIER_ENTERPRISE"_hs);
    if (ase::types::is_not_found(tier_ent_f)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageAcssPolSystem", "ENG_CONST_TIER_ENTERPRISE");
        tier_ready = false;
    }
    if (tier_ready && !ase::types::is_in_rng_f(tier_ent_f, 0.0f, byte_max)) {
        log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "StorageAcssPolSystem", hub::GLOBAL,
                  "ENG_CONST_TIER_ENTERPRISE", tier_ent_f, 0.0f, byte_max);
        tier_ready = false;
    }
    const uint8_t tier_pro = tier_ready ? static_cast<uint8_t>(tier_pro_f) : 0u;
    const uint8_t tier_ent = tier_ready ? static_cast<uint8_t>(tier_ent_f) : 0u;

    // The owner classification as a tag-filtered view, built ONCE. Membership is then an O(1)
    // test on the filtered set, never an all_of<Tag> in the body (WRFL_ASE_TAGGED_VIEWS), the
    // same form the key gates before it use.
    auto own_view = registry.view<StorageAcssOwnrTag>();

    // What this walk decided, bound to STG_ACSS_REQ_BATCH. The marks fall after the walk.
    ecs::Entity granted[STG_ACSS_REQ_BATCH];
    uint32_t granted_n = 0;
    ecs::Entity denied[STG_ACSS_REQ_BATCH];
    uint32_t denied_n = 0;

    // Only requests whose KEY gates held and which are still undecided. The tag lets nothing
    // else in: what failed on steps 1 to 6 carries StorageAcssDenyTag and drops out at the
    // exclude list.
    auto req_view = registry.view<StorageReqAcssComponent, StorageReqCredComponent,
                                  StorageStaAcssRslvComponent, StorageAcssPassTag>(
        entt::exclude<StorageAcssGrntTag, StorageAcssDenyTag>);
    for (auto entity : req_view) {
        if (granted_n >= STG_ACSS_REQ_BATCH || denied_n >= STG_ACSS_REQ_BATCH) {
            break;
        }
        auto& req  = req_view.get<StorageReqAcssComponent>(entity);
        auto& cred = req_view.get<StorageReqCredComponent>(entity);
        auto& rslv = req_view.get<StorageStaAcssRslvComponent>(entity);

        const bool owner_preset = own_view.contains(entity);

        // Effective clearance. The owner preset is the in-ladder model of former realm_owner
        // power: clearance 9. Without the preset, the auth-header value is used verbatim.
        // The effective permissions are NOT derived here: no gate of this pass reads them.
        const uint8_t eff_clrn = owner_preset ? ACSS_OWNER_CLEARANCE : cred.clrn;

        // THE AUDIT ROW IS FILED ONCE PER REQUEST, NOT ONCE PER EXIT. Its request-derived half -
        // realm, project, user, path, timestamp - is the same at EVERY exit; only result and
        // reason differ. Every request of this pass reaches exactly one exit, so exactly one row
        // is filed, and a future sixth field cannot be missing at one of six exits. The row is
        // its own entity outside the walked view, so it is filed inside the walk.
        auto aud_ent = registry.create();
        auto& aud = registry.emplace<StorageBufAudtComponent>(aud_ent);
        aud.relm_ref = req.relm_ref;
        aud.proj_ref = req.proj_ref;
        ase::utils::str_copy(aud.user_id, MAX_OWNER_ID, cred.user_id);
        ase::utils::str_copy(aud.path, MAX_PATH_LEN, req.path);
        aud.timestamp = now;
        auto& outc = registry.emplace<StorageAudtOutcComponent>(aud_ent);
        outc.action = req.action;
        outc.result = AUD_DENIED;
        registry.emplace<StorageAudtPendTag>(aud_ent);

        // Realm id and license tier stand on the realm record, not in the resolution component.
        // When the row vanishes between the systems, access falls fail-closed and says no more
        // than it knows.
        const auto relm_ent = static_cast<ecs::Entity>(req.relm_ref);
        auto* relm_ptr = registry.try_get<StorageStaRelmComponent>(relm_ent);
        if (relm_ptr == nullptr) {
            log::error(log::ERR::CAT::COMPONENT_MISSING, "StorageAcssPolSystem",
                       req.relm_ref, "StorageStaRelmComponent");
            ase::utils::str_copy(outc.reason, MAX_REASON_LEN, "realm_not_found");
            denied[denied_n++] = entity;
            continue;
        }
        char target_id[MAX_REALM_ID] = {};
        ase::utils::str_copy(target_id, MAX_REALM_ID, relm_ptr->id);
        const uint8_t target_tier = relm_ptr->tier;

        // Identity is a lookup, and a lookup compares hashes, never characters
        // (WRFL_ASE_STRING_HANDLING Section 3). Hashed ONCE per request, like in the key gates.
        const uint32_t user_hash = entt::hashed_string(cred.user_id).value();

        // The prefix hashes of the path, once per request: the need-to-know step below asks
        // "does this path lie under X" and reads the same numbers (storage_acss_ladder.hpp).
        uint32_t pfx_hash[MAX_PATH_LEN + 1] = {};
        const uint32_t path_len = ase::utils::str_len(req.path, MAX_PATH_LEN);
        path_prefix_hashes(req.path, path_len, pfx_hash);

        //Step 7: LABEL ─ workflow-status gate.
        // retired = withdrawn build (no access); draft/review = team-only (clearance >= TEAM).
        if (rslv.rule_label_hash == EDGE_LABEL_RETIRED_HASH) {
            ase::utils::str_copy(outc.reason, MAX_REASON_LEN, "retired_asset");
            denied[denied_n++] = entity;
            continue;
        }
        if ((rslv.rule_label_hash == EDGE_LABEL_DRAFT_HASH ||
             rslv.rule_label_hash == EDGE_LABEL_REVIEW_HASH) &&
            eff_clrn < PROTECTION_TEAM) {
            ase::utils::str_copy(outc.reason, MAX_REASON_LEN, "label_restricted");
            denied[denied_n++] = entity;
            continue;
        }

        //Step 8: NEED-TO-KNOW ─ active task scoping (Enterprise).
        // When the assignee has any active need-to-know task in this project, access is
        // restricted to that task's path scope. Owner-preset governs all and is exempt.
        if (!owner_preset && req.proj_ref != 0) {
            bool has_active_task = false;
            bool path_in_scope   = false;
            // Tasks are indexed under their project, so the project filter is the bucket
            // itself and only the assignee still has to be compared.
            const uint32_t task_count = idx.get_task_count(req.proj_ref);
            for (uint32_t task_index = 0; task_index < task_count; ++task_index) {
                const uint32_t task_id = idx.get_task(req.proj_ref, task_index);
                if (task_id == INVALID_ENTITY) { continue; }
                auto* task_ptr =
                    registry.try_get<StorageStaTaskComponent>(static_cast<ecs::Entity>(task_id));
                if (task_ptr == nullptr) { continue; }
                auto& task = *task_ptr;
                auto* task_idn =
                    registry.try_get<StorageTaskIdnComponent>(static_cast<ecs::Entity>(task_id));
                if (task_idn == nullptr) {
                    log::error(log::ERR::CAT::COMPONENT_MISSING, "StorageAcssPolSystem",
                               task_id, "StorageTaskIdnComponent");
                    continue;
                }
                if (task_idn->assignee_hash != user_hash) { continue; }
                bool live = (task.starts_at == 0 || task.starts_at <= now) &&
                            (task.expires_at == 0 || task.expires_at > now);
                if (!live) { continue; }
                has_active_task = true;
                if (task_idn->scope_len < 1u) { continue; }
                if (task_idn->scope_len > path_len) { continue; }
                if (pfx_hash[task_idn->scope_len] == task_idn->scope_hash) {
                    path_in_scope = true;
                    break;
                }
            }
            if (has_active_task && !path_in_scope) {
                ase::utils::str_copy(outc.reason, MAX_REASON_LEN, "need_to_know");
                denied[denied_n++] = entity;
                continue;
            }
        }

        //Step 9: QUOTA ─ realm storage budget gate (WRITE only).
        // A write is refused when the realm's measured usage already meets its tier limit. Without
        // the tier codes of the hub the tariff of the realm is unknown, and the write is refused.
        if (req.action == AUD_WRITE && !tier_ready) {
            ase::utils::str_copy(outc.reason, MAX_REASON_LEN, "tier_constant_missing");
            denied[denied_n++] = entity;
            continue;
        }
        if (req.action == AUD_WRITE) {
            uint64_t tier_limit = QUOTA_INDIE_STORAGE;
            if (target_tier == tier_pro) { tier_limit = QUOTA_PRO_STORAGE; }
            if (target_tier == tier_ent) { tier_limit = QUOTA_ENT_STORAGE; }
            uint64_t used = mgr.get_realm_usage(target_id);
            if (used >= tier_limit) {
                ase::utils::str_copy(outc.reason, MAX_REASON_LEN, "quota_exceeded");
                denied[denied_n++] = entity;
                continue;
            }
        }

        //Step 10: GRANT + AUDIT(GRANTED) ─ reached only after every applicable step passed
        outc.result = AUD_GRANTED;
        ase::utils::str_copy(outc.reason, MAX_REASON_LEN, "");
        granted[granted_n++] = entity;
    }

    // AFTER THE WALK: the decision marks. Neither is in the signature of any view still open.
    for (uint32_t gi = 0; gi < granted_n; ++gi) {
        registry.emplace<StorageAcssGrntTag>(granted[gi]);
    }
    for (uint32_t di = 0; di < denied_n; ++di) {
        registry.emplace<StorageAcssDenyTag>(denied[di]);
    }
}

void StorageAcssPolSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageAcssPol] Stopped");
}

}  // namespace ase::storage
