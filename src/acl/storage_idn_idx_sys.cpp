/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_idn_idx_sys.cpp
 * @brief       StorageIdnIdxSystem - indexes the identities the frame resolves by key
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
 * @created     2026-08-16
 * @modified    2026-08-16
 * @version     1.0.0
 *
 * CAUSAL CHAIN (Identity Index Build)
 *
 *   [Reception created realms; the Hub mirrors ready network clients]
 *          │
 *          │ Each identity publishes a key: a realm its id hash, a client its NET_CLAI_ID
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageIdnIdxSystem           │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageRelmIdnComponent (realms)       │
 *   │    - hub::HubNetClaiRdyTag (ready clients)  │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - StorageAcssIndexResourceManager (ctx)  │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Both reachable by key, in O(1)
 *          ▼
 *   StorageKycdNtfyDrnSystem and StorageKycdLnkSystem resolve without scanning
 *
 * WHY THIS SYSTEM EXISTS BESIDE StorageAcssIdxSystem
 *   The frame runs Reception, then Ingestion, then Integration. Realms are created in
 *   Reception and read from Ingestion onwards; keycards are minted DURING Ingestion and
 *   must reach the Integration ladder in the SAME frame. One rebuild cannot honestly
 *   serve both: an early one hides a freshly minted keycard for a tick - its owner's
 *   first access would be refused - and a late one leaves the Ingestion readers with
 *   nothing to read.
 *
 *   The relations are therefore split by the stage that can produce them, and each has
 *   exactly ONE writer. This system owns realms and clients; StorageAcssIdxSystem owns
 *   rules, links, required codewords, tasks and edges. Neither clears the other's rows.
 *
 * HUB Pattern (Active - reads NET_CLAI_ID)
 *
 * READS (from Hub):
 *   NET_CLAI_ID - the network client id a mirrored client entity publishes
 *
 * WRITES (to Hub):
 *   (none)
 *
 * FLYWEIGHT PATTERN (Active - StorageAcssIndexResourceManager via ctx)
 *   Registered by StorageAcssIdxSystem::on_start. Every on_start runs during startup,
 *   before any tick of any schedule, so the manager is present when this system first
 *   ticks no matter which schedule runs first.
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
#include <ase/storage/systems/acl/storage_idn_idx_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_relm_idn_comp.hpp>
#include <ase/storage/storage_acss_index_resource_manager.hpp>
#include <ase/storage/types.hpp>
// Hub (Layer 1)
#include <ase/hub/api.hpp>
// Types
#include <ase/types/types.hpp>
// Logging
#include <ase/log/log.hpp>

#include <entt/core/hashed_string.hpp>

using namespace entt::literals;

namespace ase::storage {

// HELPERS - PURE FUNCTIONS ONLY!
// NO STRUCTS HERE! Structs = Data = Components!
// NO View/Query operations in helpers! Only pure math!
namespace {

// A Hub value is an f32, so the client id survives the round trip only while it fits the
// 24-bit mantissa. Converting in ONE named place is what keeps that limit visible; a bare
// cast at the call site would hide the moment ids grow past it and clients silently start
// indexing under a neighbouring number.
uint32_t clai_id_from_hub(float published) {
    return static_cast<uint32_t>(published);
}

}  // namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageIdnIdxSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageIdnIdx] Started");
}

void StorageIdnIdxSystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    auto* idx_ptr = registry.ctx().find<StorageAcssIndexResourceManager*>();
    if (!idx_ptr || !(*idx_ptr)) {
        log::error("[StorageIdnIdx] StorageAcssIndexResourceManager not in ctx (StorageAcssIdxSystem registers it)");
        return;
    }
    auto& idx = **idx_ptr;

    // Only the rows this system owns are dropped. A shared clear is how two writers start
    // fighting over one row: it would erase the Integration builder's work while its own
    // readers still need it.
    idx.clear_ingestion();

    // Realms under their id hash. The hash is READ from the identity component, never
    // re-derived from the string, so bucket key and stored identity are one number.
    for (auto [relm_ent, relm_idn] : registry.view<StorageRelmIdnComponent>().each()) {
        idx.store_realm(static_cast<uint64_t>(relm_idn.id_hash),
                        static_cast<uint32_t>(relm_ent));
    }

    // Network clients under the id they publish. A mirrored client without the value is
    // not a fault - the Hub mirror can be ready before the id is written - so it is
    // skipped and picked up on the next frame.
    for (auto client_ent : registry.view<hub::HubNetClaiRdyTag>()) {
        const float net_id =
            hub::get(registry, static_cast<uint32_t>(client_ent), "NET_CLAI_ID"_hs);
        if (ase::types::is_not_found(net_id)) {
            continue;
        }
        idx.store_client(clai_id_from_hub(net_id), static_cast<uint32_t>(client_ent));
    }
}

void StorageIdnIdxSystem::on_stop(ecs::Registry& registry) {
    log::debug("[StorageIdnIdx] Stopped");

    auto* idx_ptr = registry.ctx().find<StorageAcssIndexResourceManager*>();
    if (idx_ptr && *idx_ptr) {
        (*idx_ptr)->clear_ingestion();
    }
}

}  // namespace ase::storage
