/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_kycd_pst_emit_sys.cpp
 * @brief       StorageKycdPstEmitSystem - ships keycard persist records on frame 35
 * @description Builds the keycard document of every pending persist record and pushes it as
 *              [35][req_id:u64][doc_len:u32][doc] onto the outbound queue.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Preservation
 * @design      DSGN_021
 * @usecase     usecase:101599 - revoke customer keycard
 * @actor       Account Protection Warden (ACCPROWARD)
 * @created     2026-11-05
 * @modified    2026-11-05
 * @version     1.0.0
 *
 * CAUSAL CHAIN (CAUSA_ASE_STORAGE section 3: revocation rides frame 35)
 *
 *   [StorageKycdRevSystem filed a record: StorageBufKycdComponent + StorageKycdPstPendTag]
 *          │
 *          │ Preservation drains the pending records
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageKycdPstEmitSystem      │
 *   │                                             │
 *   │  READS:                                     │
 *   │    → StorageBufKycdComponent                │
 *   │    → StorageBufKycdAccComponent             │
 *   │    → StorageKycdPstPendTag (filter)         │
 *   │    → StorageStaRelmComponent (via relm_ref) │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    → frame 35 onto the outbound queue       │
 *   │    → StorageKycdPstDoneTag                  │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Replica REPLACE-upserts the keycards document keyed by user_id
 *          ▼
 *   ReplicaKycdPstReqSystem (ase-replication)
 *
 * HUB Pattern (READS/WRITES)
 *
 * READS (from Hub):
 *   REP_CONST_BIN_MSG_EDGE_KYCD_PERSIST (GLOBAL) - frame code of the keycard persist frame
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
#include <ase/storage/systems/keycard/storage_kycd_pst_emit_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_buf_kycd_comp.hpp>
#include <ase/storage/components/state/storage_buf_kycd_acc_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/tag/storage_kycd_pst_pend_tag.hpp>
#include <ase/storage/components/tag/storage_kycd_pst_done_tag.hpp>
#include <ase/storage/types.hpp>
// Hub API (frame code on GLOBAL)
#include <ase/hub/api.hpp>
// Types (L0 - sentinel and range predicates)
#include <ase/types/types.hpp>
// Transport (L1 via ctx - outbound frame staging)
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

// Serialize one keycard document in the field set the Replica upserts and the read lane
// parses: user_id (the upsert key), issued_by, realm, clearance, permission, revoked and the
// codeword list. A revoke record carries no grant: clearance and permission are 0 and the
// codeword list is empty, so the document is the tombstone the next fetch reads. Every string
// goes through str_append_json_safe, the document is assembled from record data.
void build_kycd_doc(char* doc, uint32_t doc_size, const char* user_id, const char* issued_by,
                    const char* realm, uint32_t revoked) {
    ase::utils::str_copy(doc, doc_size, "{\"user_id\":\"");
    ase::utils::str_append_json_safe(doc, doc_size, user_id);
    ase::utils::str_append(doc, doc_size, "\",\"issued_by\":\"");
    ase::utils::str_append_json_safe(doc, doc_size, issued_by);
    ase::utils::str_append(doc, doc_size, "\",\"realm\":\"");
    ase::utils::str_append_json_safe(doc, doc_size, realm);
    ase::utils::str_append(doc, doc_size, "\",\"clearance\":0,\"permission\":0,\"revoked\":");
    ase::utils::str_append_u64(doc, doc_size, revoked);
    ase::utils::str_append(doc, doc_size, ",\"codewords\":[]}");
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageKycdPstEmitSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdPstEmitSystem] Started");
}

void StorageKycdPstEmitSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto* out = registry.ctx().find<transport::OutboundQueueResourceManager>();
    if (out == nullptr) return;  // no outbound lane on this tier → records stay pending

    const float code_f = hub::get(registry, hub::GLOBAL, "REP_CONST_BIN_MSG_EDGE_KYCD_PERSIST"_hs);
    if (ase::types::is_not_found(code_f)) {
        log::error(log::ERR::CAT::HUB_GLOBAL_MISSING, "StorageKycdPstEmitSystem",
                   "REP_CONST_BIN_MSG_EDGE_KYCD_PERSIST");
        return;
    }
    if (!ase::types::is_in_rng_f(code_f, 0.0f, static_cast<float>(UINT8_MAX))) {
        log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "StorageKycdPstEmitSystem", hub::GLOBAL,
                  "REP_CONST_BIN_MSG_EDGE_KYCD_PERSIST", code_f, 0.0f,
                  static_cast<float>(UINT8_MAX));
        return;
    }
    const uint8_t code = static_cast<uint8_t>(code_f);

    auto view = registry.view<StorageBufKycdComponent, StorageBufKycdAccComponent,
                              StorageKycdPstPendTag>();
    for (auto [record, buf, acc] : view.each()) {
        // The realm travels as its path id; the record holds only the entity reference.
        const char* realm = "";
        const auto relm_ent = static_cast<entt::entity>(acc.relm_ref);
        const auto* relm = registry.valid(relm_ent)
            ? registry.try_get<StorageStaRelmComponent>(relm_ent) : nullptr;
        if (relm == nullptr) {
            log::warn(log::WRN::CAT::REFERENCE_UNRESOLVED, "StorageKycdPstEmitSystem",
                      acc.relm_ref, "STG_KYCD_relm_ref");
        } else {
            realm = relm->id;
        }

        // The document holds at most the strings of the record and of the realm row, so the
        // two component widths bound it.
        char doc[sizeof(StorageBufKycdComponent) + sizeof(StorageStaRelmComponent)] = {};
        build_kycd_doc(doc, sizeof(doc), buf.issued_to, buf.issued_by, realm,
                       static_cast<uint32_t>(buf.op == KYCD_PST_OP_REVOKE));
        const uint32_t doc_len = ase::utils::str_len(doc, sizeof(doc));

        // [code:u8][req_id:u64][doc_len:u32][doc]; req_id = hashed_string(user_id) is a
        // correlation token only, the Replica upsert keys on the parsed user_id.
        const uint32_t head = sizeof(uint8_t) + sizeof(uint64_t) + sizeof(uint32_t);
        char frame[sizeof(uint8_t) + sizeof(uint64_t) + sizeof(uint32_t) + sizeof(doc)] = {};
        frame[0] = static_cast<char>(code);
        const uint64_t req_id = static_cast<uint64_t>(entt::hashed_string(buf.issued_to).value());
        std::memcpy(frame + sizeof(uint8_t), &req_id, sizeof(uint64_t));
        std::memcpy(frame + sizeof(uint8_t) + sizeof(uint64_t), &doc_len, sizeof(uint32_t));
        std::memcpy(frame + head, doc, doc_len);
        out->push_outbound(frame, head + doc_len);

        registry.remove<StorageKycdPstPendTag>(record);
        registry.emplace<StorageKycdPstDoneTag>(record);
        log::info("[StorageKycdPstEmitSystem] recipient='{}' op={} shipped on frame {} ({} bytes)",
                  buf.issued_to, buf.op, code, head + doc_len);
    }
}

void StorageKycdPstEmitSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdPstEmitSystem] Stopped");
}

}  // namespace ase::storage
