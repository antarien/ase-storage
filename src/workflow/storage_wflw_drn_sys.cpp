/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_wflw_drn_sys.cpp
 * @brief       StorageWflwDrnSystem - Drains Hub-bridge workflow-promote requests
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
 * @created     2026-07-11
 * @modified    2026-07-11
 * @version     1.0.0
 *
 * CAUSAL CHAIN (Workflow Promote Drain)
 *
 *   [POST /admin/workflow/promote → sdk::emplace_workflow_promote_request]
 *          │
 *          │ Ingestion schedule drains the Hub bridge
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageWflwDrnSystem          │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - hub::HubStgWflwReqComponent            │
 *   │    - hub::HubStgWflwPendTag (filter)        │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - StorageReqWflwTranComponent (new)      │
 *   │    - StorageWflwPendTag (new entity)        │
 *   │    - StorageWflwGateTag (target=released)   │
 *   │    - STG_WFLW_RES = PENDING (owner-scoped)  │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Bridge entity destroyed, request staged
 *          ▼
 *   StorageWflwGateSystem / StorageWflwTranSystem (Integration)
 *
 * HUB Pattern (Active)
 *
 * READS (from Hub):
 *   (none — the bridge payload rides the typed HubStgWflwReqComponent)
 *
 * WRITES (to Hub):
 *   - STG_WFLW_RES (owner = hashed_string(path)): WFLW_RES_PENDING
 *
 * FLYWEIGHT PATTERN (N/A - no external resources)
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
#include <ase/storage/systems/workflow/storage_wflw_drn_sys.hpp>
// Components from same module
#include <ase/storage/components/request/storage_req_wflw_tran_comp.hpp>
#include <ase/storage/components/tag/storage_tag_wflw_pend.hpp>
#include <ase/storage/components/tag/storage_tag_wflw_gate.hpp>
#include <ase/storage/types.hpp>
// Hub API (bridge component + discovery tag + verdict publish)
#include <ase/hub/api.hpp>
// String ops (L0 — sanitized copy)
#include <ase/utils/strops.hpp>

#include <entt/core/hashed_string.hpp>
// Logging
#include <ase/log/log.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
namespace {

// Sanitized bounded copy: JSON-breaking bytes ('"', '\\') and ASCII control
// bytes are SKIPPED during the copy so every downstream consumer (audit reason,
// frame-112 persist document) can embed the strings verbatim. Pure string math.
void sanitized_copy(char* dst, uint32_t dst_size, const char* src) {
    uint32_t w = 0;
    for (uint32_t r = 0; src[r] != '\0' && w + 1 < dst_size; ++r) {
        char c = src[r];
        if (c == '"' || c == '\\') continue;
        if (static_cast<unsigned char>(c) < 32u) continue;
        dst[w] = c;
        ++w;
    }
    dst[w] = '\0';
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageWflwDrnSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageWflwDrn] Started");
}

void StorageWflwDrnSystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    // Deferred deletion: collect drained bridge entities, destroy after the loop
    // (bounded batch per tick, mirror StorageCredAcssRspSystem).
    ecs::Entity done[WFLW_REQ_BATCH];
    uint32_t done_n = 0;

    auto bridge_view = registry.view<hub::HubStgWflwReqComponent, hub::HubStgWflwPendTag>();
    for (auto [bridge_ent, breq] : bridge_view.each()) {
        if (done_n >= WFLW_REQ_BATCH) break;

        if (breq.path[0] == '\0' || breq.target_label[0] == '\0' || breq.requested_by[0] == '\0') {
            log::error("[StorageWflwDrn] NOT_FOUND: bridge request with empty field dropped (path/target/requester required)");
            done[done_n] = bridge_ent;
            ++done_n;
            continue;
        }

        auto req_ent = registry.create();
        auto& req = registry.emplace<StorageReqWflwTranComponent>(req_ent);
        sanitized_copy(req.path, MAX_PATH_LEN, breq.path);
        sanitized_copy(req.target_label, MAX_LABEL_LEN, breq.target_label);
        sanitized_copy(req.requested_by, MAX_OWNER_ID, breq.requested_by);
        registry.emplace<StorageWflwPendTag>(req_ent);

        // released-gate: the artifact precondition runs as a SEPARATE Tag-filtered
        // check (StorageWflwGateSystem) — the transition system never re-tests it.
        if (ase::utils::str_equal(req.target_label, EDGE_LABEL_RELEASED, MAX_LABEL_LEN)) {
            registry.emplace<StorageWflwGateTag>(req_ent);
        }

        // Publish the staged verdict so the route's poll sees the request landed.
        const uint32_t owner = entt::hashed_string(req.path).value();
        hub::set(registry, owner, "STG_WFLW_RES"_hs, static_cast<float>(WFLW_RES_PENDING));

        log::info("[StorageWflwDrn] Promote request staged: {} -> {} (by {})",
                  req.path, req.target_label, req.requested_by);

        done[done_n] = bridge_ent;
        ++done_n;
    }

    for (uint32_t i = 0; i < done_n; ++i) {
        registry.destroy(done[i]);
    }
}

void StorageWflwDrnSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageWflwDrn] Stopped");
}

}  // namespace ase::storage
