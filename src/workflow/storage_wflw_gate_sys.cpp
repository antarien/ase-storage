/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_wflw_gate_sys.cpp
 * @brief       StorageWflwGateSystem - released-gate companion-artifact check
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @created     2026-07-11
 * @modified    2026-07-11
 * @version     1.0.0
 *
 * CAUSAL CHAIN (released-Gate Precondition)
 *
 *   [StorageWflwDrnSystem staged a request targeting "released"]
 *          │
 *          │ Integration schedule runs the Tag-filtered artifact check
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageWflwGateSystem         │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageReqWflwTranComponent            │
 *   │    - StorageWflwPendTag + StorageWflwGateTag│
 *   │    - StorageStaRelmComponent (audit ref)    │
 *   │    - StorageResourceManager (file_exists)   │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - pass: removes StorageWflwGateTag       │
 *   │    - fail: STG_WFLW_RES = DENIED_GATE +     │
 *   │      StorageBufAudtComponent + AudtPendTag  │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Gate passed → transition; failed → request destroyed
 *          ▼
 *   StorageWflwTranSystem (excludes StorageWflwGateTag)
 *
 * HUB Pattern (Active)
 *
 * READS (from Hub):
 *   (none)
 *
 * WRITES (to Hub):
 *   - STG_WFLW_RES (owner = hashed_string(path)): WFLW_RES_DENIED_GATE on fail
 *
 * FLYWEIGHT PATTERN (Active - StorageResourceManager via ctx)
 *   Realm path resolution + artifact existence checks via the manager.
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
#include <ase/storage/systems/workflow/storage_wflw_gate_sys.hpp>
// Components from same module
#include <ase/storage/components/request/storage_req_wflw_tran_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/state/storage_buf_audt_comp.hpp>
#include <ase/storage/components/tag/storage_tag_wflw_pend.hpp>
#include <ase/storage/components/tag/storage_tag_wflw_gate.hpp>
#include <ase/storage/components/tag/storage_tag_audt_pend.hpp>
#include <ase/storage/storage_resource_manager.hpp>
#include <ase/storage/types.hpp>
// Hub API (verdict publish)
#include <ase/hub/api.hpp>
// String ops (L0)
#include <ase/utils/strops.hpp>

#include <entt/core/hashed_string.hpp>
// Logging
#include <ase/log/log.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
namespace {

// Audit record for a gate decision (mirror storage_acss_chk_sys emit_audit):
// one entity per decision, marked pending for the Preservation batch-writer.
void emit_gate_audit(ecs::Registry& registry, uint32_t relm_ref, const char* user_id,
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

// Companion-artifact presence: <asset-abs-path><suffix> must exist. Pure
// string composition + manager query, no views.
bool artifact_present(const StorageResourceManager& mgr, const char* asset_abs,
                      const char* suffix) {
    char art[600] = {};
    ase::utils::str_copy(art, 600, asset_abs);
    ase::utils::str_append(art, 600, suffix);
    return mgr.file_exists(art);
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageWflwGateSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageWflwGate] Started");
}

void StorageWflwGateSystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    auto* mgr_ptr = registry.ctx().find<StorageResourceManager*>();
    if (!mgr_ptr || !(*mgr_ptr)) {
        return;  // manager not up yet (StorageIniSystem seeds it at Initialization)
    }
    auto& mgr = **mgr_ptr;

    // Deferred deletion: gate-failed requests are collected, destroyed after the loop.
    ecs::Entity failed[WFLW_REQ_BATCH];
    uint32_t failed_n = 0;

    auto gate_view = registry.view<StorageReqWflwTranComponent, StorageWflwPendTag,
                                   StorageWflwGateTag>();
    for (auto [req_ent, req] : gate_view.each()) {
        if (failed_n >= WFLW_REQ_BATCH) break;

        // Edge realm entity ref for the audit record (single-pass inline lookup).
        uint32_t relm_ref = 0;
        auto relm_view = registry.view<StorageStaRelmComponent>();
        for (auto [relm_ent, relm] : relm_view.each()) {
            if (ase::utils::str_equal(relm.id, EDGE_REALM_ID, MAX_REALM_ID)) {
                relm_ref = static_cast<uint32_t>(relm_ent);
                break;
            }
        }

        char asset_abs[512] = {};
        mgr.resolve_path(EDGE_REALM_ID, nullptr, req.path, asset_abs, 512);

        // The asset itself plus ALL FOUR companion artifacts must be present.
        // The first missing piece is named in the audit reason (no silent deny).
        const char* missing = nullptr;
        if (!mgr.file_exists(asset_abs)) {
            missing = "asset";
        } else if (!artifact_present(mgr, asset_abs, WFLW_ART_SIG)) {
            missing = WFLW_ART_SIG;
        } else if (!artifact_present(mgr, asset_abs, WFLW_ART_SHA)) {
            missing = WFLW_ART_SHA;
        } else if (!artifact_present(mgr, asset_abs, WFLW_ART_SBOM)) {
            missing = WFLW_ART_SBOM;
        } else if (!artifact_present(mgr, asset_abs, WFLW_ART_SMOKE)) {
            missing = WFLW_ART_SMOKE;
        }

        if (missing == nullptr) {
            // Gate passed: the transition system may now process this request.
            registry.remove<StorageWflwGateTag>(req_ent);
            log::info("[StorageWflwGate] released-gate PASSED for {} (sig+sha256+sbom+smoke present)", req.path);
            continue;
        }

        // Gate failed: verdict + attributed audit + request destroyed.
        const uint32_t owner = entt::hashed_string(req.path).value();
        hub::set(registry, owner, "STG_WFLW_RES"_hs, static_cast<float>(WFLW_RES_DENIED_GATE));

        char reason[MAX_REASON_LEN] = {};
        ase::utils::str_copy(reason, MAX_REASON_LEN, "wflw_gate(");
        ase::utils::str_append(reason, MAX_REASON_LEN, missing);
        ase::utils::str_append(reason, MAX_REASON_LEN, ")");
        emit_gate_audit(registry, relm_ref, req.requested_by, req.path,
                        mgr.get_wall_time_seconds(), AUD_DENIED, reason);

        log::warn("[StorageWflwGate] released-gate DENIED for {} — missing {}", req.path, missing);

        failed[failed_n] = req_ent;
        ++failed_n;
    }

    for (uint32_t i = 0; i < failed_n; ++i) {
        registry.destroy(failed[i]);
    }
}

void StorageWflwGateSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageWflwGate] Stopped");
}

}  // namespace ase::storage
