/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_kycd_req_drn_sys.cpp
 * @brief       StorageKycdReqDrnSystem - Drains HTTP-posted keycard issuance requests
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
 * @created     2026-04-16
 * @modified    2026-04-16
 * @version     1.0.0
 *
 * CAUSAL CHAIN (Keycard Issuance Drain)
 *
 *   [plugins/ase-pl-webserver auth_routes POST /api/auth/keycard-issued]
 *          │
 *          │ After HMAC-verify: emplace<StorageReqKycdComponent> +
 *          │ emplace<hub::HubStgKycdPendTag> on fresh request entity
 *          ▼
 *   ┌─────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageKycdReqDrnSystem                       │
 *   │                                                             │
 *   │  READS:                                                     │
 *   │    - StorageReqKycdComponent + hub::HubStgKycdPendTag       │
 *   │                                                             │
 *   │  WRITES:                                                    │
 *   │    - StorageStaIdnComponent (token entity)                  │
 *   │    - StorageStaKycdComponent (token entity)                 │
 *   │    - StorageKycdPendTag (token entity)                      │
 *   │    - STG_KYCD_ISSUED_COUNT (hub counter)                    │
 *   │  DESTROYS:                                                  │
 *   │    - request entity (collect-then-destroy)                  │
 *   └─────────────────────────────────────────────────────────────┘
 *          │
 *          │ Token entity enters existing keycard pipeline
 *          ▼
 *   StorageKycdVldSystem validates → StorageKycdLnkSystem links to client
 *
 * HUB Pattern (ARCH_ASE_HUB_API v2.0)
 *
 * READS (from Hub):
 *   STG_KYCD_ISSUED_COUNT - counter increment read-modify-write
 *
 * WRITES (to Hub):
 *   STG_KYCD_ISSUED_COUNT - counter increment per drained request
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
#include <ase/storage/systems/keycard/storage_kycd_req_drn_sys.hpp>
// Components from same module
#include <ase/storage/components/request/storage_req_kycd_comp.hpp>
#include <ase/storage/components/request/storage_req_kycd_relm_comp.hpp>
#include <ase/storage/components/request/storage_req_kycd_cwrd_comp.hpp>
#include <ase/storage/components/state/storage_sta_idn_comp.hpp>
#include <ase/storage/components/state/storage_sta_kycd_comp.hpp>
#include <ase/storage/components/state/storage_kycd_idn_comp.hpp>
#include <ase/storage/components/request/storage_req_kycd_tkn_comp.hpp>
#include <ase/storage/components/state/storage_kycd_cwrd_comp.hpp>
#include <ase/storage/components/tag/storage_tag_kycd_pend.hpp>
#include <ase/storage/components/tag/storage_tag_kycd_pst_pend.hpp>
// Hub API (discovery tag + counter)
#include <ase/hub/api.hpp>
// Types (L0 — is_not_found sentinel check)
#include <ase/types/types.hpp>
// Utils (L0 — bounded string copy for codeword grants)
#include <ase/utils/strops.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstring>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
namespace {

/** Remove the full SES_KYCD_NTF_* family a keycard mint/issuance request published for one owner.
 *  Mirrors remove_peer_family (replication_tlmt_peer_rmv_sys.cpp): these owner-scoped Hub values are
 *  transient SDK to drain IPC (set in sdk_keycard_notify.cpp, read in storage_kycd_ntfy_drn_sys.cpp).
 *  Each value lives on a separate hub-internal entity, so destroying the request entity does NOT
 *  clear it. Without this the 7 fixed keys plus the GRANT flags orphan and pollute every hub_values
 *  snapshot (immortal ghosts, the same defect class the peer family already fixed via hub::remove). */
void remove_keycard_ntf_family(ecs::Registry& registry, uint32_t owner) {
    hub::remove(registry, owner, "SES_KYCD_NTF_USER_ID_HI"_hs);
    hub::remove(registry, owner, "SES_KYCD_NTF_USER_ID_LO"_hs);
    hub::remove(registry, owner, "SES_KYCD_NTF_CLRN"_hs);
    hub::remove(registry, owner, "SES_KYCD_NTF_PERM"_hs);
    hub::remove(registry, owner, "SES_KYCD_NTF_REALM_ID_HI"_hs);
    hub::remove(registry, owner, "SES_KYCD_NTF_REALM_ID_LO"_hs);
    hub::remove(registry, owner, "SES_KYCD_NTF_EXP_AT"_hs);
    hub::remove(registry, owner, "SES_KYCD_NTF_GRANT_BINARY"_hs);
    hub::remove(registry, owner, "SES_KYCD_NTF_GRANT_SIG"_hs);
    hub::remove(registry, owner, "SES_KYCD_NTF_GRANT_SBOM"_hs);
    hub::remove(registry, owner, "SES_KYCD_NTF_GRANT_METADATA"_hs);
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageKycdReqDrnSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdReqDrnSystem] Started");
}

