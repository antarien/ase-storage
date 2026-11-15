/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_kycd_rev_syn_sys.cpp
 * @brief       StorageKycdRevSynSystem - marks the keycard one revocation row names
 * @description Hub READ only. The digest of a revocation request is taken from the hub row and
 *              compared, in one pass, against the keycards that are not yet revoked; the keycard it
 *              names receives StorageKycdRevTag (CAUSA_ASE_STORAGE, package 4, step 3).
 *
 * @design      DSGN_021
 * @usecase     usecase:101096 - revoke edge token
 * @actor       Provider Credential Keeper (PROVCREKEE)
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @parity      server_only
 * @schedule    Ingestion
 * @created     2026-11-05
 * @modified    2026-11-05
 * @version     00.00.00.00002 [seed]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * CAUSAL CHAIN (operator route, star row, revocation mark)
 *
 *   ase-pl-webserver: DELETE /api/realms/:realmId/keycards/:hash
 *          │
 *          │ WebserverKycdRevDrnSystem files HUB_STG_KYCD_REV_REQ with the mark HUB_STG_KYCD_REV
 *          ▼
 *   ┌─────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageKycdRevSynSystem                       │
 *   │                                                             │
 *   │  READS:                                                     │
 *   │    - STG_CONST_KYCD_HASH_BYTES (GLOBAL)                     │
 *   │    - rows with the mark HUB_STG_KYCD_REV                    │
 *   │      field HUB_STG_KYCD_REV_REQ_HASH                        │
 *   │    - StorageStaKycdComponent.kycd_hash                      │
 *   │                                                             │
 *   │  WRITES:                                                    │
 *   │    - StorageKycdRevTag on the keycard the digest names      │
 *   │    - takes the mark HUB_STG_KYCD_REV off the row            │
 *   └─────────────────────────────────────────────────────────────┘
 *          │
 *          │ StorageKycdRevTag on the keycard entity
 *          ▼
 *   StorageAcssChkSystem denies the keycard at once; StorageKycdRevSystem and
 *   StorageKycdPstEmitSystem carry the revocation to the Replica on frame 35
 *
 * THE MARK IS THE ANSWER. Every row read loses its mark in the same pass, matched or not: the
 * writer takes every unmarked row back in Integration, and a row that kept its mark would be read
 * again in every frame without ever naming a keycard. The reader never destroys the row.
 *
 * THIS SYSTEM MINTS NOTHING. The boundary in the head of storage_kycd_tier_seed_sys.cpp holds
 * here too: no keycard is issued and no request for one is taken. The only write on a keycard
 * entity is the revocation mark.
 *
 * HUB Pattern (MIG_ASE_HUB_API v2.0):
 *
 * READS (from the star):
 *   "STG_CONST_KYCD_HASH_BYTES"_hs   → length every revocation digest must carry (GLOBAL)
 *   "HUB_STG_KYCD_REV"_hs            → mark of the open revocation rows
 *   "HUB_STG_KYCD_REV_REQ_HASH"_hs   → digest bytes of the keycard to revoke
 *
 * WRITES (to the star):
 *   (none, the mark is taken off the row)
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
 * [ ] NO switch/case statements? (use Tag-filtered Views (separate View per type)!)
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
#include <ase/storage/systems/keycard/storage_kycd_rev_syn_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_sta_kycd_comp.hpp>
#include <ase/storage/components/tag/storage_kycd_rev_tag.hpp>
// Keycard widths (MAX_KEYCARD_HASH)
#include <ase/storage/types.hpp>
// Hub for the O(1) API - the revocation rows are read here and only here
#include <ase/hub/api.hpp>
// Sentinel and range predicates
#include <ase/types/types.hpp>
// str_equal - bounded string comparison (Layer 0)
#include <ase/utils/strops.hpp>
// Logging
#include <ase/log/log.hpp>

#include <entt/core/hashed_string.hpp>

