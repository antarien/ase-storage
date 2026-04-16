/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_kycd_sync_snd_sys.cpp
 * @brief       StorageKycdSyncSndSystem - Engine→Replica keycard sync sender
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Transmission
 * @created     2026-04-16
 * @modified    2026-04-16
 * @version     1.0.0
 *
 * CAUSAL CHAIN (Engine-side sync of validated keycard to Replica)
 *
 *   [StorageKycdLnkSystem links keycard to client, adds StorageKycdVldTag]
 *          │
 *          │ View<StorageStaIdnComponent, StorageStaKycdComponent,
 *          │       StorageKycdVldTag> without StorageKycdSntTag
 *          ▼
 *   ┌─────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageKycdSyncSndSystem          │
 *   │                                                 │
 *   │  READS:                                         │
 *   │    - StorageStaIdnComponent (user_id, client_id,│
 *   │      authenticated_at)                          │
 *   │    - StorageStaKycdComponent (clrn, expires_at, │
 *   │      relm_ref)                                  │
 *   │    - StorageStaRelmComponent (id buffer)        │
 *   │    - StorageKycdVldTag (validate filter)        │
 *   │    - StorageKycdSntTag (idempotence +           │
 *   │      invalidate filter)                         │
 *   │                                                 │
 *   │  WRITES (to Replica via ase-websocket):         │
 *   │    - BIN_MSG_SESSION_KYCD_SYNC (validate/       │
 *   │      invalidate, 160-byte frame)                │
 *   │  WRITES (ECS):                                  │
 *   │    - StorageKycdSntTag (add/erase per path)     │
 *   └─────────────────────────────────────────────────┘
 *          │
 *          │ Binary WebSocket frame → Replica
 *          ▼
 *   SessionKycdDrnSystem (Phase 3, ase-session) consumes the frame,
 *   builds SessionStaIdnComponent + SessionStaAthzComponent +
 *   SessionIdnVldTag on the Replica session entity.
 *
 * HUB Pattern (Active)
 *
 * READS (from Hub):
 *   (none)
 *
 * WRITES (to Hub):
 *   (none — transport is Binary-WS, not Hub. Session hub publication
 *    happens on Replica side via SessionIdnHubWritSystem.)
 *
 * STAR Topology (Active)
 *   ase-storage stays a leaf publisher: it reads its own Components and
 *   dispatches to ase-websocket (L1 Core) via registry.ctx() lookup. No
 *   L3→L3 dependency is created — ase-storage does not link ase-replication
 *   even though the frame id is registered there.
 *
 * FLYWEIGHT Pattern (Active)
 *   websocket::WebSocketResourceManager is a ctx()-held flyweight. The
 *   send() method is thread-safe (libdatachannel queue). No per-frame
 *   allocation in the hot path — the caller owns the stack buffer.
 *
 * Deferred-deletion note: Tag mutations during iteration use
 * registry.emplace<>()/erase<>() on the *current* entity only. Both views
 * are filtered (include/exclude) so exactly the entities we touch are the
 * ones whose tag we update; this follows the same pattern used by
 * StorageKycdLnkSystem (erase<StorageKycdVldTag> + emplace<StorageKycdRjctTag>).
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
#include <ase/storage/systems/sync/storage_kycd_sync_snd_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_sta_idn_comp.hpp>
#include <ase/storage/components/state/storage_sta_kycd_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/tag/storage_tag_kycd_vld.hpp>
#include <ase/storage/components/tag/storage_tag_kycd_snt.hpp>
// WebSocket client + wire constants (L1 Core)
#include <ase/websocket/websocket_resource_manager.hpp>
#include <ase/websocket/replica_wire_types.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>
#include <cstring>
#include <entt/core/hashed_string.hpp>

using namespace entt::literals;

namespace ase::storage {

namespace {

/**
 * Zero-fill a fixed 160-byte frame buffer and stamp the header bytes.
 * No View/Query operations — pure byte math.
 */
void init_frame(char* buf, uint8_t status, uint8_t clearance) {
    for (uint32_t i = 0; i < websocket::REPLICA_SES_KYCD_TOT_SZ; ++i) {
        buf[i] = 0;
    }
    buf[0] = static_cast<char>(websocket::BIN_MSG_SESSION_KYCD_SYNC);
    buf[1] = static_cast<char>(status);
    buf[2] = static_cast<char>(clearance);
    buf[3] = 0;
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageKycdSyncSndSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdSyncSndSystem] Started");
}

void StorageKycdSyncSndSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto* ws = registry.ctx().find<websocket::WebSocketResourceManager>();
    if (ws == nullptr || !ws->is_connected()) {
        // Engine has no live Replica connection — skip silently, the next
        // tick will retry. Sessions already sent stay tagged; on reconnect
        // Engine will decide via invalidate/validate diffs.
        return;
    }

