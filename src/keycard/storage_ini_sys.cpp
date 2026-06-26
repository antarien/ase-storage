/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_ini_sys.cpp
 * @brief       StorageIniSystem - Creates manager entity, loads JWT secret, seeds realms
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Initialization
 * @created     2026-04-05
 * @modified    2026-06-24
 * @version     1.0.0
 *
 * CAUSAL CHAIN (Storage Initialization)
 *
 *   [Server startup]
 *          │
 *          │ App runs Initialization schedule
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageIniSystem              │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - AUTH_JWT_SECRET from environment        │
 *   │    - ASE_DATA_DIR from environment           │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - StorageMgrTag entity (creates)         │
 *   │    - StorageResourceManager in ctx()        │
 *   │    - StorageStaRelmComponent (4 realms)     │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Manager entity + ResourceManager + Realms ready
 *          ▼
 *   StorageKycdDrnSystem drains tokens from ResourceManager
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
 *   JWT secret stored in StorageResourceManager.
 *   Registered in ctx() for all storage systems to access.
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
#include <ase/storage/systems/keycard/storage_ini_sys.hpp>
// Components from same module
#include <ase/storage/components/tag/storage_tag_mgr.hpp>
#include <ase/storage/storage_resource_manager.hpp>
#include <ase/storage/types.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdlib>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
// IMPORTANT: Use anonymous namespace, NOT static keyword!
//   namespace { void helper() {...} }   // CORRECT
//   static void helper() {...}          // WRONG!
// NO STRUCTS HERE! Structs = Data = Components!
// NO View/Query operations in helpers! Only pure math!
namespace {

// No helper functions needed → all logic inline in on_start()

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageIniSystem::on_start(ecs::Registry& registry) {
    log::debug("[StorageIni] Started");

    // Create manager entity with tag
    auto entity = registry.create();
    registry.emplace<StorageMgrTag>(entity);

    // Create and register StorageResourceManager in ctx()
    auto& mgr = registry.ctx().emplace<StorageResourceManager>();
    registry.ctx().emplace<StorageResourceManager*>(&mgr);

    // Load JWT secret from environment
    const char* secret = std::getenv("AUTH_JWT_SECRET");
    if (secret) {
        uint16_t len = 0;
        const char* p = secret;
        while (*p && len < 255) { ++p; ++len; }
        mgr.set_jwt_secret(secret, len);
        log::info("[StorageIni] JWT secret loaded ({} bytes)", len);
    } else {
        log::warn("[StorageIni] AUTH_JWT_SECRET not set");
    }

    // Set data directory: ASE_DATA_DIR env
    // Server (Hetzner):  ASE_DATA_DIR=/opt/ase/data
    // Local (dev):       ASE_DATA_DIR=clients/ase-client-web/public
    const char* data_dir = std::getenv("ASE_DATA_DIR");
    if (data_dir) {
        mgr.set_data_dir(data_dir);
    } else {
        mgr.set_data_dir("clients/ase-client-web/public");
    }
    log::info("[StorageIni] Data dir: {}", mgr.get_data_dir());

    // Ensure base directories exist
    char dir[512];
    mgr.resolve_path("", nullptr, "", dir, 512);
    mgr.ensure_dir(dir);

    // ASE shared realm (engine defaults) — always exists, visible to ALL realms
    // Contains: shaders, fonts, templates, starter-kits, default audio
    // No Lattice needed — every project can read from ase/ implicitly
    mgr.resolve_path("ase", nullptr, "", dir, 512);
    mgr.ensure_dir(dir);
    mgr.resolve_path("ase", nullptr, "shaders", dir, 512);
    mgr.ensure_dir(dir);
    mgr.resolve_path("ase", nullptr, "fonts", dir, 512);
    mgr.ensure_dir(dir);
    mgr.resolve_path("ase", nullptr, "templates", dir, 512);
    mgr.ensure_dir(dir);
    mgr.resolve_path("ase", nullptr, "audio", dir, 512);
    mgr.ensure_dir(dir);
    log::info("[StorageIni] ASE shared realm directory ready");

    // Project realms are created dynamically by EngineLodProjSystem
    // from manifest.storage.realm_id in MongoDB. No hardcoded realms.

    log::info("[StorageIni] Storage module ready");
}

void StorageIniSystem::tick(ecs::Registry& /*registry*/, float /*dt*/) {
    // Initialization system has no per-tick logic
}

void StorageIniSystem::on_stop(ecs::Registry& registry) {
    log::debug("[StorageIni] Stopped");
    auto* mgr = registry.ctx().find<StorageResourceManager*>();
    if (mgr && *mgr) {
        (*mgr)->clear_all();
    }
}

}  // namespace ase::storage
