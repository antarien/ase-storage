/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_latc_sync_sys.cpp
 * @brief       StorageLatcSyncSystem - retires lattice links whose lifetime has elapsed
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Preservation
 * @usecase     usecase:101533 - withdraw a lapsed lattice bond
 * @actor       Horizontal Layer Steward (HLSTEWARD)
 * @created     2026-04-05
 * @modified    2026-10-29
 * @version     1.1.0
 *
 * CAUSAL CHAIN (Lattice Sync)
 *
 *   [Lattice link entities: StorageLatLnkComponent + StorageLnkIdnComponent]
 *          │
 *          │ Preservation, 1 Hz (ARCH_ASE_STORAGE 15)
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageLatcSyncSystem         │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageLatLnkComponent (expires_at)    │
 *   │    - StorageLnkIdnComponent (realm hashes)  │
 *   │    - StorageResourceManager (wall clock)    │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - StorageLnkExpTag on an ended link      │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ The ended link carries its state; the client sees it as data
 *          ▼
 *   The ACL ladder (StorageAcssChkSystem) refuses it by the same clock
 *
 * A BOND GRANTED FOR A TIME ENDS ON ITS OWN. ARCH_ASE_STORAGE 8.3 grants the game-jam link
 * for seven days; once the wall clock passes expires_at the bond is over. The link entity is
 * not destroyed: it stays, tagged, as the record of the realms it once bridged.
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
 *   The wall clock comes from the resource manager, the same clock the ACL check reads.
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
#include <ase/storage/systems/lattice/storage_latc_sync_sys.hpp>
// Components
#include <ase/storage/components/state/storage_lat_lnk_comp.hpp>
#include <ase/storage/components/state/storage_lnk_idn_comp.hpp>
#include <ase/storage/components/tag/storage_lnk_exp_tag.hpp>
#include <ase/storage/storage_resource_manager.hpp>
// Logging
#include <ase/log/log.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
namespace {

// No helper functions: the expiry test is one comparison against the component field

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageLatcSyncSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageLatcSync] Started");
}

void StorageLatcSyncSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto& mgr = *registry.ctx().get<StorageResourceManager*>();

    // No wall clock yet means no expiry can be judged; the ACL check waits for the same clock.
    uint64_t now = mgr.get_wall_time_seconds();
    if (!now) {
        return;
    }

    // SINGLE-PASS over the links that still stand. Only the tag the view excludes is added,
    // nothing is erased, so the iteration stays valid: the tagged link leaves the view behind us.
    auto view = registry.view<StorageLatLnkComponent, StorageLnkIdnComponent>(
        entt::exclude<StorageLnkExpTag>);
    for (auto entity : view) {
        auto& link = view.get<StorageLatLnkComponent>(entity);
        auto& idn = view.get<StorageLnkIdnComponent>(entity);

        // 0 = a permanent bond (ARCH_ASE_STORAGE 8.2)
        if (!link.expires_at) {
            continue;
        }
        if (now <= link.expires_at) {
            continue;
        }

        registry.emplace<StorageLnkExpTag>(entity);
        log::info("[StorageLatcSync] Lattice link ended: entity={} source={} target={} "
                  "prefix={} expires_at={} now={}",
                  static_cast<uint32_t>(entity), idn.source_realm_hash, idn.target_realm_hash,
                  link.path_prefix, link.expires_at, now);
    }
}

void StorageLatcSyncSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageLatcSync] Stopped");
}

}  // namespace ase::storage
