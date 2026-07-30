/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_kycd_sess_cln_sys.cpp
 * @brief       StorageKycdSessClnSystem - Retires the SES_* family of a dead session
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    action/maintenance/cleanup
 * @schedule    Preservation
 * @created     2026-07-28
 * @modified    2026-07-28
 * @version     1.0.0
 *
 * CAUSAL CHAIN (Session Hub-Family Retirement)
 *
 *   [NetworkHubSyncSystem PASS 2: client disconnected]
 *          │
 *          │ removes HubStaClaiIdntComponent, KEEPS HubSessLifeTag,
 *          │ removes only NET_CLAI_ID / SES_IS_AUTHENTICATED / SES_EXP_AT
 *          ▼
 *   ┌─────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageKycdSessClnSystem                      │
 *   │                                                             │
 *   │  View: StorageKycdVldTag + HubSessLifeTag                   │
 *   │        excluding HubStaClaiIdntComponent                    │
 *   │    → remove the SES_* family THIS module published          │
 *   │    → remove StorageKycdVldTag (dead session is not valid)   │
 *   └─────────────────────────────────────────────────────────────┘
 *          │
 *          │ owner holds no hub values any more
 *          ▼
 *   HubSessLifeClnSystem can finally drop HubSessLifeTag, which its own
 *   guard blocks for as long as ANY value still names the owner.
 *
 * WHY THIS SYSTEM EXISTS
 *
 *   StorageKycdLnkSystem writes six keys on the client entity; the network
 *   module retires three. Clearance, realm and the user hash therefore
 *   survived every disconnect, with three consequences measured on
 *   2026-07-28:
 *
 *   1. HubSessLifeClnSystem never releases HubSessLifeTag, because it waits
 *      for the owner's value count to reach zero. Dead sessions accumulate
 *      tags and values for the whole uptime of the process.
 *   2. The connection snapshot is owner-blind, so the clearance and realm of
 *      every session that ever authenticated ship to every newly connecting
 *      client (PLAN_ASE_HUB_OWNR_SCOPE, IST-Zustand).
 *   3. HubNetBctReqSystem recognises a disconnect-race owner through a
 *      fixed-size list capped at NET_BCT_MAX_VALUES_PER_MSG. Once the corpses
 *      outgrow that cap, later dead owners miss the drop branch and fall into
 *      the public catch-all.
 *
 *   Same defect class as remove_keycard_ntf_family (storage_kycd_req_drn_sys.cpp)
 *   and ReplicationTlmtPeerRmvSystem's RPL_PEER_* removal: hub values live on
 *   their own hub-internal entities, so destroying or forgetting the owner
 *   entity does NOT clear them. The writer retires its own family.
 *
 * HUB Pattern (Active)
 *
 * READS (from Hub):
 *   (none — the view alone decides which owner is dead; no value is read back,
 *    which is why no NOT_FOUND check appears in tick())
 *
 * WRITES (to Hub, owner = dead client entity):
 *   removes SES_IS_AUTHENTICATED, SES_CLEARANCE, SES_EXP_AT, SES_REALM_ID,
 *   SES_USER_ID_HI, SES_USER_ID_LO
 *
 * STAR Topology (Active)
 *   Leaf retirer in the Hub star. The only outbound L3 edge is ase::hub; the
 *   dead-session verdict is read from hub-owned components (HubSessLifeTag,
 *   HubStaClaiIdntComponent), never from ase-network directly. StorageKycdLnkSystem
 *   is the paired publisher and is reached solely through hub state — no
 *   system-to-system call exists in either direction.
 *
 * FLYWEIGHT Pattern (Active)
 *   hub::remove delegates to HubResourceManager, which drops the
 *   (owner, value_id) entry from O(1) libcuckoo storage and destroys the
 *   hub-internal value entity. This system holds no handle and no copy of the
 *   values; the owner id is the only thing it carries, as a plain uint32_t.
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
 * [ ] NO switch/case statements? (use Tag-filtered Views!)
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
#include <ase/storage/systems/keycard/storage_kycd_sess_cln_sys.hpp>
// Components from same module
#include <ase/storage/components/tag/storage_tag_kycd_vld.hpp>
// Hub API (owns HubSessLifeTag + HubStaClaiIdntComponent + remove)
#include <ase/hub/api.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>
#include <entt/core/hashed_string.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
namespace {

/** Retire the SES_* family StorageKycdLnkSystem publishes for one session owner.
 *  Every key written there appears here — the two lists are a pair, and a key added
 *  on one side without the other strands in the hub for the life of the process.
 *  hub::remove on an absent key is a no-op returning false: a session that never
 *  reached a realm simply has nothing to retire, which is not an error condition. */
void remove_session_family(ecs::Registry& registry, uint32_t owner) {
    hub::remove(registry, owner, "SES_IS_AUTHENTICATED"_hs);
    hub::remove(registry, owner, "SES_CLEARANCE"_hs);
    hub::remove(registry, owner, "SES_EXP_AT"_hs);
    hub::remove(registry, owner, "SES_REALM_ID"_hs);
    hub::remove(registry, owner, "SES_USER_ID_HI"_hs);
    hub::remove(registry, owner, "SES_USER_ID_LO"_hs);
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageKycdSessClnSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdSessClnSystem] Started");
}

void StorageKycdSessClnSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * Single-pass over sessions that carried a linked keycard and have lost
     * their identity mirror. HubSessLifeTag is what distinguishes a dead
     * SESSION entity from an ordinary entity that merely lacks the mirror, so
     * both tags are required and the mirror is the exclude filter — exactly
     * the triple HubSessLifeClnSystem and HubNetBctReqSystem already use to
     * classify a disconnect.
     *
     * EnTT guarantees removing a component from the CURRENT entity during view
     * iteration is safe (WRFL_ASE_DEFERRED_DELETION — EnTT Issue #772). The
     * hub values destroyed by remove_session_family live on separate
     * hub-internal entities and carry none of the components in this view, so
     * they cannot invalidate the iteration.
     *
     * Dropping StorageKycdVldTag both terminates this loop for the entity and
     * states the truth: a disconnected session holds no valid keycard. No
     * ordering against HubSessLifeClnSystem is needed — if that system already
     * ran this tick, it releases HubSessLifeTag on the next one.
     */
    auto dead_view = registry.view<StorageKycdVldTag, hub::HubSessLifeTag>(
        entt::exclude<hub::HubStaClaiIdntComponent>);
    for (auto entity : dead_view) {
        uint32_t owner = static_cast<uint32_t>(entity);

        remove_session_family(registry, owner);
        registry.remove<StorageKycdVldTag>(entity);

        log::debug("[StorageKycdSessClnSystem] Retired SES_* family for dead session owner={}",
                   owner);
    }
}

void StorageKycdSessClnSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdSessClnSystem] Stopped");
}

}  // namespace ase::storage
