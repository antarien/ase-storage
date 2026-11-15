/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_edge_tier_sys.cpp
 * @brief       StorageEdgeTierSystem - license tier of the edge_binaries realm from the hub
 * @description Reads ENG_CONST_TIER_ENTERPRISE on GLOBAL in tick and writes it into the tier
 *              byte of the realm StorageRelmEdgeTag marks. The realm is seeded in
 *              Initialization, before the hub constants exist; its tier is set here, in
 *              Integration, after HubIniCnstSystem and before StorageAcssPolSystem.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @design      DSGN_021
 * @usecase     usecase:100265 - check access clearance
 * @actor       Provider Credential Keeper (PROVCREKEE)
 * @created     2026-11-05
 * @modified    2026-11-05
 * @version     00.00.39.00039 [core]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * CAUSAL CHAIN (CHAIN_STORAGE_EDGE_TIER: hub tier code → edge realm tier)
 *
 *   [StorageEdgeIniSystem seeded the edge realm, tier zero-initialised]
 *   [HubIniCnstSystem laid ENG_CONST_TIER_ENTERPRISE on GLOBAL]
 *          │
 *          ▼
 *   ┌───────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageEdgeTierSystem                       │
 *   │                                                           │
 *   │  READS:                                                   │
 *   │    ENG_CONST_TIER_ENTERPRISE (Hub, GLOBAL)                │
 *   │    StorageStaRelmComponent + StorageRelmEdgeTag           │
 *   │                                                           │
 *   │  WRITES:                                                  │
 *   │    StorageStaRelmComponent::tier (edge realm)             │
 *   └───────────────────────────────────────────────────────────┘
 *          │
 *          │ edge realm carries the Enterprise tier code of the hub
 *          ▼
 *   StorageAcssPolSystem step 9 weighs a write against the tier's quota
 *
 * HUB Pattern (ARCH_ASE_HUB)
 *
 * READS (Hub, GLOBAL):
 *   ENG_CONST_TIER_ENTERPRISE → the license tier code of the dedicated tariff
 *
 * WRITES (to ase-storage internal):
 *   StorageStaRelmComponent::tier → the tier byte of the edge realm
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
#include <ase/storage/systems/edge/storage_edge_tier_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/tag/storage_relm_edge_tag.hpp>
// Hub (ENG_CONST_TIER_ENTERPRISE on GLOBAL)
#include <ase/hub/api.hpp>
// Types SSOT (is_not_found / is_in_rng_f on hub::get reads)
#include <ase/types/types.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>

#include <entt/core/hashed_string.hpp>

namespace ase::storage {

using namespace entt::literals;

// Anonymous namespace for helper FUNCTIONS (NOT static!)
// NO View/Query operations in helpers! Only pure math!
namespace {

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageEdgeTierSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageEdgeTier] Started");
}

void StorageEdgeTierSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto edge_view = registry.view<StorageStaRelmComponent, StorageRelmEdgeTag>();
    if (edge_view.begin() == edge_view.end()) {
        return;
    }

    // A tier is a byte on the realm record, so a code outside 0..UINT8_MAX could never be one.
    const float byte_max = static_cast<float>(UINT8_MAX);
    const float tier_f = hub::get(registry, hub::GLOBAL, "ENG_CONST_TIER_ENTERPRISE"_hs);
    if (ase::types::is_not_found(tier_f)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageEdgeTierSystem", "ENG_CONST_TIER_ENTERPRISE");
        return;
    }
    if (!ase::types::is_in_rng_f(tier_f, 0.0f, byte_max)) {
        log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "StorageEdgeTierSystem", hub::GLOBAL,
                  "ENG_CONST_TIER_ENTERPRISE", tier_f, 0.0f, byte_max);
        return;
    }
    const uint8_t tier = static_cast<uint8_t>(tier_f);

    for (auto entity : edge_view) {
        edge_view.get<StorageStaRelmComponent>(entity).tier = tier;
    }
}

void StorageEdgeTierSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageEdgeTier] Stopped");
}

}  // namespace ase::storage
