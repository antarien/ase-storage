/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_acss_edge_ini_sys.cpp
 * @brief       StorageAcssEdgeIniSystem - Seeds the edge realm's ACL rules
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Initialization
 * @created     2026-08-31
 * @modified    2026-08-31
 * @version     1.0.0
 *
 * CAUSAL CHAIN (Edge Realm ACL Seeding)
 *
 *   [Server startup, after StorageEdgeIniSystem]
 *          │
 *          │ App runs Initialization schedule
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageAcssEdgeIniSystem      │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageStaRelmComponent (edge realm)   │
 *   │    - StorageRelmEdgeTag (realm identity)    │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - StorageAcssRuleComponent (6 rules)     │
 *   │    - StorageRuleIdnComponent (per rule)     │
 *   │    - StorageAcssRuleSufxTag (suffix rules)  │
 *   │    - StorageAcssCwrdComponent (per rule)    │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Rules + codewords ready
 *          ▼
 *   StorageAcssIdxSystem indexes them per realm
 *   StorageAcssRslvSystem picks the most specific rule for a request path
 *   StorageAcssChkSystem gates customer downloads against it
 *
 * HUB Pattern (Active)
 *
 * READS (from Hub):
 *   (none)
 *
 * WRITES (to Hub):
 *   - STG_WFLW_STAGE (owner = hashed_string(drill path)): WFLW_STAGE_DRAFT ordinal
 *   - STG_WFLW_PUB   (owner = hashed_string(drill path)): customer-public verdict
 *
 * FLYWEIGHT PATTERN (Inactive - no external resource is touched)
 *   Rules and codewords are Component data in the registry. The realm DIRECTORY tree
 *   they govern is created by StorageEdgeIniSystem through StorageResourceManager;
 *   this system never resolves a path and needs no ctx() lookup.
 *
 * DEDICATION
 *   The ACL RULES and nothing else. The realm belongs to StorageEdgeIniSystem, the
 *   workflow transition graph to StorageWflwEdgeIniSystem. Three objects that used to
 *   share one file; the file said so itself, in three separate log lines.
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
#include <ase/storage/systems/acl/storage_acss_edge_ini_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/state/storage_acss_rule_comp.hpp>
#include <ase/storage/components/state/storage_acss_cwrd_comp.hpp>
#include <ase/storage/components/state/storage_rule_idn_comp.hpp>
#include <ase/storage/components/tag/storage_relm_edge_tag.hpp>
#include <ase/storage/components/tag/storage_acss_rule_sufx_tag.hpp>
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

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageAcssEdgeIniSystem::on_start(ecs::Registry& registry) {
    log::debug("[StorageAcssEdgeIni] Started");

    // The edge realm is the scope of every rule below. StorageEdgeIniSystem is the
    // ONLY producer of StorageRelmEdgeTag, so the tag IS the identity - no scan over
    // every realm comparing id strings, which is what this tag exists to replace.
    uint32_t relm_ref = 0;
    bool relm_found = false;
    for (auto relm_ent : registry.view<StorageStaRelmComponent, StorageRelmEdgeTag>()) {
        relm_ref = static_cast<uint32_t>(relm_ent);
        relm_found = true;
        break;
    }
    if (!relm_found) {
        // The realm entity is produced in the SAME schedule by StorageEdgeIniSystem.
        // Missing means this system ran first - a scheduling fault, not bad data, so
        // the category names the ORDER and the producer, not the component.
        log::error(log::ERR::CAT::SCHEDULE_ORDER, "StorageAcssEdgeIniSystem",
                   "StorageEdgeIniSystem");
        return;
    }

    // A/ACS rules, six of them, each an Entity-per-Item pair rule+codeword. ALL FOUR
    // edge codeword axes are realm ACL DATA - not just BINARY with suffix-only
    // enforcement in the download gate.
    //
    // The six differ on exactly four axes, and they are written out rather than fed
    // through one generic seeder so that every difference is READABLE here instead of
    // hidden behind a parameter list:
    //
    //     pattern                                    kind      label     codeword
    //     release/*                                  location  released  BINARY
    //     *.sig                                      SUFFIX    released  SIG
    //     *.spdx.json                                SUFFIX    released  SBOM
    //     release/version.json                       location  released  METADATA
    //     release/compatibility.json                 location  released  METADATA
    //     release/linux-x86_64/ase-edge-daemon-drill location  DRAFT     BINARY
    //
    // KIND IS DECIDED ONCE, HERE, and carried by a Tag - the resolver ladder never
    // re-reads a pattern to find out what kind of rule it is. Semantics per types.hpp
    // ACSS_MATCH_SUFFIX_BONUS: a leading '*' means suffix rule. Because every pattern
    // below is a literal, the kind is known at the point of writing; there is no
    // runtime branch left to take.
    //
    // A location rule keeps its pattern VERBATIM, the trailing wildcard included.
    // That is what the character comparison it replaces did, and reproducing it
    // exactly is the point: a refactor must not change which assets a rule governs.

    // Rule 1 of 6 - release/*, the LOCATION rule every released binary falls under.
    const char* const bin_pattern = "release/*";
    auto bin_rule_ent = registry.create();
    auto& bin_rule = registry.emplace<StorageAcssRuleComponent>(bin_rule_ent);
    bin_rule.relm_ref = relm_ref;
    bin_rule.proj_ref = 0;
    ase::utils::str_copy(bin_rule.path_pattern, MAX_PATH_LEN, bin_pattern);
    bin_rule.protection_level = PROTECTION_PUBLIC;
    ase::utils::str_copy(bin_rule.label, MAX_LABEL_LEN, EDGE_LABEL_RELEASED);
    auto& bin_idn = registry.emplace<StorageRuleIdnComponent>(bin_rule_ent);
    bin_idn.pattern_hash = entt::hashed_string(bin_pattern).value();
    bin_idn.label_hash = entt::hashed_string(EDGE_LABEL_RELEASED).value();
    bin_idn.match_len = ase::utils::str_len(bin_pattern, MAX_PATH_LEN);
    bin_idn.match_hash = bin_idn.pattern_hash;
    auto bin_cwrd_ent = registry.create();
    auto& bin_cwrd = registry.emplace<StorageAcssCwrdComponent>(bin_cwrd_ent);
    bin_cwrd.acss_ref = static_cast<uint32_t>(bin_rule_ent);
    ase::utils::str_copy(bin_cwrd.required_cwrd, MAX_CODEWORD_LEN, EDGE_CWRD_BINARY);
    bin_cwrd.required_cwrd_hash = entt::hashed_string(EDGE_CWRD_BINARY).value();

    // Rule 2 of 6 - *.sig, a SUFFIX rule: signatures sit BESIDE the binary they sign,
    // so the rule cannot be anchored to a directory.
    const char* const sig_pattern = "*.sig";
    auto sig_rule_ent = registry.create();
    auto& sig_rule = registry.emplace<StorageAcssRuleComponent>(sig_rule_ent);
    sig_rule.relm_ref = relm_ref;
    sig_rule.proj_ref = 0;
    ase::utils::str_copy(sig_rule.path_pattern, MAX_PATH_LEN, sig_pattern);
    sig_rule.protection_level = PROTECTION_PUBLIC;
    ase::utils::str_copy(sig_rule.label, MAX_LABEL_LEN, EDGE_LABEL_RELEASED);
    auto& sig_idn = registry.emplace<StorageRuleIdnComponent>(sig_rule_ent);
    sig_idn.pattern_hash = entt::hashed_string(sig_pattern).value();
    sig_idn.label_hash = entt::hashed_string(EDGE_LABEL_RELEASED).value();
    registry.emplace<StorageAcssRuleSufxTag>(sig_rule_ent);
    sig_idn.match_len = ase::utils::str_len(sig_pattern, MAX_PATH_LEN) - 1u;
    sig_idn.match_hash = entt::hashed_string::value(sig_pattern + 1, sig_idn.match_len);
    // An extension rule is matched against the path's dot-suffixes, so its literal
    // MUST begin with a dot. A pattern that does not would silently match nothing;
    // saying so out loud is the difference between a bug and a rejected input. The
    // literal above satisfies it today - the check guards the day someone edits it.
    if (sig_idn.match_len < 1u || sig_pattern[1] != '.') {
        log::warn(log::WRN::CAT::VALUE_INVALID, "StorageAcssEdgeIniSystem",
                  sig_idn.pattern_hash, "acss_sufx_pattern_dot",
                  static_cast<float>(sig_idn.match_len));
    }
    auto sig_cwrd_ent = registry.create();
    auto& sig_cwrd = registry.emplace<StorageAcssCwrdComponent>(sig_cwrd_ent);
    sig_cwrd.acss_ref = static_cast<uint32_t>(sig_rule_ent);
    ase::utils::str_copy(sig_cwrd.required_cwrd, MAX_CODEWORD_LEN, EDGE_CWRD_SIG);
    sig_cwrd.required_cwrd_hash = entt::hashed_string(EDGE_CWRD_SIG).value();

    // Rule 3 of 6 - *.spdx.json, a SUFFIX rule for the same reason as *.sig: the
    // bill of materials is a companion file, not a location.
    const char* const sbom_pattern = "*.spdx.json";
    auto sbom_rule_ent = registry.create();
    auto& sbom_rule = registry.emplace<StorageAcssRuleComponent>(sbom_rule_ent);
    sbom_rule.relm_ref = relm_ref;
    sbom_rule.proj_ref = 0;
    ase::utils::str_copy(sbom_rule.path_pattern, MAX_PATH_LEN, sbom_pattern);
    sbom_rule.protection_level = PROTECTION_PUBLIC;
    ase::utils::str_copy(sbom_rule.label, MAX_LABEL_LEN, EDGE_LABEL_RELEASED);
    auto& sbom_idn = registry.emplace<StorageRuleIdnComponent>(sbom_rule_ent);
    sbom_idn.pattern_hash = entt::hashed_string(sbom_pattern).value();
    sbom_idn.label_hash = entt::hashed_string(EDGE_LABEL_RELEASED).value();
    registry.emplace<StorageAcssRuleSufxTag>(sbom_rule_ent);
    sbom_idn.match_len = ase::utils::str_len(sbom_pattern, MAX_PATH_LEN) - 1u;
    sbom_idn.match_hash = entt::hashed_string::value(sbom_pattern + 1, sbom_idn.match_len);
    // Same guarantee as at *.sig, and it is repeated rather than shared: a shared
    // check would have to take the pattern as a parameter, and then the reader can no
    // longer see WHICH pattern it protects without leaving this spot.
    if (sbom_idn.match_len < 1u || sbom_pattern[1] != '.') {
        log::warn(log::WRN::CAT::VALUE_INVALID, "StorageAcssEdgeIniSystem",
                  sbom_idn.pattern_hash, "acss_sufx_pattern_dot",
                  static_cast<float>(sbom_idn.match_len));
    }
    auto sbom_cwrd_ent = registry.create();
    auto& sbom_cwrd = registry.emplace<StorageAcssCwrdComponent>(sbom_cwrd_ent);
    sbom_cwrd.acss_ref = static_cast<uint32_t>(sbom_rule_ent);
    ase::utils::str_copy(sbom_cwrd.required_cwrd, MAX_CODEWORD_LEN, EDGE_CWRD_SBOM);
    sbom_cwrd.required_cwrd_hash = entt::hashed_string(EDGE_CWRD_SBOM).value();

    // Rule 4 of 6 - release/version.json, the version manifest. A full path, so
    // most-specific-wins hands it this rule rather than release/*.
    const char* const ver_pattern = "release/version.json";
    auto ver_rule_ent = registry.create();
    auto& ver_rule = registry.emplace<StorageAcssRuleComponent>(ver_rule_ent);
    ver_rule.relm_ref = relm_ref;
    ver_rule.proj_ref = 0;
    ase::utils::str_copy(ver_rule.path_pattern, MAX_PATH_LEN, ver_pattern);
    ver_rule.protection_level = PROTECTION_PUBLIC;
    ase::utils::str_copy(ver_rule.label, MAX_LABEL_LEN, EDGE_LABEL_RELEASED);
    auto& ver_idn = registry.emplace<StorageRuleIdnComponent>(ver_rule_ent);
    ver_idn.pattern_hash = entt::hashed_string(ver_pattern).value();
    ver_idn.label_hash = entt::hashed_string(EDGE_LABEL_RELEASED).value();
    ver_idn.match_len = ase::utils::str_len(ver_pattern, MAX_PATH_LEN);
    ver_idn.match_hash = ver_idn.pattern_hash;
    auto ver_cwrd_ent = registry.create();
    auto& ver_cwrd = registry.emplace<StorageAcssCwrdComponent>(ver_cwrd_ent);
    ver_cwrd.acss_ref = static_cast<uint32_t>(ver_rule_ent);
    ase::utils::str_copy(ver_cwrd.required_cwrd, MAX_CODEWORD_LEN, EDGE_CWRD_METADATA);
    ver_cwrd.required_cwrd_hash = entt::hashed_string(EDGE_CWRD_METADATA).value();

    // Rule 5 of 6 - release/compatibility.json, the second METADATA manifest. Same
    // axis as version.json and deliberately its own rule: two manifests that happen
    // to share a codeword are still two governed assets.
    const char* const compat_pattern = "release/compatibility.json";
    auto compat_rule_ent = registry.create();
    auto& compat_rule = registry.emplace<StorageAcssRuleComponent>(compat_rule_ent);
    compat_rule.relm_ref = relm_ref;
    compat_rule.proj_ref = 0;
    ase::utils::str_copy(compat_rule.path_pattern, MAX_PATH_LEN, compat_pattern);
    compat_rule.protection_level = PROTECTION_PUBLIC;
    ase::utils::str_copy(compat_rule.label, MAX_LABEL_LEN, EDGE_LABEL_RELEASED);
    auto& compat_idn = registry.emplace<StorageRuleIdnComponent>(compat_rule_ent);
    compat_idn.pattern_hash = entt::hashed_string(compat_pattern).value();
    compat_idn.label_hash = entt::hashed_string(EDGE_LABEL_RELEASED).value();
    compat_idn.match_len = ase::utils::str_len(compat_pattern, MAX_PATH_LEN);
    compat_idn.match_hash = compat_idn.pattern_hash;
    auto compat_cwrd_ent = registry.create();
    auto& compat_cwrd = registry.emplace<StorageAcssCwrdComponent>(compat_cwrd_ent);
    compat_cwrd.acss_ref = static_cast<uint32_t>(compat_rule_ent);
    ase::utils::str_copy(compat_cwrd.required_cwrd, MAX_CODEWORD_LEN, EDGE_CWRD_METADATA);
    compat_cwrd.required_cwrd_hash = entt::hashed_string(EDGE_CWRD_METADATA).value();

    // Rule 6 of 6 - Phase-12 Task-12.3 workflow-drill asset: one designated per-asset
    // rule seeded at DRAFT so an operator can promote it draft to review to approved
    // to released live (the pattern rules above sit at RELEASED). Most-specific-wins
    // gives this full-path rule to the drill asset only; named test fixture for the
    // E2E demonstration. It is the ONLY rule below that does not carry the released
    // label - which is precisely what makes it a drill.
    const char* const drill_path = "release/linux-x86_64/ase-edge-daemon-drill";
    auto drill_rule_ent = registry.create();
    auto& drill_rule = registry.emplace<StorageAcssRuleComponent>(drill_rule_ent);
    drill_rule.relm_ref = relm_ref;
    drill_rule.proj_ref = 0;
    ase::utils::str_copy(drill_rule.path_pattern, MAX_PATH_LEN, drill_path);
    drill_rule.protection_level = PROTECTION_PUBLIC;
    ase::utils::str_copy(drill_rule.label, MAX_LABEL_LEN, EDGE_LABEL_DRAFT);
    auto& drill_idn = registry.emplace<StorageRuleIdnComponent>(drill_rule_ent);
    drill_idn.pattern_hash = entt::hashed_string(drill_path).value();
    drill_idn.label_hash = entt::hashed_string(EDGE_LABEL_DRAFT).value();
    drill_idn.match_len = ase::utils::str_len(drill_path, MAX_PATH_LEN);
    drill_idn.match_hash = drill_idn.pattern_hash;
    auto drill_cwrd_ent = registry.create();
    auto& drill_cwrd = registry.emplace<StorageAcssCwrdComponent>(drill_cwrd_ent);
    drill_cwrd.acss_ref = static_cast<uint32_t>(drill_rule_ent);
    ase::utils::str_copy(drill_cwrd.required_cwrd, MAX_CODEWORD_LEN, EDGE_CWRD_BINARY);
    drill_cwrd.required_cwrd_hash = entt::hashed_string(EDGE_CWRD_BINARY).value();

    // Publish the drill's initial workflow stage (display) AND the customer-public serving verdict
    // to the Hub. The serving verdict STG_WFLW_PUB is computed HERE, from the SAME SSOT rule as the
    // promote path (public iff label == EDGE_LABEL_RELEASED, ARCH_ASE_REASONING_EDGE Section 6.4) — draft is
    // NOT public, so the edge-download gate denies the drill until an operator promotes it to released.
    // StorageWflwTranSystem republishes both keys (same owner=hashed_string(path)) on every promote.
    // The pattern rules above stay unpublished (a pattern is not a concrete request path), so the
    // statically-released assets keep serving on the realm default — no regression.
    const uint32_t drill_owner = entt::hashed_string(drill_path).value();
    hub::set(registry, drill_owner, "STG_WFLW_STAGE"_hs, static_cast<float>(WFLW_STAGE_DRAFT));
    hub::set(registry, drill_owner, "STG_WFLW_PUB"_hs,
             EDGE_LABEL_DRAFT_HASH == EDGE_LABEL_RELEASED_HASH ? 1.0f : 0.0f);
    log::info("[StorageEdgeIni] edge ACL rules ready (BINARY release/*, SIG *.sig, SBOM *.spdx.json, METADATA manifests; drill asset at draft)");
}

void StorageAcssEdgeIniSystem::tick(ecs::Registry& /*registry*/, float /*dt*/) {
    // Initialization system has no per-tick logic
}

void StorageAcssEdgeIniSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageAcssEdgeIni] Stopped");
}

}  // namespace ase::storage
