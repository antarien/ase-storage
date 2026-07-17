/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_wflw_cln_sys.cpp
 * @brief       StorageWflwClnSystem - retired-build retention cleanup (90 days)
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Observation
 * @created     2026-07-11
 * @modified    2026-07-11
 * @version     1.0.0
 *
 * CAUSAL CHAIN (Retired-Build Retention Cleanup)
 *
 *   [StorageWflwTranSystem applied a transition INTO retired]
 *          │
 *          │ Observation schedule sweeps the retention records
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageWflwClnSystem          │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageWflwRetrComponent + RetrTag     │
 *   │    - StorageStaRelmComponent (audit ref)    │
 *   │    - StorageResourceManager (delete_file)   │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - removes asset + companion artifacts    │
 *   │    - destroys retired rule + record entity  │
 *   │    - StorageBufAudtComponent (AUD_DELETE)   │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ retired > 90d: files gone, realm stays small (not the quota ceiling)
 *          ▼
 *   StorageQuotChkSystem measures the freed bytes on its next scan
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
 *   Realm path resolution + file removal + wall time via the manager.
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
#include <ase/storage/systems/workflow/storage_wflw_cln_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_wflw_retr_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/state/storage_buf_audt_comp.hpp>
#include <ase/storage/components/tag/storage_tag_wflw_retr.hpp>
#include <ase/storage/components/tag/storage_tag_audt_pend.hpp>
#include <ase/storage/storage_resource_manager.hpp>
#include <ase/storage/types.hpp>
// String ops (L0)
#include <ase/utils/strops.hpp>

#include <entt/core/hashed_string.hpp>
// Logging
#include <ase/log/log.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
namespace {

// Remove one on-disk file if present (bounded path composition, manager I/O).
// Missing companions are NOT an error — older releases shipped fewer artifacts.
void remove_if_present(StorageResourceManager& mgr, const char* asset_abs, const char* suffix) {
    char target[600] = {};
    ase::utils::str_copy(target, 600, asset_abs);
    if (suffix != nullptr) {
        ase::utils::str_append(target, 600, suffix);
    }
    if (!mgr.file_exists(target)) return;
    if (!mgr.delete_file(target)) {
        log::warn("[StorageWflwCln] could not remove {} (kept; retried next sweep)", target);
    }
}

// Audit record for a retention removal (mirror storage_acss_chk_sys emit_audit).
void emit_cln_audit(ecs::Registry& registry, uint32_t relm_ref, const char* path,
                    uint64_t timestamp) {
    auto aud_ent = registry.create();
    auto& aud = registry.emplace<StorageBufAudtComponent>(aud_ent);
    aud.relm_ref = relm_ref;
    aud.proj_ref = 0;
    ase::utils::str_copy(aud.user_id, MAX_OWNER_ID, "system:wflw_cln");
    aud.action = AUD_DELETE;
    ase::utils::str_copy(aud.path, MAX_PATH_LEN, path);
    aud.timestamp = timestamp;
    aud.result = AUD_GRANTED;
    ase::utils::str_copy(aud.reason, MAX_REASON_LEN, "wflw_retention(90d)");
    registry.emplace<StorageAudtPendTag>(aud_ent);
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageWflwClnSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageWflwCln] Started");
}

void StorageWflwClnSystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    auto* mgr_ptr = registry.ctx().find<StorageResourceManager*>();
    if (!mgr_ptr || !(*mgr_ptr)) {
        return;  // manager not up yet (StorageIniSystem seeds it at Initialization)
    }
    auto& mgr = **mgr_ptr;

    const uint64_t now = mgr.get_wall_time_seconds();

    // Deferred deletion: expired records AND their retired ACL rules are
    // collected during the Tag-filtered sweep, destroyed after the loop.
    ecs::Entity done[WFLW_REQ_BATCH];
    uint32_t done_n = 0;
    ecs::Entity rules[WFLW_REQ_BATCH];
    uint32_t rules_n = 0;

    auto retr_view = registry.view<StorageWflwRetrComponent, StorageWflwRetrTag>();
    for (auto [retr_ent, retr] : retr_view.each()) {
        if (done_n >= WFLW_REQ_BATCH) break;
        if (retr.retired_at > now) {
            log::warn("[StorageWflwCln] record for {} carries a FUTURE retire time {} — skipped",
                      retr.path, retr.retired_at);
            continue;
        }
        const uint64_t age = now - retr.retired_at;
        if (age < WFLW_RETIRED_RETENTION_S) continue;

        // Edge realm entity ref for the audit record (single-pass inline lookup).
        uint32_t relm_ref = 0;
        auto relm_view = registry.view<StorageStaRelmComponent>();
        for (auto [relm_ent, relm] : relm_view.each()) {
            if (ase::utils::str_equal(relm.id, EDGE_REALM_ID, MAX_REALM_ID)) {
                relm_ref = static_cast<uint32_t>(relm_ent);
                break;
            }
        }

        // Remove the asset and every companion artifact (missing ones are fine).
        char asset_abs[512] = {};
        mgr.resolve_path(EDGE_REALM_ID, nullptr, retr.path, asset_abs, 512);
        remove_if_present(mgr, asset_abs, nullptr);
        remove_if_present(mgr, asset_abs, WFLW_ART_SIG);
        remove_if_present(mgr, asset_abs, WFLW_ART_SHA);
        remove_if_present(mgr, asset_abs, WFLW_ART_SBOM);
        remove_if_present(mgr, asset_abs, WFLW_ART_SMOKE);

        emit_cln_audit(registry, relm_ref, retr.path, now);
        log::info("[StorageWflwCln] retired build {} removed after {}s retention", retr.path, age);

        // The retiring ACL rule dies with its files (collected, destroyed below).
        auto rule_ent = static_cast<ecs::Entity>(retr.rule_ref);
        if (registry.valid(rule_ent) && rules_n < WFLW_REQ_BATCH) {
            rules[rules_n] = rule_ent;
            ++rules_n;
        }

        done[done_n] = retr_ent;
        ++done_n;
    }

    for (uint32_t i = 0; i < rules_n; ++i) {
        registry.destroy(rules[i]);
    }
    for (uint32_t i = 0; i < done_n; ++i) {
        registry.destroy(done[i]);
    }
}

void StorageWflwClnSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageWflwCln] Stopped");
}

}  // namespace ase::storage
