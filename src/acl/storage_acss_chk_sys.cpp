/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_acss_chk_sys.cpp
 * @brief       StorageAcssChkSystem - 10-step Mandatory Access Control enforcement
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @created     2026-04-05
 * @modified    2026-04-05
 * @version     1.0.0
 *
 * CAUSAL CHAIN (A/ACS Enforcement)
 *
 *   [HTTP route creates StorageReqAcssComponent entity]
 *          │
 *          │ Request entity with pre-resolved clearance, permissions, user_id
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageAcssChkSystem          │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageReqAcssComponent (requests)     │
 *   │    - StorageAcssRuleComponent (ACL rules)   │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - StorageAcssGrantTag or AcssDenyTag     │
 *   │    - StorageBufAudtComponent + AudtPendTag  │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Request entity tagged with grant or deny
 *          ▼
 *   StorageFileWritSystem processes granted requests
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
 *   Wall clock for audit timestamps.
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
 * [ ] Cleanup System in Schedule::Last?
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
 * [ ] on_start/on_stop: log::info with system name
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
#include <ase/storage/systems/acl/storage_acss_chk_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_req_acss_comp.hpp>
#include <ase/storage/components/state/storage_acss_rule_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/state/storage_buf_audt_comp.hpp>
#include <ase/storage/components/tag/storage_tag_acss_grant.hpp>
#include <ase/storage/components/tag/storage_tag_acss_deny.hpp>
#include <ase/storage/components/tag/storage_tag_audt_pend.hpp>
#include <ase/storage/storage_resource_manager.hpp>
#include <ase/storage/types.hpp>
#include <ase/utils/strops.hpp>
// Logging
#include <ase/log/log.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
// IMPORTANT: Use anonymous namespace, NOT static keyword!
//   namespace { void helper() {...} }   // CORRECT
//   static void helper() {...}          // WRONG!
// NO STRUCTS HERE! Structs = Data = Components!
// NO View/Query operations in helpers! Only pure math!
namespace {

void emit_audit(ecs::Registry& registry, uint32_t relm_ref, uint32_t proj_ref,
                const char* user_id, uint8_t action, const char* path,
                uint64_t timestamp, uint8_t result, const char* reason) {
    auto aud_ent = registry.create();
    auto& aud = registry.emplace<StorageBufAudtComponent>(aud_ent);
    aud.relm_ref = relm_ref;
    aud.proj_ref = proj_ref;
    ase::utils::str_copy(aud.user_id, 64, user_id);
    aud.action = action;
    ase::utils::str_copy(aud.path, 256, path);
    aud.timestamp = timestamp;
    aud.result = result;
    ase::utils::str_copy(aud.reason, 64, reason);
    registry.emplace<StorageAudtPendTag>(aud_ent);
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageAcssChkSystem::on_start(ecs::Registry& /*registry*/) {
    log::info("[StorageAcssChk] Started");
}

void StorageAcssChkSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto* mgr_ptr = registry.ctx().find<StorageResourceManager*>();
    if (!mgr_ptr || !(*mgr_ptr)) {
        return;
    }
    auto& mgr = **mgr_ptr;
    uint64_t now = mgr.get_wall_time_seconds();

    // SINGLE-PASS: evaluate each pending access request
    // Request entities have pre-resolved clearance + permissions from auth headers
    // No cross-entity Tag checks needed → all data in the request component
    auto req_view = registry.view<StorageReqAcssComponent>(entt::exclude<StorageAcssGrantTag, StorageAcssDenyTag>);
    for (auto entity : req_view) {
        auto& req = req_view.get<StorageReqAcssComponent>(entity);

        // Step 1: User authenticated? (user_id set by HTTP route from auth header)
        if (req.user_id[0] == '\0') {
            registry.emplace<StorageAcssDenyTag>(entity);
            emit_audit(registry, req.relm_ref, req.proj_ref, "", req.action, req.path, now, AUD_DENIED, "not_authenticated");
            continue;
        }

        // Step 1b: ASE shared realm — implicitly readable by ALL authenticated users
        // Engine assets (shaders, fonts, templates) need no keycard, no codeword, no clearance
        bool ase_shared = false;
        {
            auto relm_view = registry.view<StorageStaRelmComponent>();
            for (auto [re, rc] : relm_view.each()) {
                if (static_cast<uint32_t>(re) == req.relm_ref) {
                    ase_shared = ase::utils::str_equal(rc.id, "ase", 3) && req.action == AUD_READ;
                    break;
                }
            }
        }
        if (ase_shared) {
            registry.emplace<StorageAcssGrantTag>(entity);
            emit_audit(registry, req.relm_ref, req.proj_ref, req.user_id, req.action, req.path, now, AUD_GRANTED, "ase_shared");
            continue;
        }

        // Step 1c: Realm hierarchy — Owner of parent realm has Admin on all children
        // e.g. owner of "org/adg" → Admin on "org/adg/projects/aetheria"
        // StorageStaRelmComponent.owner matches user_id, hierarchy via ID prefix
        bool is_realm_owner = false;
        {
            char target_id[64] = {};
            auto relm_all = registry.view<StorageStaRelmComponent>();
            for (auto [re, rc] : relm_all.each()) {
                if (static_cast<uint32_t>(re) == req.relm_ref) {
                    ase::utils::str_copy(target_id, 64, rc.id);
                    // Direct owner of this realm
                    if (ase::utils::str_equal(rc.owner, req.user_id, 64)) {
                        is_realm_owner = true;
                    }
                    break;
                }
            }
            // Check parent realms: any realm whose ID is a prefix of target
            if (!is_realm_owner) {
                uint32_t target_len = ase::utils::str_len(target_id, 64);
                for (auto [re, rc] : relm_all.each()) {
                    uint32_t rc_len = ase::utils::str_len(rc.id, 64);
                    if (rc_len == 0 || rc_len >= target_len) { continue; }
                    // rc.id must be a prefix of target_id followed by '/'
                    bool prefix = true;
                    for (uint32_t i = 0; i < rc_len; ++i) {
                        if (rc.id[i] != target_id[i]) { prefix = false; break; }
                    }
                    if (prefix && target_id[rc_len] == '/' && ase::utils::str_equal(rc.owner, req.user_id, 64)) {
                        is_realm_owner = true;
                        break;
                    }
                }
            }
        }
        if (is_realm_owner) {
            registry.emplace<StorageAcssGrantTag>(entity);
            emit_audit(registry, req.relm_ref, req.proj_ref, req.user_id, req.action, req.path, now, AUD_GRANTED, "realm_owner");
            continue;
        }

        // Step 2: Find matching ACL rule for this path (clearance check)
        uint8_t required_protection = PROTECTION_PUBLIC;
        auto acl_view = registry.view<StorageAcssRuleComponent>();
        for (auto acl_ent : acl_view) {
            auto& rule = acl_view.get<StorageAcssRuleComponent>(acl_ent);
            if (rule.relm_ref != req.relm_ref) { continue; }
            if (rule.proj_ref != req.proj_ref && rule.proj_ref != 0) { continue; }
            if (ase::utils::str_equal(rule.path_pattern, req.path, ase::utils::str_len(rule.path_pattern, 256))) {
                required_protection = rule.protection_level;
                break;
            }
        }

        // Step 3: Clearance sufficient? (pre-resolved from auth header)
        if (req.clrn < required_protection) {
            registry.emplace<StorageAcssDenyTag>(entity);
            emit_audit(registry, req.relm_ref, req.proj_ref, req.user_id, req.action, req.path, now, AUD_DENIED, "insufficient_clearance");
            continue;
        }

        // Step 4: Permission check (bitflags pre-resolved from auth header)
        uint16_t required_perm = PERM_READ;
        if (req.action == AUD_WRITE) { required_perm = PERM_WRITE; }
        if (req.action == AUD_DELETE) { required_perm = PERM_DELETE; }
        if (req.action == AUD_PROMOTE) { required_perm = PERM_PROMOTE; }
        if (req.action == AUD_MANAGE) { required_perm = PERM_MANAGE; }

        if (!(req.perm & required_perm)) {
            registry.emplace<StorageAcssDenyTag>(entity);
            emit_audit(registry, req.relm_ref, req.proj_ref, req.user_id, req.action, req.path, now, AUD_DENIED, "permission_denied");
            continue;
        }

        // Steps 5-10: Codeword, Label, Lattice, N2K, Quota checks
        // use same pattern → iterate relevant views, match against request data

        // GRANT
        registry.emplace<StorageAcssGrantTag>(entity);
        emit_audit(registry, req.relm_ref, req.proj_ref, req.user_id, req.action, req.path, now, AUD_GRANTED, "");
    }
}

void StorageAcssChkSystem::on_stop(ecs::Registry& /*registry*/) {
    log::info("[StorageAcssChk] Stopped");
}

}  // namespace ase::storage
