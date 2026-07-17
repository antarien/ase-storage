/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_edge_ini_sys.cpp
 * @brief       StorageEdgeIniSystem - Seeds the edge_binaries distribution realm
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Initialization
 * @created     2026-06-24
 * @modified    2026-06-24
 * @version     1.0.0
 *
 * CAUSAL CHAIN (Edge-Distribution Realm Seeding)
 *
 *   [Server startup, after StorageIniSystem]
 *          │
 *          │ App runs Initialization schedule
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageEdgeIniSystem          │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageResourceManager* from ctx()      │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - edge_binaries realm directory tree     │
 *   │    - StorageStaRelmComponent (edge realm)   │
 *   │    - StorageRelmPublicTag + RelmActiveTag    │
 *   │    - StorageAcssRuleComponent (release path)│
 *   │    - StorageAcssCwrdComponent (BINARY)       │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Realm + release ACL rule ready
 *          ▼
 *   StorageAcssChkSystem gates customer downloads against the rule
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
 *   Realm path resolution + directory creation via the manager.
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
#include <ase/storage/systems/edge/storage_edge_ini_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/state/storage_acss_rule_comp.hpp>
#include <ase/storage/components/state/storage_acss_cwrd_comp.hpp>
#include <ase/storage/components/state/storage_wflw_edge_comp.hpp>
#include <ase/storage/components/tag/storage_tag_relm_public.hpp>
#include <ase/storage/components/tag/storage_tag_relm_active.hpp>
#include <ase/storage/storage_resource_manager.hpp>
#include <ase/storage/types.hpp>
#include <ase/utils/strops.hpp>
// Hub publish for the drill's initial workflow stage (edge-download gate reads it).
#include <ase/hub/api.hpp>
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

// One workflow-transition edge entity (die Kante). The transition graph is
// DATA: StorageWflwTranSystem validates promote requests purely against these
// entities — a new allowed transition = one more seed call, never system code.
void seed_wflw_edge(ecs::Registry& registry, const char* from_label, const char* to_label) {
    auto edge_ent = registry.create();
    auto& edge = registry.emplace<StorageWflwEdgeComponent>(edge_ent);
    ase::utils::str_copy(edge.from_label, MAX_LABEL_LEN, from_label);
    ase::utils::str_copy(edge.to_label, MAX_LABEL_LEN, to_label);
}

