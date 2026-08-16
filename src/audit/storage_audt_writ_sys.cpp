/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_audt_writ_sys.cpp
 * @brief       StorageAudtWritSystem - Batch-writes audit entries to MongoDB
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Preservation
 * @created     2026-04-05
 * @modified    2026-06-24
 * @version     1.0.0
 *
 * CAUSAL CHAIN (Audit Persistence)
 *
 *   [FIVE producers emit one audit entity per access decision]
 *     StorageAcssChkSystem, StorageWflwTranSystem, StorageWflwGateSystem,
 *     StorageWflwClnSystem, StorageEdgeAudtDrnSystem
 *          │
 *          │ StorageSrvlLogSystem reads the pending set FIRST (see below)
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageAudtWritSystem         │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageBufAudtComponent (audit data)   │
 *   │    - StorageAudtPendTag (pending entries)   │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - frame 122 onto the outbound queue      │
 *   │    - destroys the audit entity (sole owner) │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Replica REPLACE-upserts into storage_access_audit
 *          ▼
 *   Durable forensic trail, dist RAM returned
 *
 * WHY THIS SYSTEM RETIRES THE ENTITY
 *   Measured 2026-08-16: five producers, ZERO consumers, ZERO destroys. Every
 *   grant and every deny leaked an entity for the lifetime of the process, and
 *   no decision ever reached durable storage. Both halves are the same missing
 *   drain, so both are fixed here - shipping without retiring would only move
 *   the leak, and retiring without shipping would destroy the evidence.
 *
 * ORDER AGAINST StorageSrvlLogSystem (this cost a redesign, do not undo it)
 *   The header used to promise "tags removed, entities available for
 *   surveillance scanning". That promise cannot hold: Preservation is schedule
 *   71 and Observation is 72, so THIS system runs FIRST inside the frame and
 *   whatever it retires is gone before an Observation reader looks. The fix is
 *   the order, not a retention flag - StorageSrvlLogSystem was moved into
 *   Preservation with run_after on nothing, and this system now runs after it.
 *   Anything that needs to read an access decision must join that ordering.
 *
 * HUB Pattern (N/A - No Hub reads/writes)
 *
 * READS (from Hub):
 *   (none)
 *
 * WRITES (to Hub):
 *   (none)
 *
 * FLYWEIGHT PATTERN (Active - transport::OutboundQueueResourceManager via ctx)
 *   The dist tier has no MongoDB client and never gets one: durability belongs
 *   to the Replica. This system only stages a frame; absent an outbound lane
 *   (unit test, tier without transport) it does nothing and the entries stay
 *   pending rather than being silently dropped.
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
#include <ase/storage/systems/audit/storage_audt_writ_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_buf_audt_comp.hpp>
#include <ase/storage/components/tag/storage_tag_audt_pend.hpp>
#include <ase/storage/types.hpp>
// Transport (L1 via ctx — outbound frame staging, mirror StorageWflwPstSystem)
#include <ase/transport/outbound_queue_resource_manager.hpp>
// String ops (L0)
#include <ase/utils/strops.hpp>

