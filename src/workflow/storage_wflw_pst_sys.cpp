/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_wflw_pst_sys.cpp
 * @brief       StorageWflwPstSystem - durable workflow-label persist (frame 112)
 * @description Builds the workflow-label document of every pending persist buffer and pushes it as
 *              [code][req_id:u64][doc_len:u32][doc] onto the outbound queue. Frame code and document
 *              width are the hub rows of their owners, read on GLOBAL in tick.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Preservation
 * @design      DSGN_021
 * @usecase     usecase:100049 - publish a hub value
 * @actor       Protocol Channel Steward (PROTCHASTE)
 * @created     2026-07-11
 * @modified    2026-11-05
 * @version     00.00.39.00039 [core]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * CAUSAL CHAIN (Workflow-Label Durable Persist)
 *
 *   [StorageWflwTranSystem applied a transition + staged a persist buffer]
 *          │
 *          │ Preservation schedule drains the persist buffers
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageWflwPstSystem          │
 *   │                                             │
 *   │  READS:                                     │
 *   │    → StorageBufWflwComponent                │
 *   │    → StorageWflwPstPendTag (filter)         │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    → [112][req_id:u64][doc_len:u32][doc]    │
 *   │      onto OutboundQueueResourceManager      │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Replica REPLACE-upserts storage_workflow_labels keyed {realm,path}
 *          ▼
 *   ReplicaWflwPstReqSystem (ase-replication, Preservation)
 *
 * HUB Pattern (READS/WRITES)
 *
 * READS (from Hub):
 *   NET_CONST_BIN_MSG_EDGE_WFLW_PERSIST (GLOBAL) - frame code of the workflow-label persist frame
 *   REP_CONST_EDGE_WFLW_PERSIST_DOC_MAX (GLOBAL) - largest workflow-label document in bytes
 *
 * WRITES (to Hub):
 *   none - the document rides the binary wire as string DATA, never the hub
 *
 * FLYWEIGHT PATTERN (Active - transport::OutboundQueueResourceManager via ctx)
 *   Frame staging onto the L1 transport outbound queue.
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
#include <ase/storage/systems/workflow/storage_wflw_pst_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_buf_wflw_comp.hpp>
#include <ase/storage/components/tag/storage_wflw_pst_pend_tag.hpp>
#include <ase/storage/types.hpp>
// Hub API (frame code and document width on GLOBAL)
#include <ase/hub/api.hpp>
// Types (L0 - sentinel and range predicates)
#include <ase/types/types.hpp>
// Transport (L1 via ctx - outbound frame staging; LANE_BUF_SZ bounds every frame)
#include <ase/transport/outbound_queue_resource_manager.hpp>
#include <ase/transport/types.hpp>
// String ops (L0)
#include <ase/utils/strops.hpp>
// Logging
#include <ase/log/log.hpp>

#include <entt/core/hashed_string.hpp>

#include <cstdint>
#include <cstring>

namespace ase::storage {
using namespace entt::literals;  // For "_hs hashed strings (Hub)

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * NO STRUCTS HERE! Structs = Data = Components!
 */
namespace {

// Serialize one workflow-label document ({"realm","path","label","updated_by",
// "updated_at"}). The string fields go through str_append_json_safe: they are
// already sanitized at drain time, but a document assembled from request data
// must not depend on a caller upstream having done it - the same skip applied
// twice is idempotent, while the one time it is missing upstream it is the
// difference between a valid document and an injected one.
void build_wflw_doc(char* doc, uint32_t doc_size, const char* realm, const char* path,
                    const char* label, const char* updated_by, uint64_t updated_at) {
    ase::utils::str_copy(doc, doc_size, "{\"realm\":\"");
    ase::utils::str_append_json_safe(doc, doc_size, realm);
    ase::utils::str_append(doc, doc_size, "\",\"path\":\"");
    ase::utils::str_append_json_safe(doc, doc_size, path);
    ase::utils::str_append(doc, doc_size, "\",\"label\":\"");
    ase::utils::str_append_json_safe(doc, doc_size, label);
    ase::utils::str_append(doc, doc_size, "\",\"updated_by\":\"");
    ase::utils::str_append_json_safe(doc, doc_size, updated_by);
    ase::utils::str_append(doc, doc_size, "\",\"updated_at\":");
    ase::utils::str_append_u64(doc, doc_size, updated_at);
    ase::utils::str_append(doc, doc_size, "}");
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageWflwPstSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageWflwPstSystem] Started");
}

void StorageWflwPstSystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    auto* out = registry.ctx().find<transport::OutboundQueueResourceManager>();
    if (out == nullptr) return;  // no outbound lane on this tier → buffers stay queued

