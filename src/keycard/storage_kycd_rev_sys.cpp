/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_kycd_rev_sys.cpp
 * @brief       StorageKycdRevSystem - files one durable revoke record per revoked keycard
 * @description Turns StorageKycdRevTag into a persist record (StorageBufKycdComponent with
 *              op = KYCD_PST_OP_REVOKE + StorageKycdPstPendTag) that StorageKycdPstEmitSystem
 *              ships to the Replica on frame 35.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Preservation
 * @design      DSGN_021
 * @usecase     usecase:101599 - revoke customer keycard
 * @actor       Account Protection Warden (ACCPROWARD)
 * @created     2026-04-05
 * @modified    2026-11-05
 * @version     00.00.39.00039 [core]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * CAUSAL CHAIN (CAUSA_ASE_STORAGE section 3: revocation rides frame 35)
 *
 *   [StorageKycdRevSynSystem sets StorageKycdRevTag on the keycard]
 *          │
 *          │ StorageStaKycdComponent + StorageKycdRevTag
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageKycdRevSystem          │
 *   │                                             │
 *   │  READS:                                     │
 *   │    → StorageStaKycdComponent                │
 *   │    → StorageKycdRevTag                      │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    → StorageBufKycdComponent (record)       │
 *   │    → StorageBufKycdAccComponent (record)    │
 *   │    → StorageKycdPstPendTag (record)         │
 *   │    → StorageKycdRevPstTag (emit-once mark)  │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ record entity, op = KYCD_PST_OP_REVOKE
 *          ▼
 *   StorageKycdPstEmitSystem → frame 35 → Replica REPLACE-upsert on user_id (tombstone)
 *
 * HUB Pattern (READS/WRITES)
 *
 * READS:  none - the revocation is read from storage components only
 * WRITES: none - the durable revoke leaves as DATA on frame 35, never as a hub key
 *
 * STAR Pattern (Active - record entity as the hand-over point)
 *   The keycard entity stays untouched apart from its emit-once mark; the revocation is handed
 *   on as its own record entity, so the emit system walks records and never keycards.
 *
 * FLYWEIGHT PATTERN (Active - none held here, the transport resource is read by the emit system)
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
#include <ase/storage/systems/keycard/storage_kycd_rev_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_sta_kycd_comp.hpp>
#include <ase/storage/components/state/storage_buf_kycd_comp.hpp>
#include <ase/storage/components/state/storage_buf_kycd_acc_comp.hpp>
#include <ase/storage/components/tag/storage_kycd_rev_tag.hpp>
#include <ase/storage/components/tag/storage_kycd_rev_pst_tag.hpp>
#include <ase/storage/components/tag/storage_kycd_pst_pend_tag.hpp>
// Module constants (KYCD_PST_OP_REVOKE)
#include <ase/storage/types.hpp>
// Bounded string copy
#include <ase/utils/strops.hpp>
// Logging
#include <ase/log/log.hpp>

#include <entt/core/hashed_string.hpp>

#include <cstdint>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
namespace {

// No helper functions needed → all logic inline in tick()

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageKycdRevSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdRev] Started");
}

void StorageKycdRevSystem::tick(ecs::Registry& registry, float /*dt*/) {
    // The A/ACS check already denies every keycard carrying StorageKycdRevTag in memory. This
    // system makes the revocation durable: it files one record entity that the emit system
    // ships on frame 35. The Replica REPLACE-upserts on user_id, so the record names the
    // recipient; StorageKycdRevPstTag makes the filing exactly-once.
    auto view = registry.view<StorageStaKycdComponent, StorageKycdRevTag>(
        entt::exclude<StorageKycdRevPstTag>);
    for (auto [entity, kycd] : view.each()) {
        // Without a recipient the durable document has no upsert key; the Replica cannot
        // address it. The resource is present and unusable: RESOURCE_UNAVAIL.
        if (kycd.issued_to[0] == '\0') {
            log::error(log::ERR::CAT::RESOURCE_UNAVAIL, "StorageKycdRev",
                       static_cast<uint32_t>(entity), "STG_KYCD_issued_to");
            registry.emplace<StorageKycdRevPstTag>(entity);
            continue;
        }

        const auto record = registry.create();
        auto& buf = registry.emplace<StorageBufKycdComponent>(record);
        ase::utils::str_copy(buf.kycd_hash, sizeof(buf.kycd_hash), kycd.kycd_hash);
        ase::utils::str_copy(buf.issued_to, sizeof(buf.issued_to), kycd.issued_to);
        ase::utils::str_copy(buf.issued_by, sizeof(buf.issued_by), kycd.issued_by);
        buf.op = KYCD_PST_OP_REVOKE;
        auto& acc = registry.emplace<StorageBufKycdAccComponent>(record);
        acc.relm_ref = kycd.relm_ref;
        acc.proj_ref = kycd.proj_ref;
        registry.emplace<StorageKycdPstPendTag>(record);

        registry.emplace<StorageKycdRevPstTag>(entity);
        log::info("[StorageKycdRev] recipient='{}' hash='{}' durable revoke record filed",
                  kycd.issued_to, kycd.kycd_hash);
    }
}

void StorageKycdRevSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdRev] Stopped");
}

}  // namespace ase::storage
