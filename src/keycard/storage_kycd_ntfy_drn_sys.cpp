/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_kycd_ntfy_drn_sys.cpp
 * @brief       StorageKycdNtfyDrnSystem - Bridges SDK Hub keycard-notify signals
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
 * @created     2026-04-16
 * @modified    2026-04-16
 * @version     1.0.0
 *
 * CAUSAL CHAIN (SDK Hub Bridge → Storage Request Component)
 *
 *   [plugins/ase-pl-webserver auth_routes POST /api/auth/keycard-issued]
 *          │
 *          │ sdk::verify_keycard_notify + sdk::emplace_keycard_issuance_request
 *          │ publishes SES_KYCD_NTF_{USER_ID,CLRN,REALM_ID,EXP_AT} Hub keys on a
 *          │ fresh request entity + hub::HubStgKycdPendTag
 *          ▼
 *   ┌─────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageKycdNtfyDrnSystem                      │
 *   │                                                             │
 *   │  READS (per request entity):                                │
 *   │    - hub::HubStgKycdPendTag (discovery tag)                 │
 *   │    - SES_KYCD_NTF_USER_ID   (float FNV-1a32 hash)           │
 *   │    - SES_KYCD_NTF_CLRN      (float clearance 0-9)           │
 *   │    - SES_KYCD_NTF_REALM_ID  (float FNV-1a32 hash)           │
 *   │    - SES_KYCD_NTF_EXP_AT    (float unix seconds)            │
 *   │                                                             │
 *   │  WRITES:                                                    │
 *   │    - StorageReqKycdComponent on the same request entity     │
 *   │      (leaves HubStgKycdPendTag intact for downstream)       │
 *   └─────────────────────────────────────────────────────────────┘
 *          │
 *          │ Entity now carries both ReqKycdComponent + HubStgKycdPendTag
 *          ▼
 *   StorageKycdReqDrnSystem picks it up, creates token entity,
 *   destroys request entity (tag + component released together).
 *
 * HUB Pattern (ARCH_ASE_HUB_API v2.0)
 *
 * READS (from Hub):
 *   SES_KYCD_NTF_USER_ID_HI, SES_KYCD_NTF_USER_ID_LO, SES_KYCD_NTF_CLRN,
 *   SES_KYCD_NTF_REALM_ID, SES_KYCD_NTF_EXP_AT
 *
 * WRITES (to Hub):
 *   (none — component emplace is module-local)
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
#include <ase/storage/systems/keycard/storage_kycd_ntfy_drn_sys.hpp>
// Components from same module (request-side)
#include <ase/storage/components/request/storage_req_kycd_comp.hpp>
#include <ase/storage/components/request/storage_req_kycd_relm_comp.hpp>
#include <ase/storage/components/request/storage_req_kycd_cwrd_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/types.hpp>
// Hub API (discovery tag + notify Hub keys)
#include <ase/hub/api.hpp>
// Types (L0 — is_not_found sentinel check)
#include <ase/types/types.hpp>
// String ops (L0 — codeword copy)
#include <ase/utils/strops.hpp>

#include <entt/core/hashed_string.hpp>
// Logging
#include <ase/log/log.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
namespace {

// No helper functions needed → all logic inline in tick()

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageKycdNtfyDrnSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdNtfyDrnSystem] Started");
}