#include <entt/core/hashed_string.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>
#include <cstring>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (pure doc build, no View/Query).
namespace {

// Serialize one access-decision document. Field order mirrors the frame-122
// contract registered in ase-network types.hpp: {"relm_ref","proj_ref",
// "user_id","action","path","result","reason","timestamp"}. Worst case is
// 64 + 256 + 64 chars of payload plus keys and decimals, well inside
// AUDT_PST_DOC_BUF, so the bounded appends never truncate a field away.
//
// The three string fields go through str_append_json_safe rather than plain
// concatenation: the audit buffer is filled at five production sites straight
// from request data, and `path` in particular is whatever the customer asked
// for. An unescaped quote would close the JSON string early and let the subject
// of an access decision write arbitrary fields into the very record meant to
// convict them.
void build_audt_doc(char* doc, uint32_t doc_size, uint32_t relm_ref, uint32_t proj_ref,
                    const char* user_id, uint8_t action, const char* path,
                    uint8_t result, const char* reason, uint64_t timestamp) {
    ase::utils::str_copy(doc, doc_size, "{\"relm_ref\":");
    ase::utils::str_append_u64(doc, doc_size, static_cast<uint64_t>(relm_ref));
    ase::utils::str_append(doc, doc_size, ",\"proj_ref\":");
    ase::utils::str_append_u64(doc, doc_size, static_cast<uint64_t>(proj_ref));
    ase::utils::str_append(doc, doc_size, ",\"user_id\":\"");
    ase::utils::str_append_json_safe(doc, doc_size, user_id);
    ase::utils::str_append(doc, doc_size, "\",\"action\":");
    ase::utils::str_append_u64(doc, doc_size, static_cast<uint64_t>(action));
    ase::utils::str_append(doc, doc_size, ",\"path\":\"");
    ase::utils::str_append_json_safe(doc, doc_size, path);
    ase::utils::str_append(doc, doc_size, "\",\"result\":");
    ase::utils::str_append_u64(doc, doc_size, static_cast<uint64_t>(result));
    ase::utils::str_append(doc, doc_size, ",\"reason\":\"");
    ase::utils::str_append_json_safe(doc, doc_size, reason);
    ase::utils::str_append(doc, doc_size, "\",\"timestamp\":");
    ase::utils::str_append_u64(doc, doc_size, timestamp);
    ase::utils::str_append(doc, doc_size, "}");
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageAudtWritSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageAudtWrit] Started");
}

void StorageAudtWritSystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    auto* out = registry.ctx().find<transport::OutboundQueueResourceManager>();
    if (out == nullptr) return;  // no outbound lane on this tier → entries stay pending

    // Deferred deletion: collect shipped decisions, retire them after the pass.
    ecs::Entity done[AUDT_WRIT_BATCH];
    uint32_t done_n = 0;

    auto aud_view = registry.view<StorageBufAudtComponent, StorageAudtPendTag>();
    for (auto [aud_ent, aud] : aud_view.each()) {
        if (done_n >= AUDT_WRIT_BATCH) break;  // bound ships to what we can also retire

        char doc[AUDT_PST_DOC_BUF] = {};
        build_audt_doc(doc, AUDT_PST_DOC_BUF, aud.relm_ref, aud.proj_ref, aud.user_id,
                       aud.action, aud.path, aud.result, aud.reason, aud.timestamp);
        const uint32_t doc_len = ase::utils::str_len(doc, AUDT_PST_DOC_BUF);

        // [122][req_id:u64][doc_len:u32][doc]. req_id is a correlation token only -
        // the Replica REPLACE-upserts on the parsed {user_id, path, timestamp}, so
        // a wire retry overwrites its own row instead of duplicating the decision.
        // Path hash in the high half, decision second in the low half: two denials
        // on the same path in different seconds stay distinguishable in a trace.
        char frame[AUDT_PST_HDR + AUDT_PST_DOC_BUF] = {};
        frame[0] = static_cast<char>(EDGE_AUDT_BIN_MSG_PERSIST);
        const uint64_t req_id =
            (static_cast<uint64_t>(entt::hashed_string(aud.path).value()) << 32) |
            (aud.timestamp & 0xFFFFFFFFull);
        std::memcpy(frame + 1, &req_id, 8);
        std::memcpy(frame + 9, &doc_len, 4);
        std::memcpy(frame + AUDT_PST_HDR, doc, doc_len);
        out->push_outbound(frame, AUDT_PST_HDR + doc_len);

        log::info("[StorageAudtWrit] persisted access decision user={} path={} action={} result={} (frame 122, {} bytes)",
                  aud.user_id, aud.path, static_cast<uint32_t>(aud.action),
                  static_cast<uint32_t>(aud.result), AUDT_PST_HDR + doc_len);

        done[done_n] = aud_ent;
        ++done_n;
    }

    for (uint32_t i = 0; i < done_n; ++i) {
        registry.destroy(done[i]);
    }
}

void StorageAudtWritSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageAudtWrit] Stopped");
}

}  // namespace ase::storage
