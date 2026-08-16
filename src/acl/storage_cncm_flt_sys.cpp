/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_cncm_flt_sys.cpp
 * @brief       StorageCncmFltSystem - Realm concealment filter
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @created     2026-04-05
 * @modified    2026-06-24
 * @version     1.0.0
 *
 * CAUSAL CHAIN (Concealment Filter)
 *
 *   [Realm listing request]
 *          │
 *          │ Integration schedule processes request
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageCncmFltSystem          │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageStaRelmComponent (realm list)   │
 *   │    - StorageRelmConcealTag (hidden realms)  │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - Filters realm list for non-members     │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Filtered realm list returned
 *          ▼
 *   HTTP response with visible realms only
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
 *   Concealment state stored as Tags on realm entities.
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
#include <ase/storage/systems/acl/storage_cncm_flt_sys.hpp>
// Components + tags from same module
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/tag/storage_tag_relm_public.hpp>
#include <ase/storage/components/tag/storage_tag_relm_active.hpp>
#include <ase/storage/components/tag/storage_tag_relm_conceal.hpp>
#include <ase/storage/components/tag/storage_tag_relm_suspended.hpp>
#include <ase/storage/components/tag/storage_tag_relm_archived.hpp>
#include <ase/storage/components/tag/storage_relm_visb_tag.hpp>
// Logging
#include <ase/log/log.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
namespace {

// No helper functions needed → every condition is a View filter, never a branch

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageCncmFltSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageCncmFlt] Started");
}

void StorageCncmFltSystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    // Derives ONE thing: may this realm appear in a listing to someone who has
    // named no keycard. That question is viewer-INDEPENDENT, which is the only
    // reason the answer may live on the realm entity at all. The viewer-specific
    // half - a concealed realm IS visible to its owner and its members - stays
    // in StorageAcssChkSystem, where the requester is known. Folding it in here
    // would cache one user's answer on an entity every user reads.
    //
    // EVERY VIEW IS A NAMED VARIABLE, AND THAT IS NOT A STYLE CHOICE.
    // The transpiler discovers views by their variable name (body_transformer
    // discovered_views_ / view_query_map_) and gives each its own becsy query -
    // mainQuery, query_2, query_3 ... with the excludes carried across as
    // .without(). Written INLINE inside the range-for, a view has no name to
    // discover: measured 2026-08-16, six inline views collapsed into ONE query
    // holding the union of all seven components, including the tag this system
    // WRITES, so the generated system matched a set that cannot exist and could
    // never have tagged anything. Same C++ semantics, named - and the queries
    // come out one per view.
    //
    // Each loop mutates a type that is NOT the filter of the view it walks, or
    // removes the very component the view leads with, so nothing invalidates
    // under the iterator and no deferred array is needed.

    // GAINED: public and active, and none of the three hiding states.
    auto listable_view = registry.view<StorageStaRelmComponent, StorageRelmPublicTag,
                                       StorageRelmActiveTag>(
        entt::exclude<StorageRelmConcealTag, StorageRelmSuspendedTag, StorageRelmArchivedTag>);
    for (auto relm_ent : listable_view) {
        registry.emplace_or_replace<StorageRelmVisbTag>(relm_ent);
    }

    // LOST: the tag is carried, but one condition stopped holding. Five disjoint
    // Views instead of one loop with five ifs - the conditions compose in the
    // type system, which is also what lets each one become its own becsy query.
    auto conceal_view = registry.view<StorageRelmVisbTag, StorageRelmConcealTag>();
    for (auto relm_ent : conceal_view) {
        registry.remove<StorageRelmVisbTag>(relm_ent);
    }
    auto suspended_view = registry.view<StorageRelmVisbTag, StorageRelmSuspendedTag>();
    for (auto relm_ent : suspended_view) {
        registry.remove<StorageRelmVisbTag>(relm_ent);
    }
    auto archived_view = registry.view<StorageRelmVisbTag, StorageRelmArchivedTag>();
    for (auto relm_ent : archived_view) {
        registry.remove<StorageRelmVisbTag>(relm_ent);
    }
    auto unpublic_view = registry.view<StorageRelmVisbTag>(entt::exclude<StorageRelmPublicTag>);
    for (auto relm_ent : unpublic_view) {
        registry.remove<StorageRelmVisbTag>(relm_ent);
    }
    auto inactive_view = registry.view<StorageRelmVisbTag>(entt::exclude<StorageRelmActiveTag>);
    for (auto relm_ent : inactive_view) {
        registry.remove<StorageRelmVisbTag>(relm_ent);
    }
}

void StorageCncmFltSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageCncmFlt] Stopped");
}

}  // namespace ase::storage