void StorageKycdNtfyDrnSystem::tick(ecs::Registry& registry, float /*dt*/) {
    // Tag-filtered view: request entities that carry the Hub-owned pend tag
    // but do NOT yet have a StorageReqKycdComponent (this system emplaces
    // it on first sight, so a re-iteration next tick is a no-op).
    auto view = registry.view<hub::HubStgKycdPendTag>(
        entt::exclude<StorageReqKycdComponent>);
    for (auto req_entity : view) {
        uint32_t owner = static_cast<uint32_t>(req_entity);

        // The SDK carries the exact uint32 FNV user_hash as two 16-bit float
        // halves (a single float cast would truncate the gate owner via the
        // 24-bit mantissa). Both halves are <= 65535 → exactly representable, so
        // the reconstruction below is bit-exact and lands at the gate's owner.
        float user_hash_hi_f = hub::get(registry, owner, "SES_KYCD_NTF_USER_ID_HI"_hs, 0.0f);
        if (ase::types::is_not_found(user_hash_hi_f)) {
            log::error("[StorageKycdNtfyDrn] SES_KYCD_NTF_USER_ID_HI not set on req entity={}", owner);
            continue;
        }
        float user_hash_lo_f = hub::get(registry, owner, "SES_KYCD_NTF_USER_ID_LO"_hs, 0.0f);
        if (ase::types::is_not_found(user_hash_lo_f)) {
            log::error("[StorageKycdNtfyDrn] SES_KYCD_NTF_USER_ID_LO not set on req entity={}", owner);
            continue;
        }
        float exp_at_f = hub::get(registry, owner, "SES_KYCD_NTF_EXP_AT"_hs, 0.0f);
        if (ase::types::is_not_found(exp_at_f)) {
            log::error("[StorageKycdNtfyDrn] SES_KYCD_NTF_EXP_AT not set on req entity={}", owner);
            continue;
        }
        float clearance_f = hub::get(registry, owner, "SES_KYCD_NTF_CLRN"_hs, 0.0f);
        if (ase::types::is_not_found(clearance_f)) clearance_f = 0.0f;
        // realm_hash is an FNV uint32 carried as two exact 16-bit halves (the SDK
        // producer splits it); reconstruct bit-exact so the realm match at line ~246
        // succeeds and the realm+permission binding is applied (single-float truncated).
        float realm_hash_hi_f = hub::get(registry, owner, "SES_KYCD_NTF_REALM_ID_HI"_hs, 0.0f);
        if (ase::types::is_not_found(realm_hash_hi_f)) realm_hash_hi_f = 0.0f;
        float realm_hash_lo_f = hub::get(registry, owner, "SES_KYCD_NTF_REALM_ID_LO"_hs, 0.0f);
        if (ase::types::is_not_found(realm_hash_lo_f)) realm_hash_lo_f = 0.0f;

        uint32_t user_hash  = (static_cast<uint32_t>(user_hash_hi_f) << 16)
                              | static_cast<uint32_t>(user_hash_lo_f);
        uint32_t clearance  = static_cast<uint32_t>(clearance_f);
        uint32_t realm_hash = (static_cast<uint32_t>(realm_hash_hi_f) << 16)
                              | static_cast<uint32_t>(realm_hash_lo_f);
        uint64_t exp_at     = static_cast<uint64_t>(exp_at_f);

        const char* user_id_str = hub::get_name(registry, user_hash);
        if (user_id_str == nullptr) user_id_str = "";

        auto& payload = registry.emplace<StorageReqKycdComponent>(req_entity);
        size_t copy_len = 0;
        while (user_id_str[copy_len] != '\0' && copy_len < 63) {
            payload.user_id[copy_len] = user_id_str[copy_len];
            ++copy_len;
        }
        payload.user_id[copy_len] = '\0';
        payload.user_id_hash     = user_hash;
        payload.authenticated_at = exp_at > 30 ? (exp_at - 30) : 0;
        payload.expires_at       = exp_at;
        payload.clearance        = static_cast<uint8_t>(clearance);

        // Realm + permission binding (Phase 12 operator mint): resolve the realm
        // hash to its realm entity and carry it + permission bits as a sibling
        // request component. Absent for the legacy auth-gate flow (realm_hash 0).
        if (realm_hash != 0) {
            float perm_f = hub::get(registry, owner, "SES_KYCD_NTF_PERM"_hs, 0.0f);
            if (ase::types::is_not_found(perm_f)) perm_f = 0.0f;
            uint32_t realm_ref = INVALID_ENTITY;
            auto relm_view = registry.view<StorageStaRelmComponent>();
            for (auto [re, rc] : relm_view.each()) {
                if (entt::hashed_string(rc.id).value() == realm_hash) {
                    realm_ref = static_cast<uint32_t>(re);
                    break;
                }
            }
            if (realm_ref != INVALID_ENTITY) {
                auto& relm_req = registry.emplace<StorageReqKycdRelmComponent>(req_entity);
                relm_req.relm_ref = realm_ref;
                relm_req.perm = static_cast<uint16_t>(perm_f);
            }
        }

        // Codeword grants (Phase 12 operator mint): SES_KYCD_NTF_CWRD_COUNT +
        // per-index SES_KYCD_NTF_CWRD_<i> debug labels → one request-codeword
        // entity each (Entity-per-Item), consumed by StorageKycdReqDrnSystem.
        float cw_count_f = hub::get(registry, owner, "SES_KYCD_NTF_CWRD_COUNT"_hs, 0.0f);
        if (ase::types::is_not_found(cw_count_f)) cw_count_f = 0.0f;
        uint32_t cw_count = static_cast<uint32_t>(cw_count_f);
        for (uint32_t i = 0; i < cw_count; ++i) {
            char key[NTF_KEY_BUF_LEN] = "SES_KYCD_NTF_CWRD_";
            char num[NTF_NUM_BUF_LEN];
            uint32_t n = i;
            uint32_t d = 0;
            do { num[d++] = static_cast<char>('0' + (n % DECIMAL_RADIX)); n /= DECIMAL_RADIX; } while (n > 0);
            uint32_t k = NTF_CWRD_PREFIX_LEN;
            while (d > 0) { key[k++] = num[--d]; }
            key[k] = '\0';
            const char* cw = hub::get_name(registry, entt::hashed_string(key).value());
            if (cw == nullptr || cw[0] == '\0') continue;
            auto cw_ent = registry.create();
            auto& cwc = registry.emplace<StorageReqKycdCwrdComponent>(cw_ent);
            cwc.req_ref = static_cast<uint32_t>(req_entity);
            ase::utils::str_copy(cwc.cwrd, MAX_CODEWORD_LEN, cw);
        }

        log::debug("[StorageKycdNtfyDrn] +StorageReqKycdComponent entity={} user='{}' clearance={} exp={}",
                   owner, user_id_str, clearance, exp_at);
    }
}

void StorageKycdNtfyDrnSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdNtfyDrnSystem] Stopped");
}

}  // namespace ase::storage
