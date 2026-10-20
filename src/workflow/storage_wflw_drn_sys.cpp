/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_wflw_drn_sys.cpp
 * @brief       StorageWflwDrnSystem - Drains Hub-bridge workflow-promote requests
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
 * @usecase     usecase:100329 - stage workflow promotion
 * @created     2026-07-11
 * @modified    2026-10-16
 * @version     1.3.0
 *
 * CAUSAL CHAIN (Workflow Promote Drain)
 *
 *   [a plugin files a hub row HUB_STG_WFLW_REQ with sdk::row_create and sdk::set_bytes]
 *          │
 *          │ Ingestion schedule drains the hub rows
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageWflwDrnSystem          │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - HUB_STG_WFLW_REQ rows (three strings)  │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - StorageReqWflwTranComponent (new)      │
 *   │    - StorageWflwRcvTag (new entity)         │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ row released, request received
 *          ▼
 *   StorageWflwStgeSystem (Ingestion, run_after this system)
 *
 * HUB Pattern (Active)
 *
 * READS (from Hub):
 *   - the rows of the kind HUB_STG_WFLW_REQ (register A0902): path, target label and requester as
 *     byte fields, read at their stored length with hub::bytes_len and hub::try_get_bytes
 *   - the contract widths STG_CONST_WFLW_PATH_BYTES, STG_CONST_WFLW_LABEL_BYTES and
 *     STG_CONST_WFLW_REQUESTER_BYTES on GLOBAL (hub_constants.json), checked against the request
 *     buffers MAX_PATH_LEN, MAX_LABEL_LEN and MAX_OWNER_ID
 *
 * WRITES (to Hub):
 *   (none - the staging and its verdict STG_WFLW_RES belong to StorageWflwStgeSystem)
 *
 * FLYWEIGHT PATTERN (N/A - no external resources)
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
#include <ase/storage/systems/workflow/storage_wflw_drn_sys.hpp>
// Components from same module
#include <ase/storage/components/request/storage_req_wflw_tran_comp.hpp>
#include <ase/storage/components/tag/storage_wflw_rcv_tag.hpp>
#include <ase/storage/types.hpp>
// Hub API (the request rows)
#include <ase/hub/api.hpp>
// NOT_FOUND sentinel and range predicates for the contract widths (L0)
#include <ase/types/types.hpp>
// String ops (L0 — sanitized copy)
#include <ase/utils/strops.hpp>

#include <entt/core/hashed_string.hpp>
// Logging
#include <ase/log/log.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
namespace {

// No helper functions needed - the sanitized bounded copy lives in
// ase::utils::str_copy_json_safe (L0), where the persist document builders reach it too.

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageWflwDrnSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageWflwDrn] Started");
}

void StorageWflwDrnSystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    /**
     * ONE walk over the bridge rows, bounded per tick (WFLW_REQ_BATCH, mirror
     * StorageCredAcssRspSystem); each row is released inside the walk, which row_each permits for
     * the visited row. Rows beyond the bound stay and are drained next tick.
     *
     * A string is read at its stored length into a buffer one byte wider, so it stays terminated.
     * An empty field, one that is not stored, or one wider than the contract's width is a request
     * the caller should not have sent.
     */
    if (hub::row_count(registry, "HUB_STG_WFLW_REQ"_hs) == 0u) return;

    /**
     * THE WIDTHS ARE THE CONTRACT'S (hub_constants.json, read by the writer the same way). Without them
     * no row can be judged against its bound, so the rows wait; a width the request buffers cannot hold
     * with their NUL is a broken contract, not a bad request.
     */
    const float path_bytes = hub::get(registry, hub::GLOBAL, "STG_CONST_WFLW_PATH_BYTES"_hs);
    if (types::is_not_found(path_bytes)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageWflwDrnSystem", "STG_CONST_WFLW_PATH_BYTES");
        return;
    }
    const float label_bytes = hub::get(registry, hub::GLOBAL, "STG_CONST_WFLW_LABEL_BYTES"_hs);
    if (types::is_not_found(label_bytes)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageWflwDrnSystem", "STG_CONST_WFLW_LABEL_BYTES");
        return;
    }
    const float by_bytes = hub::get(registry, hub::GLOBAL, "STG_CONST_WFLW_REQUESTER_BYTES"_hs);
    if (types::is_not_found(by_bytes)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageWflwDrnSystem", "STG_CONST_WFLW_REQUESTER_BYTES");
        return;
    }
    if (!types::is_in_rng_f(path_bytes, 1.0f, static_cast<float>(MAX_PATH_LEN - 1u))) {
        log::error(log::ERR::CAT::VALUE_OUT_OF_RANGE, "StorageWflwDrnSystem", "STG_CONST_WFLW_PATH_BYTES", path_bytes);
        return;
    }
    if (!types::is_in_rng_f(label_bytes, 1.0f, static_cast<float>(MAX_LABEL_LEN - 1u))) {
        log::error(log::ERR::CAT::VALUE_OUT_OF_RANGE, "StorageWflwDrnSystem", "STG_CONST_WFLW_LABEL_BYTES", label_bytes);
        return;
    }
    if (!types::is_in_rng_f(by_bytes, 1.0f, static_cast<float>(MAX_OWNER_ID - 1u))) {
        log::error(log::ERR::CAT::VALUE_OUT_OF_RANGE, "StorageWflwDrnSystem", "STG_CONST_WFLW_REQUESTER_BYTES", by_bytes);
        return;
    }
    const uint32_t path_max = static_cast<uint32_t>(path_bytes);
    const uint32_t label_max = static_cast<uint32_t>(label_bytes);
    const uint32_t by_max = static_cast<uint32_t>(by_bytes);

    uint32_t drained = 0u;
    hub::row_each(registry, "HUB_STG_WFLW_REQ"_hs, [&](ecs::Entity bridge) {
        if (drained >= WFLW_REQ_BATCH) return;
        ++drained;
        const uint32_t row = static_cast<uint32_t>(bridge);
        const uint32_t path_len = hub::bytes_len(registry, row, "HUB_STG_WFLW_REQ_PATH"_hs);
        const uint32_t label_len = hub::bytes_len(registry, row, "HUB_STG_WFLW_REQ_TARGET_LABEL"_hs);
        const uint32_t by_len = hub::bytes_len(registry, row, "HUB_STG_WFLW_REQ_REQUESTED_BY"_hs);
        char path[MAX_PATH_LEN] = {};
        char target_label[MAX_LABEL_LEN] = {};
        char requested_by[MAX_OWNER_ID] = {};
        if (path_len == 0u || label_len == 0u || by_len == 0u ||
            path_len > path_max || label_len > label_max || by_len > by_max ||
            !hub::try_get_bytes(registry, row, "HUB_STG_WFLW_REQ_PATH"_hs, path, path_len) ||
            !hub::try_get_bytes(registry, row, "HUB_STG_WFLW_REQ_TARGET_LABEL"_hs, target_label,
                                label_len) ||
            !hub::try_get_bytes(registry, row, "HUB_STG_WFLW_REQ_REQUESTED_BY"_hs, requested_by,
                                by_len)) {
            // INPUT_REJECTED (WRN::CAT, SSOT core/ase-log/data/log_categories.json — die
            // Kategorie IST ihr Name): eine Bruecken-Anfrage von aussen ohne Pflichtfeld wird
            // verworfen — woertlich "caller sent something unserviceable". Die Ebene ist warn,
            // und das ist hier KEIN Sichtbarkeitsverlust wie auf dem Autorisierungspfad: eine
            // unvollstaendige Anfrage ist kein Sicherheitsereignis, sondern ein Formfehler des
            // Aufrufers — genau das sagt der Hilfetext ("not an engine fault").
            log::warn(log::WRN::CAT::INPUT_REJECTED, "StorageWflwDrn", "bridge_request_fields");
            hub::row_release(registry, bridge);
            return;
        }

        auto req_ent = registry.create();
        auto& req = registry.emplace<StorageReqWflwTranComponent>(req_ent);
        ase::utils::str_copy_json_safe(req.path, MAX_PATH_LEN, path);
        ase::utils::str_copy_json_safe(req.target_label, MAX_LABEL_LEN, target_label);
        ase::utils::str_copy_json_safe(req.requested_by, MAX_OWNER_ID, requested_by);
        // StorageWflwStgeSystem stages it: pending tag, released gate and the verdict.
        registry.emplace<StorageWflwRcvTag>(req_ent);

        hub::row_release(registry, bridge);
    });
}

void StorageWflwDrnSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageWflwDrn] Stopped");
}

}  // namespace ase::storage