    const float code_f = hub::get(registry, hub::GLOBAL, "NET_CONST_BIN_MSG_EDGE_WFLW_PERSIST"_hs);
    if (ase::types::is_not_found(code_f)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageWflwPstSystem",
                   "NET_CONST_BIN_MSG_EDGE_WFLW_PERSIST");
        return;
    }
    if (!ase::types::is_in_rng_f(code_f, 0.0f, static_cast<float>(UINT8_MAX))) {
        log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "StorageWflwPstSystem", hub::GLOBAL,
                  "NET_CONST_BIN_MSG_EDGE_WFLW_PERSIST", code_f, 0.0f, static_cast<float>(UINT8_MAX));
        return;
    }

    // [code:u8][req_id:u64][doc_len:u32][doc]: the frame is staged in one lane-wide scratch, so a
    // document width the lane cannot carry is refused before any frame is built.
    const uint32_t head = sizeof(uint8_t) + sizeof(uint64_t) + sizeof(uint32_t);
    const float doc_max_f = hub::get(registry, hub::GLOBAL, "REP_CONST_EDGE_WFLW_PERSIST_DOC_MAX"_hs);
    if (ase::types::is_not_found(doc_max_f)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageWflwPstSystem",
                   "REP_CONST_EDGE_WFLW_PERSIST_DOC_MAX");
        return;
    }
    if (!ase::types::is_in_rng_f(doc_max_f, 1.0f, static_cast<float>(transport::LANE_BUF_SZ - head))) {
        log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "StorageWflwPstSystem", hub::GLOBAL,
                  "REP_CONST_EDGE_WFLW_PERSIST_DOC_MAX", doc_max_f, 1.0f,
                  static_cast<float>(transport::LANE_BUF_SZ - head));
        return;
    }
    const uint8_t code = static_cast<uint8_t>(code_f);
    const uint32_t doc_max = static_cast<uint32_t>(doc_max_f);

    // Deferred deletion: collect shipped buffers, destroy after the loop.
    ecs::Entity done[WFLW_REQ_BATCH];
    uint32_t done_n = 0;
    char frame[transport::LANE_BUF_SZ] = {};

    auto buf_view = registry.view<StorageBufWflwComponent, StorageWflwPstPendTag>();
    for (auto [buf_ent, buf] : buf_view.each()) {
        if (done_n >= WFLW_REQ_BATCH) break;

        // The document is built in place behind the frame head, bounded by the owner's width.
        char* doc = frame + head;
        build_wflw_doc(doc, doc_max, buf.realm, buf.path, buf.label, buf.updated_by, buf.updated_at);
        const uint32_t doc_len = ase::utils::str_len(doc, doc_max);
        done[done_n] = buf_ent;
        ++done_n;
        if (doc_len == 0u || doc[doc_len - 1u] != '}') {
            // A document cut at the owner's width is never shipped: the Replica would upsert a
            // label document that parses as garbage. The buffer leaves with the report.
            log::warn(log::WRN::CAT::INPUT_TRUNCATED, "StorageWflwPstSystem",
                      static_cast<uint32_t>(buf_ent), "REP_CONST_EDGE_WFLW_PERSIST_DOC_MAX",
                      "the workflow-label document exceeds the owner's width and is not shipped");
            continue;
        }

        // req_id = hashed_string(path) is a correlation token only, the Replica upsert keys on
        // the parsed {realm,path}.
        frame[0] = static_cast<char>(code);
        const uint64_t req_id = static_cast<uint64_t>(entt::hashed_string(buf.path).value());
        std::memcpy(frame + sizeof(uint8_t), &req_id, sizeof(uint64_t));
        std::memcpy(frame + sizeof(uint8_t) + sizeof(uint64_t), &doc_len, sizeof(uint32_t));
        out->push_outbound(frame, head + doc_len);

        log::info("[StorageWflwPstSystem] persisted workflow label {} for {} (frame {}, {} bytes)",
                  buf.label, buf.path, code, head + doc_len);
    }

    for (uint32_t i = 0; i < done_n; ++i) {
        registry.destroy(done[i]);
    }
}

void StorageWflwPstSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageWflwPstSystem] Stopped");
}

}  // namespace ase::storage
