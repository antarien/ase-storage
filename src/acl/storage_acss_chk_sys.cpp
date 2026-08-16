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
 * @modified    2026-06-24
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
 *   │    - StorageStaRelmComponent (realms)       │
 *   │    - StorageAcssRuleComponent (ACL rules)   │
 *   │    - StorageAcssCwrdComponent (codewords)   │
 *   │    - StorageStaKycdComponent (keycards)     │
 *   │    - StorageKycdCwrdComponent (held cwrds)  │
 *   │    - StorageLatLnkComponent (lattice links) │
 *   │    - StorageStaTaskComponent (need-to-know) │
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
#include <ase/storage/systems/acl/storage_acss_chk_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_req_acss_comp.hpp>
#include <ase/storage/components/state/storage_acss_rule_comp.hpp>
#include <ase/storage/components/state/storage_acss_cwrd_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/state/storage_sta_kycd_comp.hpp>
#include <ase/storage/components/state/storage_kycd_cwrd_comp.hpp>
#include <ase/storage/components/state/storage_lat_lnk_comp.hpp>
#include <ase/storage/components/state/storage_sta_task_comp.hpp>
#include <ase/storage/components/state/storage_relm_idn_comp.hpp>
#include <ase/storage/components/state/storage_rule_idn_comp.hpp>
#include <ase/storage/components/state/storage_kycd_idn_comp.hpp>
#include <ase/storage/components/state/storage_lnk_idn_comp.hpp>
#include <ase/storage/components/state/storage_task_idn_comp.hpp>
#include <ase/storage/components/state/storage_buf_audt_comp.hpp>
#include <ase/storage/components/tag/storage_tag_acss_grant.hpp>
#include <ase/storage/components/tag/storage_tag_acss_deny.hpp>
#include <ase/storage/components/tag/storage_tag_audt_pend.hpp>
#include <ase/storage/components/tag/storage_tag_relm_conceal.hpp>
#include <ase/storage/components/tag/storage_tag_relm_public.hpp>
#include <ase/storage/storage_resource_manager.hpp>
#include <ase/storage/storage_acss_index_resource_manager.hpp>
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

// The rolling fold below must produce the SAME numbers entt produces, because the rule
// side stores entt hashes. Asserting it at compile time turns a silent divergence - the
// kind that denies every access and looks like a data problem - into a build failure.
static_assert(((ACSS_FNV_OFFSET ^ static_cast<uint32_t>('a')) * ACSS_FNV_PRIME)
                  == entt::hashed_string::value("a", 1),
              "ACSS_FNV_* no longer match entt::hashed_string");

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

// Hash of EVERY prefix of the path, in one pass: out[L] is the hash of path[0..L).
//
// FNV-1a folds left to right - hash = (hash ^ c) * prime - so the running value after L
// characters IS the hash of the L-character prefix. One walk of the path therefore
// yields every prefix hash a rule could ask about, and a location rule is then a single
// 32-bit equality. Before, every rule walked the path again.
//
// The semantics are the ones the character comparison had, EXACTLY: a location rule
// matched when the path began with the pattern, the pattern's own trailing wildcard
// character included. Nothing here widens or narrows that - a refactor that quietly
// changed which assets a rule governs would be a change to access control disguised as
// a performance fix.
void path_prefix_hashes(const char* path, uint32_t path_len, uint32_t* out) {
    uint32_t running = ACSS_FNV_OFFSET;
    out[0] = running;
    for (uint32_t i = 0; i < path_len; ++i) {
        running = (running ^ static_cast<uint32_t>(path[i])) * ACSS_FNV_PRIME;
        out[i + 1u] = running;
    }
}

