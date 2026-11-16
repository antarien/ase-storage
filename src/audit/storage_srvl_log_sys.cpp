/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_srvl_log_sys.cpp
 * @brief       StorageSrvlLogSystem - Surveillance anomaly detection
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Preservation
 * @usecase     usecase:101627 - detect a denial streak
 * @actor       Asset Broker (ASSETBRO)
 * @created     2026-04-05
 * @modified    2026-11-16
 * @version     00.00.39.00039 [core]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * CAUSAL CHAIN (Anomaly Detection)
 *
 *   [Audit entities from StorageAcssChkSystem, still pending persistence]
 *          │
 *          │ Preservation, ahead of StorageAudtWritSystem
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageSrvlLogSystem          │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageBufAudtComponent (audit data)   │
 *   │    - StorageAudtOutcComponent (verdict)     │
 *   │    - the owner's previous streak and stamp  │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - the owner's streak and stamp           │
 *   │    - the owner's anomaly verdict            │
 *   │    - log::warn once the streak reaches the  │
 *   │      threshold                              │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ SES_ACSS_ANOMALY on the owner
 *          ▼
 *   storage_routes.cpp (ase-pl-webserver) answers it as "anomaly" to the operator
 *
 * HUB Pattern (MIG_ASE_HUB_API v2.0):
 *
 * READS (from Hub, owner = hash of the audited user id):
 *   "SES_ACSS_DENY_STREAK"_hs  → denials in a row before this one (only when measured)
 *   "SES_ACSS_DENY_STAMP"_hs   → masked stamp of the previous denial (only when measured)
 *
 * WRITES (to Hub, owner = hash of the audited user id):
 *   "SES_ACSS_DENY_STREAK"_hs  → denials in a row including this one
 *   "SES_ACSS_DENY_STAMP"_hs   → masked stamp of this denial
 *   "SES_ACSS_ANOMALY"_hs      → result of streak >= SRVL_DENY_THRESHOLD (1 or 0)
 *   all three removed on the owner's next granted access
 *
 * FLYWEIGHT PATTERN (N/A - the system reads components and hub values only)
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
#include <ase/storage/systems/audit/storage_srvl_log_sys.hpp>
// Components + tags from same module
#include <ase/storage/components/state/storage_buf_audt_comp.hpp>
#include <ase/storage/components/state/storage_audt_outc_comp.hpp>
#include <ase/storage/components/tag/storage_audt_pend_tag.hpp>
#include <ase/storage/types.hpp>
// Hub API (the streak must outlive the tick that produced it)
#include <ase/hub/api.hpp>

#include <entt/core/hashed_string.hpp>
// Logging
#include <ase/log/log.hpp>
// Strings (L0 - exact rendering of the counts and ids in the anomaly line of tick())
#include <ase/utils/strops.hpp>

#include <cstdint>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
namespace {

// No helper functions needed → the scan is one flat Tag-filtered View pass

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageSrvlLogSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageSrvlLog] Started");
}

void StorageSrvlLogSystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    // ONE flat pass over the decisions still pending persistence. This system
    // runs in Preservation AHEAD of StorageAudtWritSystem for a hard reason:
    // the writer ships each decision and destroys its entity in the same stage,
    // and Observation (72) comes after Preservation (71) - a reader scheduled
    // there would find an empty view every frame and report a permanently quiet
    // system while denials streamed through.
    // The verdict half sits on the same entity, so it joins the view instead of
    // being fetched per row - the streak logic reads context and outcome together.
    for (auto [aud_ent, aud, outc] :
         registry.view<StorageBufAudtComponent, StorageAudtOutcComponent, StorageAudtPendTag>()
             .each()) {
        (void)aud_ent;
        if (aud.user_id[0] == '\0') continue;  // unauthenticated attempt - no owner to attribute it to

        const uint32_t owner = entt::hashed_string(aud.user_id).value();

        if (outc.result != AUD_DENIED) {
            // A granted access ends the streak AND retires the rows. Leaving
            // them at 0.0 would be the quiet leak: a Hub value IS an entity, and
            // only remove() destroys it - three rows per user who ever tripped,
            // out of a 20-bit entity index.
            hub::remove(registry, owner, "SES_ACSS_DENY_STREAK"_hs);
            hub::remove(registry, owner, "SES_ACSS_DENY_STAMP"_hs);
            hub::remove(registry, owner, "SES_ACSS_ANOMALY"_hs);
            continue;
        }

        // Every read is followed by is_measured(), not exists(): every tier
        // loads the same hub_metrics.json, so the key is PRESENT with its JSON
        // default even on a tier that never wrote it. Reading that default as a
        // previous streak would invent a history this user does not have.
        const float raw_streak = hub::get(registry, owner, "SES_ACSS_DENY_STREAK"_hs, 0.0f);
        const bool streak_known = hub::is_measured(registry, owner, "SES_ACSS_DENY_STREAK"_hs);
        const float raw_stamp = hub::get(registry, owner, "SES_ACSS_DENY_STAMP"_hs, 0.0f);
        const bool stamp_known = hub::is_measured(registry, owner, "SES_ACSS_DENY_STAMP"_hs);

        const bool has_history = streak_known && stamp_known;
        const uint32_t prev_streak = has_history ? static_cast<uint32_t>(raw_streak) : 0u;
        const uint64_t prev_stamp = has_history ? static_cast<uint64_t>(raw_stamp) : 0u;

        // Masked stamps, so the subtraction stays exact in the Hub's float32
        // lane. The mask also makes the wrap harmless: a gap computed across it
        // comes out large, which only ever ends a streak that was already stale.
        const uint64_t stamp = aud.timestamp & SRVL_STAMP_MASK;
        const uint64_t gap = (stamp - prev_stamp) & SRVL_STAMP_MASK;
        const uint32_t streak = (prev_streak && gap <= SRVL_WINDOW_S) ? prev_streak + 1u : 1u;

        hub::set(registry, owner, "SES_ACSS_DENY_STREAK"_hs, static_cast<float>(streak));
        hub::set(registry, owner, "SES_ACSS_DENY_STAMP"_hs, static_cast<float>(stamp));

        // The verdict IS the comparison, stated on every denial: a streak below the
        // threshold reads 0, so the operator sees a watched owner instead of a missing row.
        const bool anomalous = streak >= static_cast<uint32_t>(SRVL_DENY_THRESHOLD);
        hub::set(registry, owner, "SES_ACSS_ANOMALY"_hs, static_cast<float>(anomalous));

        if (anomalous) {
            // The line reports a PATTERN, not an event: each denial of the streak was
            // decided and logged on its own. Its measurable core is that `streak` lies
            // outside its permitted range (at or above SRVL_DENY_THRESHOLD), which is
            // VALUE_OUT_OF_RANGE; the reaction is left to the caller, and here it is the
            // anomaly verdict above and nothing else.
            //
            // The form with owner, value_id and detail carries all six facts: `owner` is
            // the user hash, value_id the path, and the detail renders streak, threshold,
            // window, action and reason exactly - str_append_u64 prints without float and
            // without rounding.
            //
            // path 256 + reason 64 + the numbers and labels stay below 512, so nothing is
            // cut. The numbers still stand FIRST: should the bound ever bite, text is lost
            // before measurements.
            char deny_detail[512] = {};
            ase::utils::str_copy(deny_detail, 512, "streak=");
            ase::utils::str_append_u64(deny_detail, 512, streak);
            ase::utils::str_append(deny_detail, 512, " threshold=");
            ase::utils::str_append_u64(deny_detail, 512, static_cast<uint64_t>(SRVL_DENY_THRESHOLD));
            ase::utils::str_append(deny_detail, 512, " window_s=");
            ase::utils::str_append_u64(deny_detail, 512, SRVL_WINDOW_S);
            ase::utils::str_append(deny_detail, 512, " action=");
            ase::utils::str_append_u64(deny_detail, 512, static_cast<uint64_t>(outc.action));
            ase::utils::str_append(deny_detail, 512, " reason=");
            ase::utils::str_append(deny_detail, 512, outc.reason);
            log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "StorageSrvlLog", owner, aud.path,
                      deny_detail);
        }
    }
}

void StorageSrvlLogSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageSrvlLog] Stopped");
}

}  // namespace ase::storage