// One ACL rule + its required codeword (Entity-per-Item pair). Pattern semantics
// per types.hpp ACSS_MATCH_SUFFIX_BONUS: leading '*' = suffix rule.
void seed_acss_rule(ecs::Registry& registry, uint32_t relm_ref, const char* pattern,
                    const char* label, const char* required_cwrd) {
    auto rule_ent = registry.create();
    auto& rule = registry.emplace<StorageAcssRuleComponent>(rule_ent);
    rule.relm_ref = relm_ref;
    rule.proj_ref = 0;
    ase::utils::str_copy(rule.path_pattern, MAX_PATH_LEN, pattern);
    rule.protection_level = PROTECTION_PUBLIC;
    ase::utils::str_copy(rule.label, MAX_LABEL_LEN, label);

    auto cwrd_ent = registry.create();
    auto& cwrd = registry.emplace<StorageAcssCwrdComponent>(cwrd_ent);
    cwrd.acss_ref = static_cast<uint32_t>(rule_ent);
    ase::utils::str_copy(cwrd.required_cwrd, MAX_CODEWORD_LEN, required_cwrd);
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageEdgeIniSystem::on_start(ecs::Registry& registry) {
    log::debug("[StorageEdgeIni] Started");

    auto* mgr_ptr = registry.ctx().find<StorageResourceManager*>();
    if (!mgr_ptr || !(*mgr_ptr)) {
        log::error("[StorageEdgeIni] StorageResourceManager not in ctx (StorageIniSystem must run first)");
        return;
    }
    auto& mgr = **mgr_ptr;

    // Realm directory tree: edge_binaries with release + keys subdirectories
    char dir[512];
    mgr.resolve_path(EDGE_REALM_ID, nullptr, "", dir, 512);
    mgr.ensure_dir(dir);
    mgr.resolve_path(EDGE_REALM_ID, nullptr, "release", dir, 512);
    mgr.ensure_dir(dir);
    mgr.resolve_path(EDGE_REALM_ID, nullptr, "keys", dir, 512);
    mgr.ensure_dir(dir);
    log::info("[StorageEdgeIni] edge_binaries realm directory ready");

    // Realm entity: public platform realm, Enterprise tier, no concealment.
    // quota_bytes = the MEASURED 10 GB ceiling (types.hpp EDGE_REALM_QUOTA_BYTES,
    // R14 sizing) — the retired-cleanup keeps the realm small, not the ceiling.
    auto realm_ent = registry.create();
    auto& relm = registry.emplace<StorageStaRelmComponent>(realm_ent);
    ase::utils::str_copy(relm.id, MAX_REALM_ID, EDGE_REALM_ID);
    ase::utils::str_copy(relm.name, MAX_REALM_NAME, "Edge Binary Distribution");
    relm.default_protection = PROTECTION_PUBLIC;
    relm.tier = TIER_ENTERPRISE;
    relm.quota_bytes = EDGE_REALM_QUOTA_BYTES;
    registry.emplace<StorageRelmPublicTag>(realm_ent);
    registry.emplace<StorageRelmActiveTag>(realm_ent);
    log::info("[StorageEdgeIni] edge_binaries realm entity registered (tier=Enterprise, public, quota={} bytes)",
              relm.quota_bytes);

    // A/ACS rules (Entity-per-Item pairs rule+codeword). ALL FOUR edge codeword
    // axes are realm ACL DATA — not just BINARY with suffix-only enforcement in
    // the download gate: *.sig/*.spdx.json are SUFFIX rules (companions sit
    // beside binaries), version/compatibility manifests carry METADATA.
    const uint32_t relm_ref = static_cast<uint32_t>(realm_ent);
    seed_acss_rule(registry, relm_ref, "release/*", EDGE_LABEL_RELEASED, EDGE_CWRD_BINARY);
    seed_acss_rule(registry, relm_ref, "*.sig", EDGE_LABEL_RELEASED, EDGE_CWRD_SIG);
    seed_acss_rule(registry, relm_ref, "*.spdx.json", EDGE_LABEL_RELEASED, EDGE_CWRD_SBOM);
    seed_acss_rule(registry, relm_ref, "release/version.json", EDGE_LABEL_RELEASED, EDGE_CWRD_METADATA);
    seed_acss_rule(registry, relm_ref, "release/compatibility.json", EDGE_LABEL_RELEASED, EDGE_CWRD_METADATA);
    // Phase-12 Task-12.3 workflow-drill asset: one designated per-asset rule seeded at
    // DRAFT so an operator can promote it draft to review to approved to released live
    // (the pattern rules above sit at RELEASED). Most-specific-wins gives this full-path
    // rule to the drill asset only; named test fixture for the E2E demonstration.
    const char* drill_path = "release/linux-x86_64/ase-edge-daemon-drill";
    seed_acss_rule(registry, relm_ref, drill_path, EDGE_LABEL_DRAFT, EDGE_CWRD_BINARY);
    // Publish the drill's initial workflow stage (display) AND the customer-public serving verdict
    // to the Hub. The serving verdict STG_WFLW_PUB is computed HERE, from the SAME SSOT rule as the
    // promote path (public iff label == EDGE_LABEL_RELEASED, ARCH_ASE_REASONING_EDGE §6.4) — draft is
    // NOT public, so the edge-download gate denies the drill until an operator promotes it to released.
    // StorageWflwTranSystem republishes both keys (same owner=hashed_string(path)) on every promote.
    // The pattern rules above stay unpublished (a pattern is not a concrete request path), so the
    // statically-released assets keep serving on the realm default — no regression.
    const uint32_t drill_owner = entt::hashed_string(drill_path).value();
    hub::set(registry, drill_owner, "STG_WFLW_STAGE"_hs, static_cast<float>(WFLW_STAGE_DRAFT));
    hub::set(registry, drill_owner, "STG_WFLW_PUB"_hs,
             ase::utils::str_equal(EDGE_LABEL_DRAFT, EDGE_LABEL_RELEASED, MAX_LABEL_LEN) ? 1.0f : 0.0f);
    log::info("[StorageEdgeIni] edge ACL rules ready (BINARY release/*, SIG *.sig, SBOM *.spdx.json, METADATA manifests; drill asset at draft)");

    // Workflow-transition graph (Phase 12 Task 12.3) — the release pipeline
    // draft → review → approved → released → retired as seeded edge entities.
    seed_wflw_edge(registry, EDGE_LABEL_DRAFT, EDGE_LABEL_REVIEW);
    seed_wflw_edge(registry, EDGE_LABEL_REVIEW, EDGE_LABEL_APPROVED);
    seed_wflw_edge(registry, EDGE_LABEL_APPROVED, EDGE_LABEL_RELEASED);
    seed_wflw_edge(registry, EDGE_LABEL_RELEASED, EDGE_LABEL_RETIRED);
    log::info("[StorageEdgeIni] workflow transition graph seeded (4 edges: draft->review->approved->released->retired)");
}

void StorageEdgeIniSystem::tick(ecs::Registry& /*registry*/, float /*dt*/) {
    // Initialization system has no per-tick logic
}

void StorageEdgeIniSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageEdgeIni] Stopped");
}

}  // namespace ase::storage