    // Validate path: freshly linked sessions that Replica has not yet mirrored.
    auto val_view = registry.view<StorageStaIdnComponent,
                                   StorageStaKycdComponent,
                                   StorageKycdVldTag>(entt::exclude<StorageKycdSntTag>);
    for (auto [entity, idn, kycd] : val_view.each()) {
        char buf[websocket::REPLICA_SES_KYCD_TOT_SZ];
        init_frame(buf, websocket::REPLICA_SES_KYCD_STATUS_VALIDATE, kycd.clrn);

        uint32_t client_id     = idn.client_id;
        uint32_t user_id_hash  = entt::hashed_string{idn.user_id}.value();
        uint32_t realm_id_hash = 0;
        char     realm_name[64] = {};

        auto relm_entity = static_cast<entt::entity>(kycd.relm_ref);
        auto* relm = registry.try_get<StorageStaRelmComponent>(relm_entity);
        if (relm != nullptr) {
            realm_id_hash = entt::hashed_string{relm->id}.value();
            for (uint32_t i = 0; i < 64; ++i) realm_name[i] = relm->id[i];
        } else if (kycd.relm_ref != 0) {
            realm_id_hash = kycd.relm_ref;
        }

        std::memcpy(buf + websocket::REPLICA_SES_KYCD_CLNT_OFF, &client_id,        4);
        std::memcpy(buf + websocket::REPLICA_SES_KYCD_UHSH_OFF, &user_id_hash,     4);
        std::memcpy(buf + websocket::REPLICA_SES_KYCD_RHSH_OFF, &realm_id_hash,    4);
        std::memcpy(buf + websocket::REPLICA_SES_KYCD_AUT_OFF,  &idn.authenticated_at, 8);
        std::memcpy(buf + websocket::REPLICA_SES_KYCD_EXP_OFF,  &kycd.expires_at,  8);
        std::memcpy(buf + websocket::REPLICA_SES_KYCD_USER_OFF, idn.user_id,       64);
        std::memcpy(buf + websocket::REPLICA_SES_KYCD_RELM_OFF, realm_name,        64);

        if (ws->send(buf, websocket::REPLICA_SES_KYCD_TOT_SZ)) {
            registry.emplace<StorageKycdSntTag>(entity);
            log::debug("[StorageKycdSyncSndSystem] validate sent client_id={} user='{}' clearance={} realm='{}'",
                       client_id, idn.user_id, kycd.clrn, realm_name);
        } else {
            log::warn("[StorageKycdSyncSndSystem] validate send failed client_id={}", client_id);
        }
    }

    // Invalidate path: sessions previously synchronised but that have since
    // lost their StorageKycdVldTag (expiry, revocation, logout).
    auto inv_view = registry.view<StorageStaIdnComponent,
                                   StorageKycdSntTag>(entt::exclude<StorageKycdVldTag>);
    for (auto [entity, idn] : inv_view.each()) {
        char buf[websocket::REPLICA_SES_KYCD_TOT_SZ];
        init_frame(buf, websocket::REPLICA_SES_KYCD_STATUS_INVALIDATE, 0);

        uint32_t client_id = idn.client_id;
        std::memcpy(buf + websocket::REPLICA_SES_KYCD_CLNT_OFF, &client_id, 4);

        if (ws->send(buf, websocket::REPLICA_SES_KYCD_TOT_SZ)) {
            registry.erase<StorageKycdSntTag>(entity);
            log::debug("[StorageKycdSyncSndSystem] invalidate sent client_id={} user='{}'",
                       client_id, idn.user_id);
        } else {
            log::warn("[StorageKycdSyncSndSystem] invalidate send failed client_id={}", client_id);
        }
    }
}

void StorageKycdSyncSndSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdSyncSndSystem] Stopped");
}

}  // namespace ase::storage