void StorageKycdReqDrnSystem::tick(ecs::Registry& registry, float /*dt*/) {
    // Collect-then-destroy: iterate view, remember request entities, destroy after.
    auto view = registry.view<StorageReqKycdComponent, hub::HubStgKycdPendTag>();
    for (auto req_entity : view) {
        auto& req = view.get<StorageReqKycdComponent>(req_entity);

        auto token_entity = registry.create();

        auto& idn = registry.emplace<StorageStaIdnComponent>(token_entity);
        std::memcpy(idn.user_id, req.user_id, 64);
        // Carry the EXACT FNV user_hash so the codeword/clearance projection owner
        // is derived from the hash, never from the (possibly-empty/dangling) string.
        idn.user_id_hash = req.user_id_hash;
        idn.authenticated_at = req.authenticated_at;

        auto& kycd = registry.emplace<StorageStaKycdComponent>(token_entity);
        kycd.clrn = req.clearance;
        kycd.expires_at = req.expires_at;
        // Bind the keycard recipient. The publisher matches keycard→session on the
        // HASH of the recipient, never on the characters: identity is a lookup
        // (WRFL_ASE_STRING_HANDLING Section 3). The string stays for audit records and
        // log lines; it is written here together with the hash so the two cannot drift.
        ase::utils::str_copy(kycd.issued_to, sizeof(kycd.issued_to), req.user_id);
        auto& kycd_idn = registry.emplace<StorageKycdIdnComponent>(token_entity);
        kycd_idn.issued_to_hash = entt::hashed_string(req.user_id).value();

        // Optional realm/permission extension (operator edge-keycard mint).
        // Absent on the legacy auth-gate flow → relm_ref/perm stay 0 (unchanged).
        const auto* relm_ext = registry.try_get<StorageReqKycdRelmComponent>(req_entity);
        if (relm_ext != nullptr) {
            kycd.relm_ref = relm_ext->relm_ref;
            kycd.perm = relm_ext->perm;
        }

        registry.emplace<StorageKycdPendTag>(token_entity);
        // Durable persist: this minted keycard lives only in RAM until the
        // Preservation-stage StorageKycdPstEmitSystem ships it owner-keyed over
        // the Hub WS lane to the Replica MongoDB authority. The dist host NEVER
        // links a data client — durability rides the SAME Hub path the edge
        // audit trail uses (ARCH_ASE_STORAGE.md line 91; Phase 12 H-3). The tag
        // is set AFTER the codeword grants below so the emit replicates them too.
        registry.emplace<StorageKycdPstPendTag>(token_entity);

        // The minted keycard is recorded ON the request. The codeword children name
        // their request in req_ref, so the SECOND pass below walks them once and reaches
        // this keycard by entity reference. The former version did the opposite - it
        // knew the parent and searched for the children, walking EVERY requested
        // codeword for EVERY request (WS-K.2c).
        auto& tkn = registry.emplace<StorageReqKycdTknComponent>(req_entity);
        tkn.kycd_ref = static_cast<uint32_t>(token_entity);

        log::debug("[StorageKycdReqDrn] +StorageKycdPendTag token={} user='{}' clearance={} relm={} perm={} exp={}",
                   static_cast<uint32_t>(token_entity), req.user_id,
                   static_cast<uint32_t>(req.clearance),
                   kycd.relm_ref, static_cast<uint32_t>(kycd.perm), req.expires_at);

        float issued_count = hub::get(registry, hub::GLOBAL, "STG_KYCD_ISSUED_COUNT"_hs, 0.0f);
        if (ase::types::is_not_found(issued_count)) issued_count = 0.0f;
        hub::set(registry, hub::GLOBAL, "STG_KYCD_ISSUED_COUNT"_hs, issued_count + 1.0f);

    }

    // PASS 2 - the codeword grants, walked ONCE over the children instead of once per
    // request. Each requested codeword names its request in req_ref, and the request now
    // carries the keycard it minted, so the parent is one try_get away. The entity is
    // re-purposed in place. No matching entities on the legacy auth-gate flow → none.
    for (auto [cwrd_req_entity, cwrd_req] :
         registry.view<StorageReqKycdCwrdComponent>().each()) {
        auto* tkn = registry.try_get<StorageReqKycdTknComponent>(
            static_cast<ecs::Entity>(cwrd_req.req_ref));
        if (tkn == nullptr) {
            // The parent has not minted (yet, or at all): the grant stays pending and is
            // picked up on the frame its request drains. Not a fault.
            continue;
        }
        char cwrd_copy[32] = {};
        ase::utils::str_copy(cwrd_copy, sizeof(cwrd_copy), cwrd_req.cwrd);
        // ADDING a type this view does not filter on leaves the iteration set untouched.
        // REMOVING StorageReqKycdCwrdComponent here would dissolve the very range being
        // walked, so the request rows are retired in one batch below instead.
        auto& cwrd = registry.emplace_or_replace<StorageKycdCwrdComponent>(cwrd_req_entity);
        cwrd.kycd_ref = tkn->kycd_ref;
        ase::utils::str_copy(cwrd.cwrd, sizeof(cwrd.cwrd), cwrd_copy);
        cwrd.cwrd_hash = entt::hashed_string(cwrd_copy).value();
        log::debug("[StorageKycdReqDrn] +StorageKycdCwrdComponent kycd={} cwrd='{}'",
                   cwrd.kycd_ref, cwrd.cwrd);
    }

    // Retire the request rows of everything that now holds a grant. The range walks the
    // GRANT type while the call removes the REQUEST type - two different types, so the
    // range cannot collapse under the call that walks it. A codeword whose parent has not
    // minted keeps its request row and stays untouched.
    auto granted_view = registry.view<StorageKycdCwrdComponent>();
    registry.remove<StorageReqKycdCwrdComponent>(granted_view.begin(), granted_view.end());

    // PASS 3 - release the drained requests. Deferred to here because pass 2 reaches the
    // keycard THROUGH the request: destroying it in pass 1 would cut the only link its
    // codeword children have to the keycard they belong to.
    ecs::Entity minted[KYCD_REQ_BATCH];
    uint32_t minted_n = 0;
    for (auto [req_entity, tkn] : registry.view<StorageReqKycdTknComponent>().each()) {
        (void)tkn;
        if (minted_n >= KYCD_REQ_BATCH) break;
        minted[minted_n] = req_entity;
        ++minted_n;
    }
    for (uint32_t i = 0; i < minted_n; ++i) {
        // Clear the transient SES_KYCD_NTF_* IPC values BEFORE releasing the request
        // entity: they sit on separate hub-internal entities and would otherwise orphan
        // into every hub_values snapshot (the preloader-flood the user reported).
        remove_keycard_ntf_family(registry, static_cast<uint32_t>(minted[i]));
        registry.destroy(minted[i]);
    }
}

void StorageKycdReqDrnSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdReqDrnSystem] Stopped");
}

}  // namespace ase::storage