#include <cstdint>

namespace ase::storage {
using namespace entt::literals;

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * IMPORTANT: Use anonymous namespace, NOT static keyword!
 *   OK: namespace { void helper() {...} }   // CORRECT
 *   NO: static void helper() {...}          // WRONG!
 * NO STRUCTS HERE! Structs = Data = Components!
 */
namespace {

/**
 * No helper functions needed for this system.
 * All logic is inline in tick().
 */

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageKycdRevSynSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdRevSynSystem] Started");
}

void StorageKycdRevSynSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * ONE ROW, ONE DIGEST, ONE PASS OVER THE KEYCARDS, ONE MARK TAKEN OFF.
     *
     * The digest length comes from the star; without it no row can be judged, and the rows keep
     * their marks until the constant stands. It must fit the width of kycd_hash.
     */
    const float hash_bytes = hub::get(registry, hub::GLOBAL, "STG_CONST_KYCD_HASH_BYTES"_hs);
    if (types::is_not_found(hash_bytes)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageKycdRevSynSystem",
                   "STG_CONST_KYCD_HASH_BYTES");
        return;
    }
    if (!types::is_in_rng_f(hash_bytes, 1.0f, static_cast<float>(MAX_KEYCARD_HASH))) {
        log::error(log::ERR::CAT::VALUE_OUT_OF_RANGE, "StorageKycdRevSynSystem",
                   "STG_CONST_KYCD_HASH_BYTES", hash_bytes);
        return;
    }
    const uint32_t hash_len = static_cast<uint32_t>(hash_bytes);

    hub::mark_each(registry, "HUB_STG_KYCD_REV"_hs, [&](ecs::Entity row_entity) {
        const uint32_t row = static_cast<uint32_t>(row_entity);
        const uint32_t len = hub::bytes_len(registry, row, "HUB_STG_KYCD_REV_REQ_HASH"_hs);
        if (len != hash_len) {
            log::warn(log::WRN::CAT::INPUT_REJECTED, "StorageKycdRevSynSystem", row,
                      "HUB_STG_KYCD_REV_REQ_HASH",
                      "the revocation digest has the wrong length and names no keycard");
            hub::row_untag(registry, row_entity, "HUB_STG_KYCD_REV"_hs);
            return;
        }
        char digest[MAX_KEYCARD_HASH] = {};
        if (!hub::try_get_bytes(registry, row, "HUB_STG_KYCD_REV_REQ_HASH"_hs, digest, hash_len)) {
            log::warn(log::WRN::CAT::INPUT_INCOMPLETE, "StorageKycdRevSynSystem", row,
                      "HUB_STG_KYCD_REV_REQ_HASH", "the revocation row carries no digest");
            hub::row_untag(registry, row_entity, "HUB_STG_KYCD_REV"_hs);
            return;
        }

        // The keycard set is walked once per row. The mark is emplaced with emplace_or_replace
        // on the entity of the current iteration only, a type the view does not iterate.
        uint32_t matched = 0u;
        for (auto [kycd_entity, kycd] : registry.view<StorageStaKycdComponent>().each()) {
            if (!utils::str_equal(kycd.kycd_hash, digest, hash_len)) {
                continue;
            }
            registry.emplace_or_replace<StorageKycdRevTag>(kycd_entity);
            ++matched;
            log::info("[StorageKycdRevSynSystem] keycard revoked (row={} issued_to='{}')",
                      row, kycd.issued_to);
        }
        if (matched == 0u) {
            log::warn(log::WRN::CAT::INPUT_REJECTED, "StorageKycdRevSynSystem", row,
                      "HUB_STG_KYCD_REV_REQ_HASH", "the revocation digest names no issued keycard");
        }
        hub::row_untag(registry, row_entity, "HUB_STG_KYCD_REV"_hs);
    });
}

void StorageKycdRevSynSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdRevSynSystem] Stopped");
}

}  // namespace ase::storage