// Hashes of every EXTENSION an asset path ends with: each suffix beginning at a '.'.
// "build-1.2.3.spdx.json" yields ".json", ".spdx.json", ".3.spdx.json" and so on, so an
// extension rule finds itself no matter how many dots the version number carries.
uint32_t path_extension_hashes(const char* path, uint32_t path_len, uint32_t* out_hash,
                               uint32_t* out_len, uint32_t max_out) {
    uint32_t count = 0;
    for (uint32_t at = 0; at < path_len && count < max_out; ++at) {
        if (path[at] != '.') { continue; }
        const uint32_t len = path_len - at;
        out_hash[count] = entt::hashed_string::value(path + at, len);
        out_len[count] = len;
        ++count;
    }
    return count;
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageAcssChkSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageAcssChk] Started");
}

void StorageAcssChkSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto* mgr_ptr = registry.ctx().find<StorageResourceManager*>();
    if (!mgr_ptr || !(*mgr_ptr)) {
        return;
    }
    auto& mgr = **mgr_ptr;
    uint64_t now = mgr.get_wall_time_seconds();

    auto* idx_ptr = registry.ctx().find<StorageAcssIndexResourceManager*>();
    if (!idx_ptr || !(*idx_ptr)) {
        log::error("[StorageAcssChk] StorageAcssIndexResourceManager not in ctx (StorageAcssIdxSystem must run first)");
        return;
    }
    auto& idx = **idx_ptr;

    // SINGLE-PASS: evaluate each pending access request through the canonical Section 14.1 ladder.
    // Request entities carry clearance + permissions pre-resolved from the validated keycard.
    // The ladder is strictly ordered and every GRANT happens at step 10 ONLY. There are no
    // pre-ladder shortcut grants: the public realm and the realm-owner power are modelled
    // INSIDE the ladder (PUBLIC protection rule + owner keycard preset) so they too pass
    // through clearance/codeword/permission/label/need-to-know/quota.
    // Tag-filtered classification views, built ONCE. A request names its realm by entity
    // id, so the classification is a membership test on the filtered view — O(1), and the
    // tag stays where the ECS rules want it: in the View filter, never in an all_of<Tag>
    // runtime check. Both were per-request scans over every realm before (WS-K.2c).
    auto pub_view = registry.view<StorageStaRelmComponent, StorageRelmPublicTag>();
    auto cnc_view = registry.view<StorageStaRelmComponent, StorageRelmConcealTag>();

    auto req_view = registry.view<StorageReqAcssComponent>(entt::exclude<StorageAcssGrantTag, StorageAcssDenyTag>);
    for (auto entity : req_view) {
        auto& req = req_view.get<StorageReqAcssComponent>(entity);

        // ── Step 1: KEYCARD VALID ─ authenticated identity present
        // user_id is set by the HTTP route from the keycard JWT (validated by
        // StorageKycdVldSystem); an empty user_id means no valid keycard reached here.
        if (req.user_id[0] == '\0') {
            registry.emplace<StorageAcssDenyTag>(entity);
            emit_audit(registry, req.relm_ref, req.proj_ref, "", req.action, req.path, now, AUD_DENIED, "not_authenticated");
            continue;
        }

        // Action → required permission bitflag (used by step 3 lattice and step 6 permission)
        uint16_t required_perm = PERM_READ;
        if (req.action == AUD_WRITE)   { required_perm = PERM_WRITE; }
        if (req.action == AUD_DELETE)  { required_perm = PERM_DELETE; }
        if (req.action == AUD_PROMOTE) { required_perm = PERM_PROMOTE; }
        if (req.action == AUD_MANAGE)  { required_perm = PERM_MANAGE; }

        // ── Step 2: REALM MEMBERSHIP + CONCEALMENT ─ resolve the target realm core data
        char     target_id[MAX_REALM_ID] = {};
        char     target_owner[MAX_OWNER_ID] = {};
        uint8_t  target_tier = TIER_INDIE;
        bool     realm_found = false;
        bool     public_realm = false;
        bool     owner_preset = false;
        // req.relm_ref IS the realm's entity id, so the realm is reached directly. The
        // former version scanned EVERY realm and compared each entity id against the one
        // it already held — a linear search for a lookup the registry answers in O(1),
        // and it ran once per request (WS-K.2c).
        const auto relm_ent = static_cast<ecs::Entity>(req.relm_ref);
        // The requester's identity, hashed ONCE per request. Every identity test below
        // is a 32-bit equality against this value: identity is a lookup, and a lookup
        // compares hashes, never characters (WRFL_ASE_STRING_HANDLING Section 3).
        const uint32_t user_hash = entt::hashed_string(req.user_id).value();
        if (auto* rc = registry.try_get<StorageStaRelmComponent>(relm_ent)) {
            realm_found = true;
            ase::utils::str_copy(target_id, MAX_REALM_ID, rc->id);
            ase::utils::str_copy(target_owner, MAX_OWNER_ID, rc->owner);
            target_tier = rc->tier;
            // Identity lives beside the record. A realm without it cannot be classified
            // at all, so its absence is reported rather than silently treated as private.
            auto* rc_idn = registry.try_get<StorageRelmIdnComponent>(relm_ent);
            if (rc_idn == nullptr) {
                log::error(log::ERR::CAT::COMPONENT_MISSING, "StorageAcssChkSystem",
                           req.relm_ref, "StorageRelmIdnComponent");
            } else {
                // Public 'ase' realm by id is the in-ladder PUBLIC protection source
                public_realm = rc_idn->id_hash == ACSS_REALM_PUBLIC_HASH;
                // Direct owner of this realm → owner keycard preset (ARCH :819)
                owner_preset = rc_idn->owner_hash == user_hash;
            }
        }
        // Public realm classification via the tag on THAT realm (a realm carrying the
        // StorageRelmPublicTag is public regardless of its id naming).
        if (realm_found && !public_realm) {
            public_realm = pub_view.contains(relm_ent);
        }
        // Parent-realm ownership: owner of "org/adg" governs "org/adg/projects/x".
        // The condition the scan tested - rc.id is a prefix of target_id AND the next
        // character is '/' - is exactly "rc.id is a path ANCESTOR of target_id", and the
        // ancestors of a path can be read off the path itself. Enumerating them costs
        // path depth and hits the index once per step, where the scan cost one pass over
        // every realm per request (WS-K.2c).
        if (realm_found && !owner_preset) {
            const uint32_t target_len = ase::utils::str_len(target_id, MAX_REALM_ID);
            char ancestor[MAX_REALM_ID] = {};
            for (uint32_t cut = 1u; cut < target_len && !owner_preset; ++cut) {
                if (target_id[cut] != '/') { continue; }
                for (uint32_t i = 0; i < cut; ++i) { ancestor[i] = target_id[i]; }
                ancestor[cut] = '\0';
                const uint32_t ancestor_hash = entt::hashed_string(ancestor).value();
                const uint32_t cand = idx.get_realm(static_cast<uint64_t>(ancestor_hash));
                if (cand == INVALID_ENTITY) { continue; }
                auto* rc_idn =
                    registry.try_get<StorageRelmIdnComponent>(static_cast<ecs::Entity>(cand));
                if (rc_idn == nullptr) { continue; }
                // Identity is the hash. The bucket key and the stored id_hash are the
                // same number, so agreeing on it IS the confirmation - there is nothing
                // a character comparison would add beyond the cost of walking the string.
                if (rc_idn->id_hash != ancestor_hash) { continue; }
                if (rc_idn->owner_hash == user_hash) {
                    owner_preset = true;
                }
            }
        }
        // Concealment via Tag-filtered View: a concealed realm is invisible to non-owners.
        // Public realms are never concealed; the owner-preset always sees its own realm.
        bool concealed = false;
        if (realm_found && !public_realm && !owner_preset) {
            concealed = cnc_view.contains(relm_ent);
        }
        if (!realm_found || concealed) {
            // Concealment leaks nothing: deny as realm_not_found, never access_denied.
            registry.emplace<StorageAcssDenyTag>(entity);
            emit_audit(registry, req.relm_ref, req.proj_ref, req.user_id, req.action, req.path, now, AUD_DENIED, "realm_not_found");
            continue;
        }

        // Effective keycard attributes for the rest of the ladder. The owner preset is the
        // in-ladder model of former realm_owner power: clearance 9, all permissions, wildcard
        // codeword. Without the preset, the auth-header values are used verbatim. The public
        // realm needs no boost — its PUBLIC protection rule lets the auth-header values pass.
        (void)target_owner;
        uint8_t  eff_clrn = owner_preset ? ACSS_OWNER_CLEARANCE : req.clrn;
        uint16_t eff_perm = owner_preset ? ACSS_OWNER_PERMS     : req.perm;

        // ── Step 2 (cont.): match the ACL rule for this path ─ clearance/label/codeword src.
        // The public realm contributes an implicit PUBLIC protection rule (level 0, no
        // codewords) so engine defaults stay readable by every authenticated user — this is
        // the in-ladder replacement of the old pre-ladder "ase_shared" grant.
        uint8_t  required_protection = PROTECTION_PUBLIC;
        uint32_t matched_rule = INVALID_ENTITY;
        uint32_t rule_label_hash = 0;
        // The request's own prefixes and extensions, hashed ONCE. Three separate gates
        // below ask "does this path lie under X" - the ACL rules, the lattice share and
        // the need-to-know scope - and all three read these same numbers.
        uint32_t pfx_hash[MAX_PATH_LEN + 1] = {};
        uint32_t ext_hash[ACSS_MAX_PATH_PARTS] = {};
        uint32_t ext_len[ACSS_MAX_PATH_PARTS] = {};
        const uint32_t path_len = ase::utils::str_len(req.path, MAX_PATH_LEN);
        path_prefix_hashes(req.path, path_len, pfx_hash);
        const uint32_t ext_n = path_extension_hashes(req.path, path_len, ext_hash,
                                                     ext_len, ACSS_MAX_PATH_PARTS);
        {
            // Pattern semantics (types.hpp ACSS_MATCH_SUFFIX_BONUS): an extension rule
            // ("*.sig" governs companion artifacts that sit BESIDE binaries, where no
            // location can reach them) carries StorageAcssRuleSufxTag; every other rule
            // is a location rule. The MOST SPECIFIC rule wins (extension over location,
            // longer literal over shorter) — never first-iteration order, which is
            // storage-order dependent and would let a broad location rule swallow the
            // "*.sig" codeword requirement.
            //
            // The request's own locations and extensions are hashed ONCE, above; each
            // rule is then one 32-bit equality against them. The two kinds are matched by
            // two different computations, so they are two loops over two Tag-separated
            // buckets - never one loop that reads a discriminator and branches.
            uint32_t best_score = 0;
            const uint32_t loc_rule_count = idx.get_location_rule_count(req.relm_ref);
            for (uint32_t rule_index = 0; rule_index < loc_rule_count; ++rule_index) {
                const uint32_t acl_id = idx.get_location_rule(req.relm_ref, rule_index);
                if (acl_id == INVALID_ENTITY) { continue; }
                const auto acl_ent = static_cast<ecs::Entity>(acl_id);
                auto* rule_ptr = registry.try_get<StorageAcssRuleComponent>(acl_ent);
                if (rule_ptr == nullptr) { continue; }
                auto& rule = *rule_ptr;
                // rule.relm_ref == req.relm_ref holds by construction of the bucket.
                if (rule.proj_ref != req.proj_ref && rule.proj_ref != 0) { continue; }
                auto* rule_idn = registry.try_get<StorageRuleIdnComponent>(acl_ent);
                if (rule_idn == nullptr) {
                    log::error(log::ERR::CAT::COMPONENT_MISSING, "StorageAcssChkSystem",
                               acl_id, "StorageRuleIdnComponent");
                    continue;
                }
                // The path must be at least as long as the pattern and must begin with
                // it - the same condition the character comparison enforced, now one
                // 32-bit equality against the prefix hash of exactly that length.
                if (rule_idn->match_len < 1u) { continue; }
                if (rule_idn->match_len > path_len) { continue; }
                if (pfx_hash[rule_idn->match_len] != rule_idn->match_hash) { continue; }
                const uint32_t score = 1u + rule_idn->match_len;
                if (score <= best_score) { continue; }
                best_score = score;
                required_protection = rule.protection_level;
                matched_rule = acl_id;
                rule_label_hash = rule_idn->label_hash;
            }

            const uint32_t ext_rule_count = idx.get_extension_rule_count(req.relm_ref);
            for (uint32_t rule_index = 0; rule_index < ext_rule_count; ++rule_index) {
                const uint32_t acl_id = idx.get_extension_rule(req.relm_ref, rule_index);
                if (acl_id == INVALID_ENTITY) { continue; }
                const auto acl_ent = static_cast<ecs::Entity>(acl_id);
                auto* rule_ptr = registry.try_get<StorageAcssRuleComponent>(acl_ent);
                if (rule_ptr == nullptr) { continue; }
                auto& rule = *rule_ptr;
                if (rule.proj_ref != req.proj_ref && rule.proj_ref != 0) { continue; }
                auto* rule_idn = registry.try_get<StorageRuleIdnComponent>(acl_ent);
                if (rule_idn == nullptr) {
                    log::error(log::ERR::CAT::COMPONENT_MISSING, "StorageAcssChkSystem",
                               acl_id, "StorageRuleIdnComponent");
                    continue;
                }
                if (rule_idn->match_len < 1u) { continue; }
                bool match = false;
                for (uint32_t i = 0; i < ext_n; ++i) {
                    if (ext_hash[i] == rule_idn->match_hash &&
                        ext_len[i] == rule_idn->match_len) { match = true; break; }
                }
                if (!match) { continue; }
                // Score parity with the character version: it scored on the FULL pattern
                // length, wildcard included, which is one more than the literal.
                const uint32_t score = ACSS_MATCH_SUFFIX_BONUS + rule_idn->match_len + 1u;
                if (score <= best_score) { continue; }
                best_score = score;
                required_protection = rule.protection_level;
                matched_rule = acl_id;
                rule_label_hash = rule_idn->label_hash;
            }
        }

        // ── Step 3: LATTICE ─ cross-realm access requires a valid, bilateral link.
        // A request whose path lies in another realm's shared prefix is only admissible
        // through an approved, unexpired lattice link; the link caps clearance and perms.
        // Owner-preset access stays within the owner's own realm hierarchy (no link needed).
        if (!owner_preset) {
            bool lattice_required = false;
            bool lattice_ok       = false;
            // Links are indexed under the realm they name as target. The bucket key and
            // the link's own target_realm_hash are the same number, so agreeing on it IS
            // the confirmation - a character comparison would only re-walk the string.
            const uint32_t target_hash = entt::hashed_string(target_id).value();
            const uint32_t link_count = idx.get_link_count(static_cast<uint64_t>(target_hash));
            for (uint32_t link_index = 0; link_index < link_count; ++link_index) {
                const uint32_t link_id =
                    idx.get_link(static_cast<uint64_t>(target_hash), link_index);
                if (link_id == INVALID_ENTITY) { continue; }
                const auto link_ent = static_cast<ecs::Entity>(link_id);
                auto* link_ptr = registry.try_get<StorageLatLnkComponent>(link_ent);
                if (link_ptr == nullptr) { continue; }
                auto& link = *link_ptr;
                auto* link_idn = registry.try_get<StorageLnkIdnComponent>(link_ent);
                if (link_idn == nullptr) {
                    log::error(log::ERR::CAT::COMPONENT_MISSING, "StorageAcssChkSystem",
                               link_id, "StorageLnkIdnComponent");
                    continue;
                }
                if (link_idn->target_realm_hash != target_hash) { continue; }
                if (link_idn->prefix_len < 1u) { continue; }
                if (link_idn->prefix_len > path_len) { continue; }
                if (pfx_hash[link_idn->prefix_len] != link_idn->prefix_hash) { continue; }
                lattice_required = true;
                bool approved = link.approved_by_source != 0 && link.approved_by_target != 0;
                bool live     = link.expires_at == 0 || link.expires_at > now;
                bool perm_ok  = (link.permissions & required_perm) != 0;
                bool clrn_ok  = required_protection <= link.max_clearance;
                if (approved && live && perm_ok && clrn_ok) {
                    lattice_ok = true;
                    break;
                }
            }
            if (lattice_required && !lattice_ok) {
                registry.emplace<StorageAcssDenyTag>(entity);
                emit_audit(registry, req.relm_ref, req.proj_ref, req.user_id, req.action, req.path, now, AUD_DENIED, "no_lattice_link");
                continue;
            }
        }

        // ── Step 4: CLEARANCE ─ vertical Schutzstufe gate (public realm rule keeps PUBLIC)
        if (eff_clrn < required_protection) {
            registry.emplace<StorageAcssDenyTag>(entity);
            emit_audit(registry, req.relm_ref, req.proj_ref, req.user_id, req.action, req.path, now, AUD_DENIED, "insufficient_clearance");
            continue;
        }

        // ── Step 5: CODEWORD ─ horizontal gate; keycard must hold EVERY required codeword.
        // The owner-preset wildcard satisfies any requirement; a held "ALL" codeword too.
        // The public realm carries no required codewords, so public reads pass unaffected.
        if (matched_rule != INVALID_ENTITY && !owner_preset) {
            // Three nested walks - required codewords, the user's keycards, and the
            // codewords each keycard carries - collapse into one walk over the rule's
            // own requirements plus four membership questions. The four cases are the
            // same the nested version tested: the keycard may be scoped to this realm or
            // global (scope 0), and a held wildcard satisfies any requirement.
            bool missing_codeword = false;
            const uint32_t required_count = idx.get_required_count(matched_rule);
            for (uint32_t required_index = 0; required_index < required_count; ++required_index) {
                const uint32_t cwrd_id = idx.get_required(matched_rule, required_index);
                if (cwrd_id == INVALID_ENTITY) { continue; }
                auto* required =
                    registry.try_get<StorageAcssCwrdComponent>(static_cast<ecs::Entity>(cwrd_id));
                if (required == nullptr) { continue; }
                const uint32_t needed = required->required_cwrd_hash;
                const bool held =
                    idx.has_held(StorageAcssIndexResourceManager::compose_held_key(
                        user_hash, req.relm_ref, needed)) ||
                    idx.has_held(StorageAcssIndexResourceManager::compose_held_key(
                        user_hash, 0u, needed)) ||
                    idx.has_held(StorageAcssIndexResourceManager::compose_held_key(
                        user_hash, req.relm_ref, ACSS_CWRD_WILDCARD_HASH)) ||
                    idx.has_held(StorageAcssIndexResourceManager::compose_held_key(
                        user_hash, 0u, ACSS_CWRD_WILDCARD_HASH));
                if (!held) { missing_codeword = true; break; }
            }
            if (missing_codeword) {
                registry.emplace<StorageAcssDenyTag>(entity);
                emit_audit(registry, req.relm_ref, req.proj_ref, req.user_id, req.action, req.path, now, AUD_DENIED, "missing_codeword");
                continue;
            }
        }

        // ── Step 6: PERMISSION ─ action bitflag gate (owner preset holds all flags)
        if (!(eff_perm & required_perm)) {
            registry.emplace<StorageAcssDenyTag>(entity);
            emit_audit(registry, req.relm_ref, req.proj_ref, req.user_id, req.action, req.path, now, AUD_DENIED, "permission_denied");
            continue;
        }

        // ── Step 7: LABEL ─ workflow-status gate.
        // retired = withdrawn build (no access); draft/review = team-only (clearance >= TEAM).
        if (rule_label_hash == EDGE_LABEL_RETIRED_HASH) {
            registry.emplace<StorageAcssDenyTag>(entity);
            emit_audit(registry, req.relm_ref, req.proj_ref, req.user_id, req.action, req.path, now, AUD_DENIED, "retired_asset");
            continue;
        }
        if ((rule_label_hash == EDGE_LABEL_DRAFT_HASH ||
             rule_label_hash == EDGE_LABEL_REVIEW_HASH) &&
            eff_clrn < PROTECTION_TEAM) {
            registry.emplace<StorageAcssDenyTag>(entity);
            emit_audit(registry, req.relm_ref, req.proj_ref, req.user_id, req.action, req.path, now, AUD_DENIED, "label_restricted");
            continue;
        }

        // ── Step 8: NEED-TO-KNOW ─ active task scoping (Enterprise).
        // When the assignee has any active need-to-know task in this project, access is
        // restricted to that task's path scope. Owner-preset governs all and is exempt.
        if (!owner_preset && req.proj_ref != 0) {
            bool has_active_task = false;
            bool path_in_scope   = false;
            // Tasks are indexed under their project, so the project filter is the bucket
            // itself and only the assignee still has to be compared.
            const uint32_t task_count = idx.get_task_count(req.proj_ref);
            for (uint32_t task_index = 0; task_index < task_count; ++task_index) {
                const uint32_t task_id = idx.get_task(req.proj_ref, task_index);
                if (task_id == INVALID_ENTITY) { continue; }
                auto* task_ptr =
                    registry.try_get<StorageStaTaskComponent>(static_cast<ecs::Entity>(task_id));
                if (task_ptr == nullptr) { continue; }
                auto& task = *task_ptr;
                auto* task_idn =
                    registry.try_get<StorageTaskIdnComponent>(static_cast<ecs::Entity>(task_id));
                if (task_idn == nullptr) {
                    log::error(log::ERR::CAT::COMPONENT_MISSING, "StorageAcssChkSystem",
                               task_id, "StorageTaskIdnComponent");
                    continue;
                }
                if (task_idn->assignee_hash != user_hash) { continue; }
                bool live = (task.starts_at == 0 || task.starts_at <= now) &&
                            (task.expires_at == 0 || task.expires_at > now);
                if (!live) { continue; }
                has_active_task = true;
                if (task_idn->scope_len < 1u) { continue; }
                if (task_idn->scope_len > path_len) { continue; }
                if (pfx_hash[task_idn->scope_len] == task_idn->scope_hash) {
                    path_in_scope = true;
                    break;
                }
            }
            if (has_active_task && !path_in_scope) {
                registry.emplace<StorageAcssDenyTag>(entity);
                emit_audit(registry, req.relm_ref, req.proj_ref, req.user_id, req.action, req.path, now, AUD_DENIED, "need_to_know");
                continue;
            }
        }

        // ── Step 9: QUOTA ─ realm storage budget gate (WRITE only).
        // A write is refused when the realm's measured usage already meets its tier limit.
        if (req.action == AUD_WRITE) {
            uint64_t tier_limit = QUOTA_INDIE_STORAGE;
            if (target_tier == TIER_PRO)        { tier_limit = QUOTA_PRO_STORAGE; }
            if (target_tier == TIER_ENTERPRISE) { tier_limit = QUOTA_ENT_STORAGE; }
            uint64_t used = mgr.get_realm_usage(target_id);
            if (used >= tier_limit) {
                registry.emplace<StorageAcssDenyTag>(entity);
                emit_audit(registry, req.relm_ref, req.proj_ref, req.user_id, req.action, req.path, now, AUD_DENIED, "quota_exceeded");
                continue;
            }
        }

        // ── Step 10: GRANT + AUDIT(GRANTED) ─ reached only after every applicable step passed
        registry.emplace<StorageAcssGrantTag>(entity);
        emit_audit(registry, req.relm_ref, req.proj_ref, req.user_id, req.action, req.path, now, AUD_GRANTED, "");
    }
}

void StorageAcssChkSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageAcssChk] Stopped");
}

}  // namespace ase::storage
