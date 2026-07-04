/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_relm_proj_sync_sys.cpp
 * @brief       StorageRelmProjSyncSystem - one A/ACS realm per Engine project (owner = project owner)
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
 * @created     2026-07-04
 * @modified    2026-07-04
 * @version     1.0.0
 *
 * CAUSAL CHAIN (Per-Project Realm Sync)
 *
 *   [Project hub pipeline publishes ENG_PROJ_* + <proj_id>_owner almanach names]
 *          │
 *          │ Engine Hub (owner 0): ENG_PROJ_COUNT / ENG_PROJ_<i> / <proj_id>_owner
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageRelmProjSyncSystem     │
 *   │                                             │
 *   │  READS (from Hub):                          │
 *   │    - ENG_PROJ_COUNT (owner 0)               │
 *   │    - ENG_PROJ_<i> name → proj_id            │
 *   │    - <proj_id>_owner name → owner user id   │
 *   │    - ENG_PROJ_TIER (owner proj_hash)        │
 *   │                                             │
 *   │  WRITES (Components):                        │
 *   │    - StorageStaRelmComponent (id, owner)    │
 *   │    - StorageRelm{Personal,Active,Conceal}Tag│
 *   │    - StorageAcssRuleComponent (credentials) │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ realm.owner = project owner → owner-preset grants owner,
 *          │ conceal-tag denies non-owner (realm_not_found)
 *          ▼
 *   StorageAcssChkSystem resolves the project realm for a credential path.
 *
 * HUB Pattern (Active - consumes the Engine project hub)
 *
 * READS (from Hub):
 *   ENG_PROJ_COUNT        — live project count (owner 0)
 *   ENG_PROJ_<i>          — almanach name = project id string (i in [0, count))
 *   <proj_id>_owner       — almanach name = owner user id string
 *   ENG_PROJ_TIER         — project tier (owner = hashed_string(proj_id))
 *
 * WRITES (to Hub):
 *   (none — creates ECS realm + ACL-rule entities, not Hub values)
 *
 * FLYWEIGHT PATTERN (N/A — no external resource handles)
 *   Realm/rule data live in char[]/primitive component fields, no ctx handles.
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
// Own header FIRST
#include <ase/storage/systems/acl/storage_relm_proj_sync_sys.hpp>
// Components + tags from same module
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/state/storage_acss_rule_comp.hpp>
#include <ase/storage/components/tag/storage_tag_relm_personal.hpp>
#include <ase/storage/components/tag/storage_tag_relm_active.hpp>
#include <ase/storage/components/tag/storage_tag_relm_conceal.hpp>
#include <ase/storage/types.hpp>
// Lower layers
#include <ase/hub/api.hpp>
#include <ase/types/types.hpp>
#include <ase/utils/strops.hpp>
#include <ase/log/log.hpp>

#include <cstdint>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (pure string builds, no View/Query).
namespace {

// Append the decimal encoding of n to out (bounded, NUL-terminated). Pure string math.
void append_dec(char* out, uint32_t cap, uint32_t n) {
    char tmp[12] = {};
    uint32_t t = 0u;
    if (n == 0u) {
        tmp[t++] = '0';
    } else {
        uint32_t v = n;
        while (v > 0u && t < 11u) { tmp[t++] = static_cast<char>('0' + (v % 10u)); v /= 10u; }
    }
    char num[12] = {};
    uint32_t o = 0u;
    while (t > 0u) num[o++] = tmp[--t];
    num[o] = '\0';
    ase::utils::str_append(out, cap, num);
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageRelmProjSyncSystem::on_start(ecs::Registry& registry) {
    (void)registry;
    log::debug("[StorageRelmProjSync] Started");
}

void StorageRelmProjSyncSystem::tick(ecs::Registry& registry, float /*dt*/) {
    // Project hub is Engine-global (owner 0), populated by the project hub pipeline. On the World/dist
    // tiers ENG_PROJ_COUNT is absent → this returns immediately (realms are an Engine-tier concern).
    if (!ase::hub::exists(registry, 0u, "ENG_PROJ_COUNT"_hs)) return;
    float count_f = ase::hub::get(registry, 0u, "ENG_PROJ_COUNT"_hs);
    if (ase::types::is_not_found(count_f)) return;
    uint32_t count = static_cast<uint32_t>(count_f);

    for (uint32_t i = 0u; i < count; ++i) {
        char idx_key[32] = {};
        ase::utils::str_copy(idx_key, sizeof(idx_key), "ENG_PROJ_");
        append_dec(idx_key, sizeof(idx_key), i);
        const char* proj_id = ase::hub::get_name(registry, entt::hashed_string(idx_key).value());
        if (proj_id == nullptr || proj_id[0] == '\0') continue;

        char owner_key[96] = {};
        ase::utils::str_copy(owner_key, sizeof(owner_key), proj_id);
        ase::utils::str_append(owner_key, sizeof(owner_key), "_owner");
        const char* owner = ase::hub::get_name(registry, entt::hashed_string(owner_key).value());
        if (owner == nullptr || owner[0] == '\0') continue;  // no owner → cannot form an owned realm

        uint32_t proj_hash = entt::hashed_string(proj_id).value();
        float tier_f = ase::hub::get(registry, proj_hash, "ENG_PROJ_TIER"_hs);
        uint8_t tier = ase::types::is_not_found(tier_f) ? TIER_INDIE : static_cast<uint8_t>(tier_f);

        // Idempotency: skip if a realm with this id already exists. The scan finishes (break) before
        // any create(), so no view invalidation.
        bool realm_exists = false;
        for (auto [re, rc] : registry.view<StorageStaRelmComponent>().each()) {
            (void)re;
            if (ase::utils::str_equal(rc.id, proj_id, MAX_REALM_ID)) { realm_exists = true; break; }
        }
        if (realm_exists) continue;

        // Create the project realm: owner = project-owner user id string. The A/ACS ladder's owner-preset
        // (realm.owner == req.user_id) grants the owner; a non-owner is concealed → realm_not_found (no
        // existence leak). Credentials live at PROTECTION_PROTECTED (API keys / service credentials).
        auto realm_ent = registry.create();
        auto& relm = registry.emplace<StorageStaRelmComponent>(realm_ent);
        ase::utils::str_copy(relm.id, MAX_REALM_ID, proj_id);
        ase::utils::str_copy(relm.name, MAX_REALM_NAME, proj_id);
        ase::utils::str_copy(relm.owner, MAX_OWNER_ID, owner);
        relm.default_protection = PROTECTION_PROTECTED;
        relm.tier = tier;
        registry.emplace<StorageRelmPersonalTag>(realm_ent);
        registry.emplace<StorageRelmActiveTag>(realm_ent);
        registry.emplace<StorageRelmConcealTag>(realm_ent);

        // Credential-subtree ACL rule so a realm member who passes concealment still needs clearance 5.
        auto rule_ent = registry.create();
        auto& rule = registry.emplace<StorageAcssRuleComponent>(rule_ent);
        rule.relm_ref = static_cast<uint32_t>(realm_ent);
        rule.proj_ref = 0u;
        ase::utils::str_copy(rule.path_pattern, MAX_PATH_LEN, "customers/");
        rule.protection_level = PROTECTION_PROTECTED;

        log::info("[StorageRelmProjSync] realm created for project '{}' owner='{}' (protected, concealed)",
                  proj_id, owner);
    }
}

void StorageRelmProjSyncSystem::on_stop(ecs::Registry& registry) {
    (void)registry;
    log::debug("[StorageRelmProjSync] Stopped");
}

}  // namespace ase::storage
