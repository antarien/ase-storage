/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_acss_idx_sys.cpp
 * @brief       StorageAcssIdxSystem - rebuilds the A/ACS lookup index each tick
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @created     2026-08-16
 * @modified    2026-08-16
 * @version     1.0.0
 *
 * CAUSAL CHAIN (A/ACS Index Build)
 *
 *   [Realms, ACL rules, lattice links, keycards and tasks exist as entities]
 *          │
 *          │ Each child entity names its parent in a foreign-key field
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageAcssIdxSystem          │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageStaRelmComponent (realms)       │
 *   │    - StorageAcssRuleComponent (ACL rules)   │
 *   │    - StorageAcssCwrdComponent (required)    │
 *   │    - StorageLatLnkComponent (lattice links) │
 *   │    - StorageStaTaskComponent (need-to-know) │
 *   │    - StorageStaKycdComponent (keycards)     │
 *   │    - StorageKycdCwrdComponent (held cwrds)  │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - StorageAcssIndexResourceManager (ctx)  │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Every relation reachable by key, in O(1)
 *          ▼
 *   StorageAcssChkSystem walks the ladder without scanning
 *
 * WHY THE REBUILD IS FULL, NOT INCREMENTAL
 *   The index is DERIVED state. Rebuilding it from the entities costs one linear pass
 *   per relation, while the scans it replaces cost a product per request. Maintaining
 *   it incrementally would buy nothing and would introduce the one failure this code
 *   must never have: a destroyed keycard or a retired rule surviving in the index and
 *   granting access that the entities no longer support.
 *
 * HUB Pattern (N/A - No Hub reads/writes)
 *
 * READS (from Hub):
 *   (none)
 *
 * WRITES (to Hub):
 *   (none)
 *
 * FLYWEIGHT PATTERN (Active - StorageAcssIndexResourceManager via ctx)
 *   Owns the reverse index; components keep only their uint32_t entity references.
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
#include <ase/storage/systems/acl/storage_acss_idx_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_acss_rule_comp.hpp>
#include <ase/storage/components/state/storage_acss_cwrd_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/state/storage_sta_kycd_comp.hpp>
#include <ase/storage/components/state/storage_kycd_cwrd_comp.hpp>
#include <ase/storage/components/state/storage_lat_lnk_comp.hpp>
#include <ase/storage/components/state/storage_wflw_edge_comp.hpp>
#include <ase/storage/components/state/storage_relm_idn_comp.hpp>
#include <ase/storage/components/state/storage_kycd_idn_comp.hpp>
#include <ase/storage/components/state/storage_lnk_idn_comp.hpp>
#include <ase/storage/components/tag/storage_acss_rule_sufx_tag.hpp>
#include <ase/storage/components/state/storage_sta_idn_comp.hpp>
#include <ase/storage/components/state/storage_sta_cur_cur_comp.hpp>
#include <ase/storage/components/tag/storage_tag_kycd_vld.hpp>
#include <ase/storage/components/state/storage_sta_task_comp.hpp>
#include <ase/storage/storage_acss_index_resource_manager.hpp>
#include <ase/storage/types.hpp>
#include <ase/containers/int_hash.hpp>
// Logging
#include <ase/log/log.hpp>

using namespace entt::literals;

namespace ase::storage {

// HELPERS - PURE FUNCTIONS ONLY!
// NO STRUCTS HERE! Structs = Data = Components!
// NO View/Query operations in helpers! Only pure math!
namespace {

// Composite membership key for "user U holds codeword C in realm scope R".
// The composition itself lives in the manager, next to the set it addresses, and this
// is only the local name for it. Writer and reader are different systems: a second
// private copy would agree today and drift apart the first time one side is touched -
// and a drifted key does not fail loudly, it silently answers "not held" and denies
// every access.
uint64_t acss_held_key(uint32_t user_hash, uint32_t realm_scope, uint32_t codeword_hash) {
    return StorageAcssIndexResourceManager::compose_held_key(user_hash, realm_scope,
                                                            codeword_hash);
}

}  // namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageAcssIdxSystem::on_start(ecs::Registry& registry) {
    log::debug("[StorageAcssIdx] Started");

    auto& idx = registry.ctx().emplace<StorageAcssIndexResourceManager>();
    registry.ctx().emplace<StorageAcssIndexResourceManager*>(&idx);
}

void StorageAcssIdxSystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    auto* idx_ptr = registry.ctx().find<StorageAcssIndexResourceManager*>();
    if (!idx_ptr || !(*idx_ptr)) {
        log::error("[StorageAcssIdx] StorageAcssIndexResourceManager not in ctx (on_start must run first)");
        return;
    }
    auto& idx = **idx_ptr;

    // Derived state: dropped whole, so nothing outlives the entities it came from - but
    // ONLY the rows this system owns. Realms and network clients belong to
    // StorageIdnIdxSystem, which built them earlier in the SAME frame (tier order
    // Reception, Ingestion, Integration); clearing them here would erase a fresh row and
    // leave the ladder reading an empty bucket. Every relation has exactly ONE writer.
    idx.clear_integration();

    // ACL rules under the realm they scope, filed by the kind of match they perform.
    // The Tag decides the bucket HERE, once per rule per tick, so neither reading loop
    // in the ladder ever inspects a discriminator: extension rules and location rules
    // are two Tag-filtered Views, exactly as the ECS rules require.
    for (auto [rule_ent, rule] :
         registry.view<StorageAcssRuleComponent>(entt::exclude<StorageAcssRuleSufxTag>).each()) {
        idx.store_rule(rule.relm_ref, static_cast<uint32_t>(rule_ent));
        idx.store_location_rule(rule.relm_ref, static_cast<uint32_t>(rule_ent));
    }
    for (auto [rule_ent, rule] :
         registry.view<StorageAcssRuleComponent, StorageAcssRuleSufxTag>().each()) {
        idx.store_rule(rule.relm_ref, static_cast<uint32_t>(rule_ent));
        idx.store_extension_rule(rule.relm_ref, static_cast<uint32_t>(rule_ent));
    }

    // Lattice links under the realm they target.
    for (auto [link_ent, link_idn] : registry.view<StorageLnkIdnComponent>().each()) {
        idx.store_link(static_cast<uint64_t>(link_idn.target_realm_hash),
                       static_cast<uint32_t>(link_ent));
    }

    // Required codewords under the ACL rule that demands them.
    for (auto [cwrd_ent, cwrd] : registry.view<StorageAcssCwrdComponent>().each()) {
        idx.store_required(cwrd.acss_ref, static_cast<uint32_t>(cwrd_ent));
    }

    // Workflow transitions under the label they lead away from.
    for (auto [edge_ent, edge] : registry.view<StorageWflwEdgeComponent>().each()) {
        idx.store_edge(static_cast<uint64_t>(edge.from_label_hash),
                       static_cast<uint32_t>(edge_ent));
    }

    // Curation rows under their (project, key) pair. The curator adds fresh rows to this
    // same map as it creates them, so a row minted for one request is visible to the next
    // request of the same pass.
    for (auto [cur_ent, cur] : registry.view<StorageStaCurCurComponent>().each()) {
        idx.store_curation(
            StorageAcssIndexResourceManager::compose_curation_key(cur.project_ref, cur.key_hash),
            static_cast<uint32_t>(cur_ent));
    }

    // Users holding an authenticated session. Built HERE, at the start of Integration,
    // because the sessions of this frame are validated during Ingestion - an earlier
    // build would answer with the previous frame's set and refuse a user who just
    // authenticated.
    for (auto [session_ent, idn] :
         registry.view<StorageStaIdnComponent, StorageKycdVldTag>().each()) {
        (void)session_ent;
        idx.store_session(idn.user_id_hash);
    }

    // Need-to-know tasks under their project.
    for (auto [task_ent, task] : registry.view<StorageStaTaskComponent>().each()) {
        idx.store_task(task.proj_ref, static_cast<uint32_t>(task_ent));
    }

    // Held codewords, flattened. Walking the LEAVES and reaching the parent keycard by
    // its entity id is what collapses two nesting levels: the keycard is a try_get, not
    // a scan, and the ladder afterwards asks a membership question instead of walking
    // every keycard of a user and every codeword on it.
    for (auto [held_ent, held] : registry.view<StorageKycdCwrdComponent>().each()) {
        (void)held_ent;
        const auto kycd_ent = static_cast<ecs::Entity>(held.kycd_ref);
        auto* kycd = registry.try_get<StorageStaKycdComponent>(kycd_ent);
        if (kycd == nullptr) {
            log::error(log::ERR::CAT::INVALID_ENTITY, "StorageAcssIdxSystem", held.kycd_ref,
                       "StorageStaKycdComponent");
            continue;
        }
        auto* kycd_idn = registry.try_get<StorageKycdIdnComponent>(kycd_ent);
        if (kycd_idn == nullptr) {
            log::error(log::ERR::CAT::COMPONENT_MISSING, "StorageAcssIdxSystem", held.kycd_ref,
                       "StorageKycdIdnComponent");
            continue;
        }
        idx.store_held(acss_held_key(kycd_idn->issued_to_hash, kycd->relm_ref, held.cwrd_hash));
    }
}

void StorageAcssIdxSystem::on_stop(ecs::Registry& registry) {
    log::debug("[StorageAcssIdx] Stopped");

    auto* idx_ptr = registry.ctx().find<StorageAcssIndexResourceManager*>();
    if (idx_ptr && *idx_ptr) {
        (*idx_ptr)->clear_all();
    }
}

}  // namespace ase::storage
