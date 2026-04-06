/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_file_writ_sys.cpp
 * @brief       StorageFileWritSystem - Filesystem CRUD operations
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @created     2026-04-05
 * @modified    2026-04-05
 * @version     1.0.0
 *
 * CAUSAL CHAIN (File Write)
 *
 *   [Access granted by StorageAcssChkSystem]
 *          │
 *          │ Integration schedule processes granted requests
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageFileWritSystem          │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageReqAcssComponent (file path)    │
 *   │    - StorageAcssGrantTag (granted requests) │
 *   │    - StorageStaRelmComponent (realm info)   │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - File data via ResourceManager I/O      │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ File written to realm-scoped path
 *          ▼
 *   HTTP response with write result
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
 *   Realm path resolution and filesystem I/O via ResourceManager.
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
#include <ase/storage/systems/fs/storage_file_writ_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_req_acss_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/tag/storage_tag_acss_grant.hpp>
#include <ase/storage/storage_resource_manager.hpp>
#include <ase/utils/strops.hpp>
// Logging
#include <ase/log/log.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
namespace {

// No helper functions needed → file I/O delegated to ResourceManager

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageFileWritSystem::on_start(ecs::Registry& /*registry*/) {
    log::info("[StorageFileWrit] Started");
}

void StorageFileWritSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto* mgr_ptr = registry.ctx().find<StorageResourceManager*>();
    if (!mgr_ptr || !(*mgr_ptr)) {
        return;
    }
    auto& mgr = **mgr_ptr;

    // SINGLE-PASS: process each granted access request
    auto grant_view = registry.view<StorageReqAcssComponent, StorageAcssGrantTag>();
    for (auto entity : grant_view) {
        auto& req = grant_view.get<StorageReqAcssComponent>(entity);

        // Resolve realm-scoped filesystem path via ResourceManager
        char resolved[512] = {};
        auto relm_ent = static_cast<entt::entity>(req.relm_ref);
        if (!registry.valid(relm_ent)) {
            log::error("[StorageFileWrit] Realm entity invalid for ref={}", req.relm_ref);
            continue;
        }
        auto* relm = registry.try_get<StorageStaRelmComponent>(relm_ent);
        if (!relm) {
            log::error("[StorageFileWrit] Realm component missing for ref={}", req.relm_ref);
            continue;
        }
        mgr.resolve_path(relm->id, nullptr, req.path, resolved, 512);

        // Write file via ResourceManager filesystem bridge
        if (!mgr.write_file(resolved, req.path, ase::utils::str_len(req.path, 256))) {
            log::error("[StorageFileWrit] Write failed: {}", resolved);
        }
    }
}

void StorageFileWritSystem::on_stop(ecs::Registry& /*registry*/) {
    log::info("[StorageFileWrit] Stopped");
}

}  // namespace ase::storage
