/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_wflw_tran_sys.cpp
 * @brief       StorageWflwTranSystem - Asset workflow label transitions
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @created     2026-04-05
 * @modified    2026-07-11
 * @version     2.0.0
 *
 * CAUSAL CHAIN (Workflow Transition)
 *
 *   [StorageWflwDrnSystem staged request; StorageWflwGateSystem passed released-gate]
 *          │
 *          │ Integration schedule processes label change
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageWflwTranSystem         │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageReqWflwTranComponent + PendTag  │
 *   │      (excludes StorageWflwGateTag)          │
 *   │    - StorageWflwEdgeComponent (die Kanten)  │
 *   │    - StorageAcssRuleComponent (label)       │
 *   │    - SES_KYCD_PERM (PERM_PROMOTE axis)      │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - StorageAcssRuleComponent.label         │
 *   │    - STG_WFLW_RES / STG_WFLW_STAGE          │
 *   │    - StorageBufAudtComponent (attribution)  │
 *   │    - StorageBufWflwComponent + PstPendTag   │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Label transitioned (draft → review → approved → released → retired)
 *          ▼
 *   StorageAcssChkSystem serves per new label; StorageWflwPstSystem persists (112)
 *
 * HUB Pattern (Active)
 *
 * READS (from Hub):
 *   - SES_KYCD_PERM (owner = hashed_string(requested_by)): PERM_PROMOTE axis
 *
 * WRITES (to Hub):
 *   - STG_WFLW_RES (owner = hashed_string(path)): WFLW_RES_* verdict
 *   - STG_WFLW_STAGE (owner = hashed_string(path)): WFLW_STAGE_* ordinal
 *
 * FLYWEIGHT PATTERN (Active - StorageResourceManager via ctx)
 *   Draft bootstrap checks on-disk asset presence; wall time for audit/persist.
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
#include <ase/storage/systems/workflow/storage_wflw_tran_sys.hpp>
// Components from same module
#include <ase/storage/components/request/storage_req_wflw_tran_comp.hpp>
#include <ase/storage/components/state/storage_wflw_edge_comp.hpp>
#include <ase/storage/components/state/storage_acss_rule_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/state/storage_buf_audt_comp.hpp>
#include <ase/storage/components/state/storage_buf_wflw_comp.hpp>
#include <ase/storage/components/state/storage_wflw_retr_comp.hpp>
#include <ase/storage/components/tag/storage_tag_wflw_pend.hpp>
#include <ase/storage/components/tag/storage_tag_wflw_retr.hpp>
#include <ase/storage/components/tag/storage_tag_wflw_gate.hpp>
#include <ase/storage/components/tag/storage_tag_wflw_pst_pend.hpp>
#include <ase/storage/components/tag/storage_tag_audt_pend.hpp>
#include <ase/storage/storage_resource_manager.hpp>
#include <ase/storage/types.hpp>
// Hub API (perm read + verdict/stage publish)
#include <ase/hub/api.hpp>
// Types (L0 — is_not_found / is_neg_float sentinel checks on hub::get reads)
#include <ase/types/types.hpp>
// String ops (L0)
#include <ase/utils/strops.hpp>

#include <entt/core/hashed_string.hpp>
// Logging
#include <ase/log/log.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
namespace {

// Audit record for a transition decision (mirror storage_acss_chk_sys emit_audit):
// one entity per decision, marked pending for the Preservation batch-writer. The
// requester identity IS the keycard attribution the DoD requires.
void emit_tran_audit(ecs::Registry& registry, uint32_t relm_ref, const char* user_id,
                     const char* path, uint64_t timestamp, uint8_t result,
                     const char* reason) {
    auto aud_ent = registry.create();
    auto& aud = registry.emplace<StorageBufAudtComponent>(aud_ent);
    aud.relm_ref = relm_ref;
    aud.proj_ref = 0;
    ase::utils::str_copy(aud.user_id, MAX_OWNER_ID, user_id);
    aud.action = AUD_PROMOTE;
    ase::utils::str_copy(aud.path, MAX_PATH_LEN, path);
    aud.timestamp = timestamp;
    aud.result = result;
    ase::utils::str_copy(aud.reason, MAX_REASON_LEN, reason);
    registry.emplace<StorageAudtPendTag>(aud_ent);
}

// Bounded "wflw...(from->to)" reason composition. Pure string math, no views.
void compose_edge_reason(char* out, uint32_t out_size, const char* prefix,
                         const char* from_label, const char* to_label) {
    ase::utils::str_copy(out, out_size, prefix);
    ase::utils::str_append(out, out_size, "(");
    ase::utils::str_append(out, out_size, from_label);
    ase::utils::str_append(out, out_size, "->");
    ase::utils::str_append(out, out_size, to_label);
    ase::utils::str_append(out, out_size, ")");
}

// Label → display stage ordinal (WFLW_STAGE_*). Sequential value mapping over
// the fixed label chain (same str_equal ladder the ACL label gate uses).
float stage_ordinal(const char* label) {
    if (ase::utils::str_equal(label, EDGE_LABEL_REVIEW, MAX_LABEL_LEN))   return static_cast<float>(WFLW_STAGE_REVIEW);
    if (ase::utils::str_equal(label, EDGE_LABEL_APPROVED, MAX_LABEL_LEN)) return static_cast<float>(WFLW_STAGE_APPROVED);
    if (ase::utils::str_equal(label, EDGE_LABEL_RELEASED, MAX_LABEL_LEN)) return static_cast<float>(WFLW_STAGE_RELEASED);
    if (ase::utils::str_equal(label, EDGE_LABEL_RETIRED, MAX_LABEL_LEN))  return static_cast<float>(WFLW_STAGE_RETIRED);
    return static_cast<float>(WFLW_STAGE_DRAFT);
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageWflwTranSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageWflwTran] Started");
}

void StorageWflwTranSystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    auto* mgr_ptr = registry.ctx().find<StorageResourceManager*>();
    if (!mgr_ptr || !(*mgr_ptr)) {
        return;  // manager not up yet (StorageIniSystem seeds it at Initialization)
    }
    auto& mgr = **mgr_ptr;

    // Deferred deletion: processed requests are collected, destroyed after the loop.
    ecs::Entity done[WFLW_REQ_BATCH];
    uint32_t done_n = 0;

    // Requests still carrying StorageWflwGateTag belong to StorageWflwGateSystem.
    auto req_view = registry.view<StorageReqWflwTranComponent, StorageWflwPendTag>(
        entt::exclude<StorageWflwGateTag>);
    for (auto [req_ent, req] : req_view.each()) {
        if (done_n >= WFLW_REQ_BATCH) break;

        const uint32_t owner = entt::hashed_string(req.path).value();
        const uint64_t now = mgr.get_wall_time_seconds();

        // Edge realm entity ref (rule scope + audit record).
        uint32_t relm_ref = 0;
        auto relm_view = registry.view<StorageStaRelmComponent>();
        for (auto [relm_ent, relm] : relm_view.each()) {
            if (ase::utils::str_equal(relm.id, EDGE_REALM_ID, MAX_REALM_ID)) {
                relm_ref = static_cast<uint32_t>(relm_ent);
                break;
            }
        }

        // A/ACS permission axis: the requester keycard session must hold
        // PERM_PROMOTE (published owner-scoped by the keycard pipeline). Clearance
        // was gated at the route (operator mint-gate); permission is enforced HERE.
        // A missing key (no live session on this dist) means NO permissions —
        // fail-closed, never a silent grant.
        const uint32_t requester = entt::hashed_string(req.requested_by).value();
        float perm_f = hub::get(registry, requester, "SES_KYCD_PERM"_hs, 0.0f);
        if (ase::types::is_not_found(perm_f)) {
            perm_f = 0.0f;
        }
        if (ase::types::is_neg_float(perm_f)) {
            log::warn(log::WRN::CAT::VALUE_NEGATIVE, "StorageWflwTranSystem", requester,
                      "SES_KYCD_PERM", perm_f);
            perm_f = 0.0f;
        }
        const uint16_t perm = static_cast<uint16_t>(perm_f);
        if ((perm & PERM_PROMOTE) == 0u) {
            hub::set(registry, owner, "STG_WFLW_RES"_hs, static_cast<float>(WFLW_RES_DENIED_PERM));
            emit_tran_audit(registry, relm_ref, req.requested_by, req.path, now,
                            AUD_DENIED, "missing_perm(PROMOTE)");
            log::warn("[StorageWflwTran] DENIED {} -> {} — requester {} lacks PERM_PROMOTE",
                      req.path, req.target_label, req.requested_by);
            done[done_n] = req_ent;
            ++done_n;
            continue;
        }

        // Locate the per-asset ACL rule (EXACT pattern match, realm-scoped). The
        // rule's label field IS the asset's current workflow stage.
        ecs::Entity rule_ent_found = entt::null;
        auto rule_view = registry.view<StorageAcssRuleComponent>();
        for (auto rule_ent : rule_view) {
            auto& r = rule_view.get<StorageAcssRuleComponent>(rule_ent);
            if (r.relm_ref != relm_ref) continue;
            if (!ase::utils::str_equal(r.path_pattern, req.path, MAX_PATH_LEN)) continue;
            rule_ent_found = rule_ent;
            break;
        }

        // No rule yet: an on-disk build without one IS the draft stage — publish
        // deposits builds straight into the realm; the first promote adopts them
        // (data-driven bootstrap, mirror of EDGE_LABEL_DRAFT semantics).
        if (rule_ent_found == entt::null) {
            char asset_abs[512] = {};
            mgr.resolve_path(EDGE_REALM_ID, nullptr, req.path, asset_abs, 512);
            if (!mgr.file_exists(asset_abs)) {
                hub::set(registry, owner, "STG_WFLW_RES"_hs, static_cast<float>(WFLW_RES_NOT_FOUND));
                emit_tran_audit(registry, relm_ref, req.requested_by, req.path, now,
                                AUD_DENIED, "wflw_no_asset");
                log::error("[StorageWflwTran] NOT_FOUND: no ACL rule and no on-disk asset for {}", req.path);
                done[done_n] = req_ent;
                ++done_n;
                continue;
            }
            auto new_rule_ent = registry.create();
            auto& r = registry.emplace<StorageAcssRuleComponent>(new_rule_ent);
            r.relm_ref = relm_ref;
            r.proj_ref = 0;
            ase::utils::str_copy(r.path_pattern, MAX_PATH_LEN, req.path);
            r.protection_level = PROTECTION_PUBLIC;
            ase::utils::str_copy(r.label, MAX_LABEL_LEN, EDGE_LABEL_DRAFT);
            rule_ent_found = new_rule_ent;
            log::info("[StorageWflwTran] Draft rule bootstrapped for on-disk asset {}", req.path);
        }

        auto& rule = registry.get<StorageAcssRuleComponent>(rule_ent_found);

        // Data-driven edge validation (die Kanten): allowed IFF a seeded edge
        // entity matches (from == rule.label && to == request.target). No switch,
        // no if-chain over labels — adding a transition = seeding one entity.
        bool allowed = false;
        auto edge_view = registry.view<StorageWflwEdgeComponent>();
        for (auto [edge_ent, edge] : edge_view.each()) {
            if (ase::utils::str_equal(edge.from_label, rule.label, MAX_LABEL_LEN) &&
                ase::utils::str_equal(edge.to_label, req.target_label, MAX_LABEL_LEN)) {
                allowed = true;
                break;
            }
        }

        char reason[MAX_REASON_LEN] = {};
        if (!allowed) {
            hub::set(registry, owner, "STG_WFLW_RES"_hs, static_cast<float>(WFLW_RES_DENIED_EDGE));
            compose_edge_reason(reason, MAX_REASON_LEN, "wflw_edge", rule.label, req.target_label);
            emit_tran_audit(registry, relm_ref, req.requested_by, req.path, now,
                            AUD_DENIED, reason);
            log::warn("[StorageWflwTran] DENIED edge {} -> {} for {} (by {})",
                      rule.label, req.target_label, req.path, req.requested_by);
            done[done_n] = req_ent;
            ++done_n;
            continue;
        }

        // APPLY: write the new label, publish verdict + stage, audit with the
        // keycard attribution, stage the durable frame-112 persist buffer.
        compose_edge_reason(reason, MAX_REASON_LEN, "wflw", rule.label, req.target_label);
        ase::utils::str_copy(rule.label, MAX_LABEL_LEN, req.target_label);

        hub::set(registry, owner, "STG_WFLW_RES"_hs, static_cast<float>(WFLW_RES_APPLIED));
        hub::set(registry, owner, "STG_WFLW_STAGE"_hs, stage_ordinal(rule.label));
        // Public-servable verdict — the SINGLE place this policy lives (ARCH_ASE_REASONING_EDGE §6.4:
        // download-access runs over the ase-storage A/ACS infrastructure). A customer download is
        // public ONLY at the released stage (EDGE_LABEL_RELEASED = "Public download", types.hpp). The
        // edge-webserver serving gate READS this boolean; it never re-decides the label policy itself.
        hub::set(registry, owner, "STG_WFLW_PUB"_hs,
                 ase::utils::str_equal(rule.label, EDGE_LABEL_RELEASED, MAX_LABEL_LEN) ? 1.0f : 0.0f);

        emit_tran_audit(registry, relm_ref, req.requested_by, req.path, now,
                        AUD_GRANTED, reason);

        auto buf_ent = registry.create();
        auto& buf = registry.emplace<StorageBufWflwComponent>(buf_ent);
        ase::utils::str_copy(buf.realm, MAX_REALM_ID, EDGE_REALM_ID);
        ase::utils::str_copy(buf.path, MAX_PATH_LEN, req.path);
        ase::utils::str_copy(buf.label, MAX_LABEL_LEN, rule.label);
        ase::utils::str_copy(buf.updated_by, MAX_OWNER_ID, req.requested_by);
        buf.updated_at = now;
        registry.emplace<StorageWflwPstPendTag>(buf_ent);

        // A transition INTO retired starts the retention clock: one record entity
        // per retired build (Entity-per-Item); StorageWflwClnSystem deletes the
        // files once WFLW_RETIRED_RETENTION_S elapses.
        if (ase::utils::str_equal(rule.label, EDGE_LABEL_RETIRED, MAX_LABEL_LEN)) {
            auto retr_ent = registry.create();
            auto& retr = registry.emplace<StorageWflwRetrComponent>(retr_ent);
            retr.rule_ref = static_cast<uint32_t>(rule_ent_found);
            ase::utils::str_copy(retr.path, MAX_PATH_LEN, req.path);
            retr.retired_at = now;
            registry.emplace<StorageWflwRetrTag>(retr_ent);
        }

        log::info("[StorageWflwTran] APPLIED {} for {} (by {})", reason, req.path, req.requested_by);

        done[done_n] = req_ent;
        ++done_n;
    }

    for (uint32_t i = 0; i < done_n; ++i) {
        registry.destroy(done[i]);
    }
}

void StorageWflwTranSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageWflwTran] Stopped");
}

}  // namespace ase::storage
