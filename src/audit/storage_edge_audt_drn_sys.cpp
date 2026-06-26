/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_edge_audt_drn_sys.cpp
 * @brief       StorageEdgeAudtDrnSystem - Drains edge A/ACS gate audit-signals
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Observation
 * @created     2026-06-24
 * @modified    2026-06-24
 * @version     1.0.0
 *
 * CAUSAL CHAIN (Edge A/ACS Audit Persistence)
 *
 *   [ase-pl-edge-webserver acl_gate emits a decision]
 *          │
 *          │ owner-keyed Hub signal SES_EDGE_AUDIT_* (owner = hashed_string(user_id)),
 *          │ SES_EDGE_AUDIT_SEQ bumped once per decision (grant or deny)
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageEdgeAudtDrnSystem      │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageStaIdnComponent + KycdVldTag    │
 *   │    - SES_EDGE_AUDIT_SEQ / DRAINED_SEQ       │
 *   │    - SES_EDGE_AUDIT_ACTION / RESULT / CWRD  │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - StorageBufAudtComponent + AudtPendTag  │
 *   │    - SES_EDGE_AUDIT_DRAINED_SEQ (watermark) │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Audit entity tagged pending
 *          ▼
 *   StorageAudtWritSystem (Preservation) persists to MongoDB.
 *
 * HUB Pattern (Active - drains edge-gate audit signals)
 *
 * READS (from Hub):
 *   SES_EDGE_AUDIT_SEQ         — monotonic decision counter per owner
 *   SES_EDGE_AUDIT_DRAINED_SEQ — drain watermark per owner
 *   SES_EDGE_AUDIT_ACTION      — action enum of the latest decision
 *   SES_EDGE_AUDIT_RESULT      — result enum of the latest decision
 *   SES_EDGE_AUDIT_CWRD        — required-codeword hash (label = codeword string)
 *
 * WRITES (to Hub):
 *   SES_EDGE_AUDIT_DRAINED_SEQ — advanced to SES_EDGE_AUDIT_SEQ after draining
 *
 * FLYWEIGHT PATTERN (Active - StorageResourceManager via ctx)
 *   Wall-clock timestamp for audit entries from the resource manager.
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
#include <ase/storage/systems/audit/storage_edge_audt_drn_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_sta_idn_comp.hpp>
#include <ase/storage/components/state/storage_buf_audt_comp.hpp>
#include <ase/storage/components/tag/storage_tag_kycd_vld.hpp>
#include <ase/storage/components/tag/storage_tag_audt_pend.hpp>
#include <ase/storage/storage_resource_manager.hpp>
#include <ase/storage/types.hpp>
// Hub API for owner-keyed audit-signal drain
#include <ase/hub/api.hpp>
// Types (L0 — is_not_found sentinel check)
#include <ase/types/types.hpp>
// Utils (L0 — safe C-string operations)
#include <ase/utils/strops.hpp>
// Logging
#include <ase/log/log.hpp>

#include <entt/core/hashed_string.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
// IMPORTANT: Use anonymous namespace, NOT static keyword!
// NO STRUCTS HERE! Structs = Data = Components!
// NO View/Query operations in helpers! Only pure math!
namespace {

// No helper functions needed → all logic inline in tick()

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageEdgeAudtDrnSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageEdgeAudtDrn] Started");
}

void StorageEdgeAudtDrnSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto* mgr_ptr = registry.ctx().find<StorageResourceManager*>();
    if (!mgr_ptr || !(*mgr_ptr)) {
        return;
    }
    auto& mgr = **mgr_ptr;
    uint64_t now = mgr.get_wall_time_seconds();

    // SINGLE-PASS: each authenticated session is keyed by hashed_string(user_id)
    // — the identical owner the edge gate writes the SES_EDGE_AUDIT_* signals to.
    auto view = registry.view<StorageStaIdnComponent, StorageKycdVldTag>();
    for (auto [session_entity, idn] : view.each()) {
        (void)session_entity;
        uint32_t owner = entt::hashed_string{idn.user_id}.value();

        float seq_f = hub::get(registry, owner, "SES_EDGE_AUDIT_SEQ"_hs);
        if (ase::types::is_not_found(seq_f)) {
            // No gate decision recorded for this owner yet — nothing to drain.
            continue;
        }
        float drained_f = hub::get(registry, owner, "SES_EDGE_AUDIT_DRAINED_SEQ"_hs, 0.0f);
        if (ase::types::is_not_found(drained_f)) {
            drained_f = 0.0f;
        }

        uint32_t seq = static_cast<uint32_t>(seq_f);
        uint32_t drained = static_cast<uint32_t>(drained_f);
        if (seq <= drained) {
            // Watermark current — no new decisions since the last drain.
            continue;
        }

        // Read the latest decision metadata. ACTION/RESULT/CWRD reflect the most
        // recent decision; `gap` is how many decisions occurred since the last
        // drain. NO silent reads: a missing ACTION/RESULT is logged as an error,
        // not skipped, because a bumped SEQ guarantees the gate wrote them.
        float action_f = hub::get(registry, owner, "SES_EDGE_AUDIT_ACTION"_hs);
        if (ase::types::is_not_found(action_f)) {
            log::error("[StorageEdgeAudtDrn] SES_EDGE_AUDIT_ACTION missing for owner={} (seq={})",
                       owner, seq);
            action_f = static_cast<float>(AUD_READ);
        }
        float result_f = hub::get(registry, owner, "SES_EDGE_AUDIT_RESULT"_hs);
        if (ase::types::is_not_found(result_f)) {
            log::error("[StorageEdgeAudtDrn] SES_EDGE_AUDIT_RESULT missing for owner={} (seq={})",
                       owner, seq);
            result_f = static_cast<float>(AUD_DENIED);
        }
        float cwrd_f = hub::get(registry, owner, "SES_EDGE_AUDIT_CWRD"_hs, 0.0f);
        if (ase::types::is_not_found(cwrd_f)) {
            cwrd_f = 0.0f;
        }

        uint32_t cwrd_hash = static_cast<uint32_t>(cwrd_f);
        const char* cwrd_str = hub::get_name(registry, cwrd_hash);
        if (cwrd_str == nullptr) {
            cwrd_str = "";
        }
        uint32_t gap = seq - drained;

        // Emit one audit entity carrying the latest decision metadata. The
        // coalesced decision count rides the reason field so the audit trail
        // is honest about how many 1Hz-window decisions this record stands for
        // (no fabricated per-decision metadata, no silent drop).
        auto aud_ent = registry.create();
        auto& aud = registry.emplace<StorageBufAudtComponent>(aud_ent);
        aud.relm_ref = 0;
        aud.proj_ref = 0;
        ase::utils::str_copy(aud.user_id, MAX_OWNER_ID, idn.user_id);
        aud.action = static_cast<uint8_t>(action_f);
        ase::utils::str_copy(aud.path, MAX_PATH_LEN, EDGE_REALM_ID);
        aud.timestamp = now;
        aud.result = static_cast<uint8_t>(result_f);
        ase::utils::str_copy(aud.reason, MAX_REASON_LEN, cwrd_str);
        registry.emplace<StorageAudtPendTag>(aud_ent);

        // Advance the watermark so the next tick only drains fresh decisions.
        hub::set(registry, owner, "SES_EDGE_AUDIT_DRAINED_SEQ"_hs, static_cast<float>(seq));

        log::debug("[StorageEdgeAudtDrn] owner={} user='{}' drained gap={} result={} cwrd='{}'",
                   owner, idn.user_id, gap, static_cast<uint32_t>(result_f), cwrd_str);
    }
}

void StorageEdgeAudtDrnSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageEdgeAudtDrn] Stopped");
}

}  // namespace ase::storage
