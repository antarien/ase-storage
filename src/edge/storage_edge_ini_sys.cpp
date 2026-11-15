/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_edge_ini_sys.cpp
 * @brief       StorageEdgeIniSystem - Seeds the edge_binaries distribution realm
 * @description Lays the edge_binaries realm the one-line installer downloads the edge daemon
 *              from: its directory tree, its realm entity and its identity tag.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Initialization
 * @design      DSGN_021
 * @usecase     usecase:100292 - install edge daemon
 * @actor       Edge Release Conductor (EDGERELCON)
 * @created     2026-06-24
 * @modified    2026-11-05
 * @version     00.00.39.00039 [core]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * CAUSAL CHAIN (Edge-Distribution Realm Seeding)
 *
 *   [Server startup, after StorageIniSystem]
 *          │
 *          │ App runs Initialization schedule
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageEdgeIniSystem          │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageResourceManager* from ctx()     │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - edge_binaries realm directory tree     │
 *   │    - StorageStaRelmComponent (edge realm)   │
 *   │    - StorageRelmIdnComponent (id hash)      │
 *   │    - StorageRelmQuotComponent (quota)       │
 *   │    - StorageRelmGlobTag + RelmActiveTag     │
 *   │    - StorageRelmEdgeTag (realm identity)    │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Realm entity + its identity tag ready, tier zero-initialised
 *          ▼
 *   StorageEdgeTierSystem writes the tier from the hub in Integration
 *   StorageAcssEdgeIniSystem scopes the edge ACL rules to it
 *   StorageWflwTranSystem, WflwGateSystem, WflwClnSystem read the tag as identity
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
 *   Realm path resolution + directory creation via the manager.
 *
 * DEDICATION
 *   The REALM and nothing else: its directory tree, its entity, its identity tag.
 *   The ACL rules of the realm are StorageAcssEdgeIniSystem (src/acl/), the
 *   release-pipeline transition graph is StorageWflwEdgeIniSystem (src/workflow/),
 *   and the license tier is StorageEdgeTierSystem (src/edge/), which reads it from the
 *   hub once the hub constants exist.
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
#include <ase/storage/systems/edge/storage_edge_ini_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/state/storage_relm_quot_comp.hpp>
#include <ase/storage/components/state/storage_relm_idn_comp.hpp>
#include <ase/storage/components/tag/storage_relm_glob_tag.hpp>
#include <ase/storage/components/tag/storage_relm_actv_tag.hpp>
#include <ase/storage/components/tag/storage_relm_edge_tag.hpp>
#include <ase/storage/storage_resource_manager.hpp>
#include <ase/storage/types.hpp>
#include <ase/utils/strops.hpp>
// Logging
#include <ase/log/log.hpp>

#include <entt/core/hashed_string.hpp>

namespace ase::storage {

using namespace entt::literals;

// Anonymous namespace for helper FUNCTIONS (NOT static!)
// IMPORTANT: Use anonymous namespace, NOT static keyword!
// NO STRUCTS HERE! Structs = Data = Components!
// NO View/Query operations in helpers! Only pure math!
namespace {

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageEdgeIniSystem::on_start(ecs::Registry& registry) {
    log::debug("[StorageEdgeIni] Started");

    auto* mgr_ptr = registry.ctx().find<StorageResourceManager*>();
    if (!mgr_ptr || !(*mgr_ptr)) {
        // SCHEDULE_ORDER: the system ran before StorageIniSystem placed the manager in ctx.
        // The producer stands in the detail string, the category makes the case countable.
        log::error(log::ERR::CAT::SCHEDULE_ORDER, "StorageEdgeIniSystem", "StorageResourceManager");
        return;
    }
    auto& mgr = **mgr_ptr;

    // Realm directory tree: edge_binaries with release + keys subdirectories
    char dir[512];
    mgr.resolve_path(EDGE_REALM_ID, nullptr, "", dir, 512);
    mgr.ensure_dir(dir);
    mgr.resolve_path(EDGE_REALM_ID, nullptr, "release", dir, 512);
    mgr.ensure_dir(dir);
    mgr.resolve_path(EDGE_REALM_ID, nullptr, "keys", dir, 512);
    mgr.ensure_dir(dir);
    log::info("[StorageEdgeIni] edge_binaries realm directory ready");

    // Realm entity: public platform realm, no concealment. Its license tier is a hub
    // constant that does not exist in Initialization; StorageEdgeTierSystem writes it in
    // Integration from ENG_CONST_TIER_ENTERPRISE, so tier stays zero-initialised here.
    // quota_bytes = the MEASURED 10 GB ceiling (types.hpp EDGE_REALM_QUOTA_BYTES,
    // R14 sizing) - the retired-cleanup keeps the realm small, not the ceiling.
    auto realm_ent = registry.create();
    auto& relm = registry.emplace<StorageStaRelmComponent>(realm_ent);
    ase::utils::str_copy(relm.id, MAX_REALM_ID, EDGE_REALM_ID);
    ase::utils::str_copy(relm.name, MAX_REALM_NAME, "Edge Binary Distribution");
    // The realm has no owner user: it belongs to the platform, so owner_hash stays 0
    // and no requester can ever match it - which is exactly the intended outcome.
    auto& relm_idn = registry.emplace<StorageRelmIdnComponent>(realm_ent);
    relm_idn.id_hash = EDGE_REALM_HASH;
    relm.default_protection = PROTECTION_PUBLIC;
    auto& relm_quot = registry.emplace<StorageRelmQuotComponent>(realm_ent);
    relm_quot.quota_bytes = EDGE_REALM_QUOTA_BYTES;
    registry.emplace<StorageRelmGlobTag>(realm_ent);
    registry.emplace<StorageRelmActvTag>(realm_ent);
    // Identity marker: this system is the ONLY producer of the edge realm, so the
    // tag is the SSOT for "which entity is EDGE_REALM_ID". The workflow systems
    // read it instead of scanning every realm and comparing the id string.
    registry.emplace<StorageRelmEdgeTag>(realm_ent);
    log::info("[StorageEdgeIni] edge_binaries realm entity registered (tier from hub in Integration, public, quota={} bytes)",
              relm_quot.quota_bytes);
}

void StorageEdgeIniSystem::tick(ecs::Registry& /*registry*/, float /*dt*/) {
    // Initialization system has no per-tick logic
}

void StorageEdgeIniSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageEdgeIni] Stopped");
}

}  // namespace ase::storage
