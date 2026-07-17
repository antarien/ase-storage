/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_wflw_pst_sys.cpp
 * @brief       StorageWflwPstSystem - durable workflow-label persist (frame 112)
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Preservation
 * @created     2026-07-11
 * @modified    2026-07-11
 * @version     1.0.0
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
 *   │    - StorageBufWflwComponent                │
 *   │    - StorageWflwPstPendTag (filter)         │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - [112][req_id:u64][doc_len:u32][doc]    │
 *   │      onto OutboundQueueResourceManager      │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Replica REPLACE-upserts storage_workflow_labels keyed {realm,path}
 *          ▼
 *   ReplicaEdgeWflwPstSystem (ase-replication, Preservation)
 *
 * HUB Pattern (N/A - No Hub reads/writes)
 *
 * READS (from Hub):
 *   (none)
 *
 * WRITES (to Hub):
 *   (none — the document rides the binary wire as string DATA, never the Hub)
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
#include <ase/storage/systems/workflow/storage_wflw_pst_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_buf_wflw_comp.hpp>
#include <ase/storage/components/tag/storage_tag_wflw_pst_pend.hpp>
#include <ase/storage/types.hpp>
// Transport (L1 via ctx — outbound frame staging, mirror StorageCredAcssRspSystem)
#include <ase/transport/outbound_queue_resource_manager.hpp>
#include <ase/transport/types.hpp>
// String ops (L0)
#include <ase/utils/strops.hpp>

#include <entt/core/hashed_string.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>
#include <cstring>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (pure frame/doc build, no View/Query).
namespace {

// Append an unsigned decimal to a bounded string buffer. Pure string math
// (base-10 digit extraction via DECIMAL_RADIX; the do-while emits at least one
// digit, so a zero value renders as "0" without a special case).
void append_u64_decimal(char* out, uint32_t out_size, uint64_t v) {
    char digits[21] = {};
    uint32_t n = 0;
    do {
        digits[n] = static_cast<char>('0' + (v % DECIMAL_RADIX));
        ++n;
        v /= DECIMAL_RADIX;
    } while (v > 0 && n < 20);
    char rev[21] = {};
    for (uint32_t i = 0; i < n; ++i) {
        rev[i] = digits[n - 1 - i];
    }
    rev[n] = '\0';
    ase::utils::str_append(out, out_size, rev);
}

// Serialize one workflow-label document ({"realm","path","label","updated_by",
// "updated_at"}). All string fields were sanitized at drain time (quotes and
// backslashes stripped), so plain concatenation yields valid JSON.
void build_wflw_doc(char* doc, uint32_t doc_size, const char* realm, const char* path,
                    const char* label, const char* updated_by, uint64_t updated_at) {
    ase::utils::str_copy(doc, doc_size, "{\"realm\":\"");
    ase::utils::str_append(doc, doc_size, realm);
    ase::utils::str_append(doc, doc_size, "\",\"path\":\"");
    ase::utils::str_append(doc, doc_size, path);
    ase::utils::str_append(doc, doc_size, "\",\"label\":\"");
    ase::utils::str_append(doc, doc_size, label);
    ase::utils::str_append(doc, doc_size, "\",\"updated_by\":\"");
    ase::utils::str_append(doc, doc_size, updated_by);
    ase::utils::str_append(doc, doc_size, "\",\"updated_at\":");
    append_u64_decimal(doc, doc_size, updated_at);
    ase::utils::str_append(doc, doc_size, "}");
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageWflwPstSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageWflwPst] Started");
}

void StorageWflwPstSystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    auto* out = registry.ctx().find<transport::OutboundQueueResourceManager>();
    if (out == nullptr) return;  // no outbound lane on this tier → buffers stay queued

    // Deferred deletion: collect shipped buffers, destroy after the loop.
    ecs::Entity done[WFLW_REQ_BATCH];
    uint32_t done_n = 0;

    auto buf_view = registry.view<StorageBufWflwComponent, StorageWflwPstPendTag>();
    for (auto [buf_ent, buf] : buf_view.each()) {
        if (done_n >= WFLW_REQ_BATCH) break;

        char doc[WFLW_PST_DOC_BUF] = {};
        build_wflw_doc(doc, WFLW_PST_DOC_BUF, buf.realm, buf.path, buf.label,
                       buf.updated_by, buf.updated_at);
        const uint32_t doc_len = ase::utils::str_len(doc, WFLW_PST_DOC_BUF);

        // [112][req_id:u64][doc_len:u32][doc] — same envelope as the keycard
        // persist frame 35; req_id = hashed_string(path) is a correlation token
        // only, the Replica upsert keys on the parsed {realm,path}.
        char frame[13 + WFLW_PST_DOC_BUF] = {};
        frame[0] = static_cast<char>(EDGE_WFLW_BIN_MSG_PERSIST);
        const uint64_t req_id = static_cast<uint64_t>(entt::hashed_string(buf.path).value());
        std::memcpy(frame + 1, &req_id, 8);
        std::memcpy(frame + 9, &doc_len, 4);
        std::memcpy(frame + 13, doc, doc_len);
        out->push_outbound(frame, 13u + doc_len);

        log::info("[StorageWflwPst] persisted workflow label {} for {} (frame 112, {} bytes)",
                  buf.label, buf.path, 13u + doc_len);

        done[done_n] = buf_ent;
        ++done_n;
    }

    for (uint32_t i = 0; i < done_n; ++i) {
        registry.destroy(done[i]);
    }
}

void StorageWflwPstSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageWflwPst] Stopped");
}

}  // namespace ase::storage
