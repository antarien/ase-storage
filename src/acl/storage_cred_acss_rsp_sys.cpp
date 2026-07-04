/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_cred_acss_rsp_sys.cpp
 * @brief       StorageCredAcssRspSystem - ships the A/ACS verdict back to the Replica
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Reception
 * @created     2026-07-04
 * @modified    2026-07-04
 * @version     1.0.0
 *
 * CAUSAL CHAIN (Credential A/ACS Verdict Emit)
 *
 *   [StorageAcssChkSystem emplaced StorageAcssGrantTag/DenyTag on the request entity]
 *          │
 *          │ request entity: StorageReqAcssComponent + StorageCredAcssPndComponent + verdict tag
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageCredAcssRspSystem      │
 *   │                                             │
 *   │  READS (Tag-filtered Views):                │
 *   │    - <ReqAcss, CredAcssPnd, AcssGrantTag>   │
 *   │    - <ReqAcss, CredAcssPnd, AcssDenyTag>    │
 *   │                                             │
 *   │  WRITES:                                     │
 *   │    - transport outbound CACSS_WIRE_RES frame│
 *   │    - destroys the request entity (deferred) │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ [87][req_id][verdict][reason] → L2 demux ws->send() → Replica
 *          ▼
 *   ReplicaRcvSystem correlates req_id → the pending credential request → gate the Vault op.
 *
 * HUB Pattern (N/A — transport outbound + components, no Hub values)
 *
 * READS (from Hub):
 *   (none)
 *
 * WRITES (to Hub):
 *   (none)
 *
 * FLYWEIGHT PATTERN (Active — transport::OutboundQueueResourceManager via ctx)
 *   The L1 outbound queue is a ctx flyweight the L2 demux drains + ws->send()s; this system pushes to it.
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
// Own header FIRST
#include <ase/storage/systems/acl/storage_cred_acss_rsp_sys.hpp>
// Components + tags from same module
#include <ase/storage/components/state/storage_req_acss_comp.hpp>
#include <ase/storage/components/state/storage_cred_acss_pnd_comp.hpp>
#include <ase/storage/components/tag/storage_tag_acss_grant.hpp>
#include <ase/storage/components/tag/storage_tag_acss_deny.hpp>
#include <ase/storage/types.hpp>
// Lower layers
#include <ase/transport/outbound_queue_resource_manager.hpp>
#include <ase/transport/types.hpp>
#include <ase/log/log.hpp>

#include <cstdint>
#include <cstring>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (pure frame build + push, no View/Query).
namespace {

// Stage one CACSS_WIRE_RES frame [87][req_id:u64][verdict:u8][reason:u8] onto the outbound queue.
void emit_verdict(transport::OutboundQueueResourceManager* out, uint64_t req_id, bool granted) {
    char frame[transport::CACSS_RES_FRAME_SZ] = {};
    frame[0]  = static_cast<char>(transport::CACSS_WIRE_RES);
    std::memcpy(frame + 1, &req_id, 8);
    frame[9]  = static_cast<char>(granted ? transport::CACSS_VERDICT_GRANT : transport::CACSS_VERDICT_DENY);
    frame[10] = static_cast<char>(granted ? transport::CACSS_REASON_GRANTED : transport::CACSS_REASON_DENIED);
    out->push_outbound(frame, 11u);
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageCredAcssRspSystem::on_start(ecs::Registry& registry) {
    (void)registry;
    log::debug("[StorageCredAcssRsp] Started");
}

void StorageCredAcssRspSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto* out = registry.ctx().find<transport::OutboundQueueResourceManager>();
    if (out == nullptr) return;  // no outbound lane on this tier → nothing to emit

    // Deferred deletion: collect resolved requests, destroy after the pass (never during iteration).
    entt::entity done[CRED_ACSS_RSP_BATCH];
    uint32_t done_n = 0u;

    // GRANT verdicts (Tag-filtered View).
    for (auto [ent, req, pnd] :
         registry.view<StorageReqAcssComponent, StorageCredAcssPndComponent, StorageAcssGrantTag>().each()) {
        (void)req;
        if (done_n >= CRED_ACSS_RSP_BATCH) break;  // bound emits to what we can also destroy (no double-emit)
        emit_verdict(out, pnd.req_id, true);
        done[done_n++] = ent;
        log::info("[StorageCredAcssRsp] verdict req_id={} GRANT", pnd.req_id);
    }

    // DENY verdicts (Tag-filtered View). Concealment-safe: coarse reason on the wire, precise in audit.
    for (auto [ent, req, pnd] :
         registry.view<StorageReqAcssComponent, StorageCredAcssPndComponent, StorageAcssDenyTag>().each()) {
        (void)req;
        if (done_n >= CRED_ACSS_RSP_BATCH) break;
        emit_verdict(out, pnd.req_id, false);
        done[done_n++] = ent;
        log::info("[StorageCredAcssRsp] verdict req_id={} DENY", pnd.req_id);
    }

    for (uint32_t i = 0u; i < done_n; ++i) registry.destroy(done[i]);
}

void StorageCredAcssRspSystem::on_stop(ecs::Registry& registry) {
    (void)registry;
    log::debug("[StorageCredAcssRsp] Stopped");
}

}  // namespace ase::storage
