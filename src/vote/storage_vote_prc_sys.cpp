/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_vote_prc_sys.cpp
 * @brief       StorageVotePrcSystem - Evaluates Vote of Confidence outcomes
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Observation
 * @created     2026-04-05
 * @modified    2026-06-24
 * @version     1.0.0
 *
 * CAUSAL CHAIN (Vote Evaluation)
 *
 *   [HTTP routes create vote entities with ballots]
 *          │
 *          │ Observation schedule evaluates at 1Hz
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageVotePrcSystem          │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageVotePendTag (open votes)        │
 *   │    - StorageStaVoteComponent (vote config)  │
 *   │    - StorageBlltVoteComponent (ballots)     │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - Removes StorageVotePendTag on resolve  │
 *   │    - Issues keycard if vote accepted        │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Vote resolved: accepted or rejected
 *          ▼
 *   Keycard issued or vote entity marked complete
 *
 * HUB Pattern (N/A - No Hub reads/writes)
 *
 * READS (from Hub):
 *   (none)
 *
 * WRITES (to Hub):
 *   (none)
 *
 * FLYWEIGHT PATTERN (Active - StorageResourceManager via ctx)
 *   Vote evaluation checks ballot counts against quorum thresholds.
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
#include <ase/storage/systems/vote/storage_vote_prc_sys.hpp>
// Components + tags from same module
#include <ase/storage/components/state/storage_sta_vote_comp.hpp>
#include <ase/storage/components/state/storage_vote_cnt_comp.hpp>
#include <ase/storage/components/state/storage_bllt_vote_comp.hpp>
#include <ase/storage/components/request/storage_req_kycd_comp.hpp>
#include <ase/storage/components/request/storage_req_kycd_relm_comp.hpp>
#include <ase/storage/components/tag/storage_tag_vot_pend.hpp>
#include <ase/storage/storage_resource_manager.hpp>
#include <ase/storage/types.hpp>
// Hub API (the keycard drain filters on hub::HubStgKycdPendTag)
#include <ase/hub/api.hpp>
// Utils (L0 — bounded string copy)
#include <ase/utils/strops.hpp>

#include <entt/core/hashed_string.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
namespace {

// No helper functions needed → every step is a flat View pass in tick()

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageVotePrcSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageVotePrc] Started");
}

void StorageVotePrcSystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    auto* mgr_pp = registry.ctx().find<StorageResourceManager*>();
    if (mgr_pp == nullptr || *mgr_pp == nullptr) return;  // no storage tier here
    const uint64_t now = (*mgr_pp)->get_wall_time_seconds();
    if (!now) {
        log::warn("[StorageVotePrc] wall clock unavailable - deadlines cannot be evaluated this tick");
        return;
    }

    // ── PASS 1: clear the tallies of every OPEN vote ────────────────────────
    // The counts are DERIVED from the ballots on every tick, never accumulated.
    // An accumulator can only ever count up, so a withdrawn or corrected ballot
    // could never lower a tally - and a motion would pass on ballots that no
    // longer exist.
    for (auto [vote_ent, cnt] :
         registry.view<StorageVoteCntComponent, StorageVotPendTag>().each()) {
        (void)vote_ent;
        cnt.votes_for = 0;
        cnt.votes_against = 0;
        cnt.votes_abstain = 0;
    }

    // ── PASS 2: walk the BALLOTS once, reach the parent vote by entity ref ───
    // The other direction - for each vote, scan every ballot - is the M:N shape
    // WS-K.2c forbids: it costs votes x ballots and rescans the whole ballot set
    // once per open motion. A ballot names its vote, so the join runs the cheap
    // way round: one pass over the children, O(1) into the parent.
    // The decision is counted as ARITHMETIC on the ballot's data, not as a
    // dispatch on it - no branch per category, so the shape of this loop does
    // not change when a ballot kind is added.
    auto cnt_view = registry.view<StorageVoteCntComponent, StorageVotPendTag>();
    for (auto [bllt_ent, bllt] : registry.view<StorageBlltVoteComponent>().each()) {
        (void)bllt_ent;
        const auto vote_ent = static_cast<ecs::Entity>(bllt.vote_ref);
        if (!cnt_view.contains(vote_ent)) continue;  // vote concluded, unknown, or not open
        auto& cnt = cnt_view.get<StorageVoteCntComponent>(vote_ent);
        cnt.votes_for     = static_cast<uint8_t>(cnt.votes_for     + (bllt.decision == BALLOT_FOR     ? 1u : 0u));
        cnt.votes_against = static_cast<uint8_t>(cnt.votes_against + (bllt.decision == BALLOT_AGAINST ? 1u : 0u));
        cnt.votes_abstain = static_cast<uint8_t>(cnt.votes_abstain + (bllt.decision == BALLOT_ABSTAIN ? 1u : 0u));
    }

    // ── PASS 3: conclude what can be concluded ──────────────────────────────
    // Deferred: nothing is created or retagged while the filtered View is being
    // walked. StorageVotPendTag is the very filter of this View, so removing it
    // inside the loop would mutate the range under the iterator.
    ecs::Entity done[VOTE_PRC_BATCH];
    ecs::Entity accepted[VOTE_PRC_BATCH];
    uint32_t done_n = 0;
    uint32_t accepted_n = 0;

    for (auto [vote_ent, vote, cnt] :
         registry.view<StorageStaVoteComponent, StorageVoteCntComponent, StorageVotPendTag>().each()) {
        if (done_n >= VOTE_PRC_BATCH) break;

        if (!cnt.votes_required) {
            // A motion nobody has to answer can never be decided, and it would
            // otherwise sit open forever, costing a ballot scan every tick.
            log::warn("[StorageVotePrc] vote on {} has votes_required=0 - undecidable, concluding as lapsed",
                      vote.subject);
            done[done_n] = vote_ent;
            ++done_n;
            continue;
        }

        const uint32_t cast = static_cast<uint32_t>(cnt.votes_for) +
                              static_cast<uint32_t>(cnt.votes_against) +
                              static_cast<uint32_t>(cnt.votes_abstain);
        const bool quorum = cast >= static_cast<uint32_t>(cnt.votes_required);
        const bool lapsed = cnt.deadline && now > cnt.deadline;
        if (!quorum && !lapsed) continue;  // still collecting

        // Abstentions carry the motion to quorum but never decide it: the
        // outcome is a plain majority of the ballots that took a side.
        const bool passed = quorum && cnt.votes_for > cnt.votes_against;

        done[done_n] = vote_ent;
        ++done_n;
        if (passed) {
            accepted[accepted_n] = vote_ent;
            ++accepted_n;
        }
        log::info("[StorageVotePrc] vote on {} concluded: for={} against={} abstain={} required={} -> {}",
                  vote.subject, static_cast<uint32_t>(cnt.votes_for),
                  static_cast<uint32_t>(cnt.votes_against), static_cast<uint32_t>(cnt.votes_abstain),
                  static_cast<uint32_t>(cnt.votes_required), passed ? "PASSED" : "REJECTED");
    }

    // Issue the keycard for every passed motion. The mint is REQUESTED through
    // components, never by calling the keycard system: a fresh entity carrying
    // StorageReqKycd + StorageReqKycdRelm + hub::HubStgKycdPendTag is exactly
    // what StorageKycdReqDrnSystem already drains. What the keycard carries is
    // the vote's own data, so a realm voting in a reader and one voting in a
    // maintainer run the same code with different numbers.
    for (uint32_t i = 0; i < accepted_n; ++i) {
        const auto* vote = registry.try_get<StorageStaVoteComponent>(accepted[i]);
        if (vote == nullptr) continue;

        const auto req_ent = registry.create();
        auto& req = registry.emplace<StorageReqKycdComponent>(req_ent);
        ase::utils::str_copy(req.user_id, MAX_OWNER_ID, vote->subject);
        req.user_id_hash = entt::hashed_string(vote->subject).value();
        req.authenticated_at = now;
        req.expires_at = now + VOTE_KYCD_TTL_S;
        req.clearance = vote->granted_clearance;

        auto& relm = registry.emplace<StorageReqKycdRelmComponent>(req_ent);
        relm.relm_ref = vote->relm_ref;
        relm.perm = vote->granted_perm;

        registry.emplace<hub::HubStgKycdPendTag>(req_ent);
        log::info("[StorageVotePrc] keycard requested for {} in realm {} (clearance {}, perm {})",
                  vote->subject, vote->relm_ref, static_cast<uint32_t>(vote->granted_clearance),
                  static_cast<uint32_t>(vote->granted_perm));
    }

    for (uint32_t i = 0; i < done_n; ++i) {
        registry.remove<StorageVotPendTag>(done[i]);
    }
}

void StorageVotePrcSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageVotePrc] Stopped");
}

}  // namespace ase::storage
