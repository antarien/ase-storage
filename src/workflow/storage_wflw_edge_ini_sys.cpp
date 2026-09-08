/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_wflw_edge_ini_sys.cpp
 * @brief       StorageWflwEdgeIniSystem - Seeds the release-pipeline transition graph
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Initialization
 * @created     2026-08-31
 * @modified    2026-08-31
 * @version     1.0.0
 *
 * CAUSAL CHAIN (Workflow Transition Graph Seeding)
 *
 *   [Server startup]
 *          │
 *          │ App runs Initialization schedule
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageWflwEdgeIniSystem      │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - (nothing - EDGE_LABEL_* from types.hpp)│
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - StorageWflwEdgeComponent (4 entities)  │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Allowed transitions available as entities
 *          ▼
 *   StorageWflwTranSystem validates every promote request against them
 *   StorageAcssIdxSystem indexes them for the per-tick lookup
 *
 * HUB Pattern (N/A - No Hub reads/writes)
 *
 * READS (from Hub):
 *   (none)
 *
 * WRITES (to Hub):
 *   (none)
 *
 * FLYWEIGHT PATTERN (Inactive - no external resource is touched)
 *   The transition graph is Component data in the registry, not a file, socket or
 *   arena. Nothing is resolved through StorageResourceManager, so the system needs
 *   no ctx() lookup and has no failure mode when the manager is not up yet.
 *
 * DEDICATION
 *   The TRANSITION GRAPH and nothing else. The realm the pipeline governs belongs to
 *   StorageEdgeIniSystem, the ACL rules that carry the labels to
 *   StorageAcssEdgeIniSystem. Three objects that used to share one file; the file
 *   said so itself, in three separate log lines.
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
#include <ase/storage/systems/workflow/storage_wflw_edge_ini_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_wflw_edge_comp.hpp>
#include <ase/storage/types.hpp>
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

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageWflwEdgeIniSystem::on_start(ecs::Registry& registry) {
    log::debug("[StorageWflwEdgeIni] Started");

    // The release pipeline as ALLOWED transitions, one entity each. The chain is
    // written out instead of looped, because the four steps are not four values of
    // one thing: each is a named stage change with its own meaning, and the SHAPE of
    // the pipeline has to be readable here without following a table elsewhere.
    //
    //     draft ────▶ review ────▶ approved ────▶ released ────▶ retired
    //
    // What must hold in EVERY block below, and what no compiler and no gate checks:
    // the label copied into the field and the label handed to hashed_string are the
    // SAME constant. A mismatched pair is not a loud failure - the transition simply
    // never matches, and the release pipeline stalls with no error anywhere.

    // draft → review: an uploaded build enters release-manager review.
    auto edge_draft_review = registry.create();
    auto& draft_review = registry.emplace<StorageWflwEdgeComponent>(edge_draft_review);
    ase::utils::str_copy(draft_review.from_label, MAX_LABEL_LEN, EDGE_LABEL_DRAFT);
    draft_review.from_label_hash = entt::hashed_string(EDGE_LABEL_DRAFT).value();
    ase::utils::str_copy(draft_review.to_label, MAX_LABEL_LEN, EDGE_LABEL_REVIEW);
    draft_review.to_label_hash = entt::hashed_string(EDGE_LABEL_REVIEW).value();

    // review → approved: the review passed, the build awaits publication.
    auto edge_review_approved = registry.create();
    auto& review_approved = registry.emplace<StorageWflwEdgeComponent>(edge_review_approved);
    ase::utils::str_copy(review_approved.from_label, MAX_LABEL_LEN, EDGE_LABEL_REVIEW);
    review_approved.from_label_hash = entt::hashed_string(EDGE_LABEL_REVIEW).value();
    ase::utils::str_copy(review_approved.to_label, MAX_LABEL_LEN, EDGE_LABEL_APPROVED);
    review_approved.to_label_hash = entt::hashed_string(EDGE_LABEL_APPROVED).value();

    // approved → released: the ONLY transition that makes an asset customer-public.
    auto edge_approved_released = registry.create();
    auto& approved_released = registry.emplace<StorageWflwEdgeComponent>(edge_approved_released);
    ase::utils::str_copy(approved_released.from_label, MAX_LABEL_LEN, EDGE_LABEL_APPROVED);
    approved_released.from_label_hash = entt::hashed_string(EDGE_LABEL_APPROVED).value();
    ase::utils::str_copy(approved_released.to_label, MAX_LABEL_LEN, EDGE_LABEL_RELEASED);
    approved_released.to_label_hash = entt::hashed_string(EDGE_LABEL_RELEASED).value();

    // released → retired: withdrawal. There is deliberately NO edge back out of
    // retired and none that skips a stage - the graph IS the policy.
    auto edge_released_retired = registry.create();
    auto& released_retired = registry.emplace<StorageWflwEdgeComponent>(edge_released_retired);
    ase::utils::str_copy(released_retired.from_label, MAX_LABEL_LEN, EDGE_LABEL_RELEASED);
    released_retired.from_label_hash = entt::hashed_string(EDGE_LABEL_RELEASED).value();
    ase::utils::str_copy(released_retired.to_label, MAX_LABEL_LEN, EDGE_LABEL_RETIRED);
    released_retired.to_label_hash = entt::hashed_string(EDGE_LABEL_RETIRED).value();

    log::info("[StorageEdgeIni] workflow transition graph seeded (4 edges: draft->review->approved->released->retired)");
}

void StorageWflwEdgeIniSystem::tick(ecs::Registry& /*registry*/, float /*dt*/) {
    // Initialization system has no per-tick logic
}

void StorageWflwEdgeIniSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageWflwEdgeIni] Stopped");
}

}  // namespace ase::storage
