/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_cred_acss_rcv_sys.cpp
 * @brief       StorageCredAcssRcvSystem - drives the A/ACS ladder for a Replica credential request
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Reception
 * @created     2026-07-04
 * @modified    2026-08-19
 * @version     1.1.0
 *
 * CAUSAL CHAIN (Credential A/ACS Request Drain)
 *
 *   [Replica replica_cred_push emits CACSS_WIRE_REQ before a customer-credential Vault op]
 *          │
 *          │ L2 KernelWbskDspcSystem routes type 86 → LANE_CACSS
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageCredAcssRcvSystem      │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - LANE_CACSS frames (req_id/user_hash/   │
 *   │      action/project_id/provider)            │
 *   │    - StorageStaKycdComponent (by user_hash) │
 *   │    - StorageStaRelmComponent (by project_id)│
 *   │                                             │
 *   │  WRITES (Components):                        │
 *   │    - StorageReqAcssComponent (for ladder)   │
 *   │    - StorageCredAcssPndComponent (req_id)   │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Integration StorageAcssChkSystem renders Grant/DenyTag
 *          ▼
 *   StorageCredAcssRspSystem ships CACSS_WIRE_RES back to the Replica.
 *
 * INDIZES JE PASS (2026-08-19): die Keycard- und Realm-Aufloesung laeuft ueber zwei
 * O(1)-Indizes, die EINMAL vor der Frame-Schleife gebaut werden - vorher scannte
 * jede Frame-Iteration beide Views voll (M Frames x N Zeilen; der Batch ist die
 * aeussere Menge, weshalb kein nested-view-Check es sah). Der Realm-Index wird beim
 * Anlegen eines Realms NACHGEFUEHRT, damit ein zweiter Frame desselben Kunden im
 * SELBEN Batch die Zeile wiederverwendet statt sie zu duplizieren.
 *
 * HUB Pattern (N/A — transport lane + components, no Hub values)
 *
 * READS (from Hub):
 *   (none)
 *
 * WRITES (to Hub):
 *   (none)
 *
 * FLYWEIGHT PATTERN (Active — transport::InboundQueueResourceManager via ctx)
 *   The L1 inbound lane queue is a ctx flyweight the L2 demux fills; this system drains LANE_CACSS.
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
// Own header FIRST
#include <ase/storage/systems/acl/storage_cred_acss_rcv_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_req_acss_comp.hpp>
#include <ase/storage/components/state/storage_req_cred_comp.hpp>
#include <ase/storage/components/state/storage_cred_acss_pnd_comp.hpp>
#include <ase/storage/components/state/storage_sta_kycd_comp.hpp>
#include <ase/storage/components/state/storage_kycd_grnt_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/state/storage_relm_quot_comp.hpp>
#include <ase/storage/components/state/storage_relm_idn_comp.hpp>
#include <ase/storage/components/tag/storage_relm_usr_tag.hpp>
#include <ase/storage/components/tag/storage_relm_actv_tag.hpp>
#include <ase/storage/components/tag/storage_relm_cncm_tag.hpp>
#include <ase/storage/types.hpp>
// Lower layers
#include <ase/transport/inbound_queue_resource_manager.hpp>
#include <ase/transport/types.hpp>
#include <ase/containers/hash_map.hpp>
#include <ase/containers/int_hash.hpp>
#include <ase/utils/strops.hpp>
#include <ase/log/log.hpp>

#include <cstdint>
#include <cstring>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (pure byte/string math, no View/Query).
namespace {

// Append the decimal encoding of n to out (bounded, NUL-terminated). Pure string math.
void append_dec(char* out, uint32_t cap, uint32_t n) {
    char tmp[12] = {};
    uint32_t t = 0u;
    if (n == 0u) {
        tmp[t++] = '0';
    } else {
        uint32_t v = n;
        while (v > 0u && t < 11u) { tmp[t++] = static_cast<char>('0' + (v % 10u)); v /= 10u; }
    }
    char num[12] = {};
    uint32_t o = 0u;
    while (t > 0u) num[o++] = tmp[--t];
    num[o] = '\0';
    ase::utils::str_append(out, cap, num);
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageCredAcssRcvSystem::on_start(ecs::Registry& registry) {
    log::debug("[StorageCredAcssRcv] Started");
    if (registry.ctx().find<transport::InboundQueueResourceManager>() == nullptr) {
        registry.ctx().emplace<transport::InboundQueueResourceManager>();
    }
    auto& queue = registry.ctx().get<transport::InboundQueueResourceManager>();
    queue.register_route(transport::CACSS_WIRE_REQ, transport::LANE_CACSS);
}

void StorageCredAcssRcvSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto* queue = registry.ctx().find<transport::InboundQueueResourceManager>();
    if (queue == nullptr || !queue->has_inbound(transport::LANE_CACSS)) return;

    char buf[transport::LANE_BUF_SZ] = {};
    uint32_t msg_len = 0u;

    // O(1)-Indizes, EINMAL je Pass gebaut - jede Frame-Iteration fragt "gibt es eine
    // Zeile fuer diesen Schluessel" gegen den Index statt die Views voll zu scannen
    // (der dekodierte Batch ist die aeussere Menge, M Frames x N Zeilen). Der
    // Realm-Index wird beim Anlegen NACHGEFUEHRT, damit ein zweiter Frame desselben
    // Kunden im SELBEN Batch die neue Zeile wiederverwendet statt sie zu duplizieren.
    ase::containers::HashMap<uint32_t, ecs::Entity, ase::containers::IntMixHash> kycd_of_user;
    for (auto [ke, kc] : registry.view<StorageStaKycdComponent>().each()) {
        const uint32_t issued_hash = entt::hashed_string(kc.issued_to).value();
        if (kycd_of_user.find(issued_hash) == kycd_of_user.end()) {
            kycd_of_user[issued_hash] = ke;
        }
    }
    ase::containers::HashMap<uint32_t, ecs::Entity, ase::containers::IntMixHash> relm_of_hash;
    for (auto [re, rc_idn] : registry.view<StorageRelmIdnComponent>().each()) {
        if (relm_of_hash.find(rc_idn.id_hash) == relm_of_hash.end()) {
            relm_of_hash[rc_idn.id_hash] = re;
        }
    }

    // Frame: [86][req_id:u64][user_hash:u32][action:u8][project_id:char[64]][provider:char[64]] = 142 bytes.
    while (queue->pop_inbound(transport::LANE_CACSS, buf, transport::LANE_BUF_SZ, msg_len)) {
        if (msg_len < 142u) {
            // INPUT_REJECTED (WRN::CAT, SSOT core/ase-log/data/log_categories.json — die
            // Kategorie IST ihr Name, eine Nummer gibt es seit dem Hash-Umbau nicht mehr):
            // "Fix: Request rejected, caller must correct it — not an
            // engine fault". Die Stelle verwirft (msg_len = 0; continue), und genau das sagt
            // der Hilfetext: korrigieren muss der AUFRUFER. NICHT VALUE_INVALID oder
            // OUT_OF_RANGE — die beschreiben einen Wert, der im System ungueltig ist oder
            // seinen Bereich verlaesst; hier hat eine fremde Gegenstelle einen zu kurzen
            // Rahmen geschickt, und die HERKUNFT ist der Unterschied, nicht die Reaktion.
            // Die Dreier-Form ohne owner ist die dafuer gebaute; die Bytezahl faellt weg, der
            // value_id benennt stattdessen die gebrochene Kontraktzusage.
            log::warn(log::WRN::CAT::INPUT_REJECTED, "StorageCredAcssRcvSystem",
                      "CACSS_WIRE_REQ_frame_len");
            msg_len = 0u;
            continue;
        }
        if (static_cast<uint8_t>(buf[0]) != transport::CACSS_WIRE_REQ) {
            log::warn(log::WRN::CAT::INPUT_REJECTED, "StorageCredAcssRcvSystem",
                      "CACSS_WIRE_REQ_msg_type");
            msg_len = 0u;
            continue;
        }
        uint64_t req_id = 0u;
        std::memcpy(&req_id, buf + 1, 8);
        uint32_t user_hash = 0u;
        std::memcpy(&user_hash, buf + 9, 4);
        uint8_t action = static_cast<uint8_t>(buf[13]);
        const char* project_id = buf + 14;   // NUL-padded char[64]
        const char* provider = buf + 78;     // NUL-padded char[64]

        // Resolve the session keycard by user_hash (== hashed_string(issued_to)) → user_id + clrn + perm
        // + the realm it grants (A/ACS: Keycard → Realm-Membership). No keycard → empty user_id so the
        // ladder denies at step 1 (fail-closed, no silent grant). One O(1) lookup, no walk.
        char user_id[MAX_OWNER_ID] = {};
        uint8_t clrn = 0u;
        uint16_t perm = 0u;
        uint32_t kc_relm = 0u;
        uint32_t kc_proj = 0u;
        auto kc_it = kycd_of_user.find(user_hash);
        if (kc_it != kycd_of_user.end() && registry.valid(kc_it->second)) {
            const auto& kc = registry.get<StorageStaKycdComponent>(kc_it->second);
            ase::utils::str_copy(user_id, MAX_OWNER_ID, kc.issued_to);
            kc_relm = kc.relm_ref;
            kc_proj = kc.proj_ref;
            // The terms are the second row of the same keycard. A card without them
            // stays at clearance 0 / no permissions - the ladder then denies, which
            // is the same fail-closed answer as no keycard at all.
            const auto* kg = registry.try_get<StorageKycdGrntComponent>(kc_it->second);
            if (kg != nullptr) {
                clrn = kg->clrn;
                perm = kg->perm;
            } else {
                log::error(log::ERR::CAT::COMPONENT_MISSING, "StorageCredAcssRcvSystem",
                           static_cast<uint32_t>(kc_it->second), "StorageKycdGrntComponent");
            }
        }

        // The A/ACS realm is the one the keycard grants membership to — NOT a per-project realm. An
        // operator-mint keycard carries an explicit relm_ref; the normal customer auth-gate keycard
        // carries 0, so bind to the customer's OWN realm: one realm per customer, id = owner = the
        // account user_id. This IS the anti-tenant isolation (identity binding in the ONE unified store,
        // org = user_hash), never a per-project/per-org silo. The ladder's owner-preset (rc.owner ==
        // req.user_id) grants the owner; the ConcealTag denies non-owners realm_not_found (no leak).
        // project_id/provider stay in the ACL path as the resource label. Resolve-or-create idempotently.
        uint32_t relm_ref = kc_relm;
        if (relm_ref == 0u && user_id[0] != '\0') {
            // A personal realm is named after its owner, so its id hash IS user_hash -
            // the value that arrived on the wire. One O(1) lookup against the pass index.
            auto rl_it = relm_of_hash.find(user_hash);
            if (rl_it != relm_of_hash.end() && registry.valid(rl_it->second)) {
                relm_ref = static_cast<uint32_t>(rl_it->second);
            }
            if (relm_ref == 0u) {
                auto realm_ent = registry.create();
                auto& relm = registry.emplace<StorageStaRelmComponent>(realm_ent);
                ase::utils::str_copy(relm.id, MAX_REALM_ID, user_id);
                ase::utils::str_copy(relm.name, MAX_REALM_NAME, user_id);
                ase::utils::str_copy(relm.owner, MAX_OWNER_ID, user_id);
                relm.default_protection = PROTECTION_PROTECTED;
                // The accounting row is created empty: a personal realm gets no
                // ceiling here, and StorageQuotChkSystem skips a zero ceiling. The
                // row still has to exist, or the realm would be invisible to the
                // scan the day a ceiling IS set on it.
                registry.emplace<StorageRelmQuotComponent>(realm_ent);
                // A personal realm is named after its owner, so both hashes are the
                // same value - written here, beside the strings, never re-derived later.
                auto& relm_idn = registry.emplace<StorageRelmIdnComponent>(realm_ent);
                relm_idn.id_hash = user_hash;
                relm_idn.owner_hash = user_hash;
                registry.emplace<StorageRelmUsrTag>(realm_ent);
                registry.emplace<StorageRelmActvTag>(realm_ent);
                registry.emplace<StorageRelmCncmTag>(realm_ent);
                relm_ref = static_cast<uint32_t>(realm_ent);
                relm_of_hash[user_hash] = realm_ent;  // nachgefuehrt - same-batch frames reuse it
                log::info("[StorageCredAcssRcv] customer realm created (identity-bound owner='{}')", user_id);
            }
        }

        // Build the credential asset path (ACL-rule prefix match); org segment = decimal(user_hash).
        char path[MAX_PATH_LEN] = {};
        ase::utils::str_copy(path, MAX_PATH_LEN, "customers/");
        append_dec(path, MAX_PATH_LEN, user_hash);
        ase::utils::str_append(path, MAX_PATH_LEN, "/projects/");
        ase::utils::str_append(path, MAX_PATH_LEN, project_id);
        ase::utils::str_append(path, MAX_PATH_LEN, "/credentials/");
        ase::utils::str_append(path, MAX_PATH_LEN, provider);

        // Emplace the access-check request for the single ACL authority + the wire correlation.
        auto req_ent = registry.create();
        auto& req = registry.emplace<StorageReqAcssComponent>(req_ent);
        req.relm_ref = relm_ref;
        req.proj_ref = kc_proj;  // keycard project scope (0 = realm-wide access)
        ase::utils::str_copy(req.path, 256u, path);
        req.action = action;   // wire 0/1/2 == AUD_READ/WRITE/DELETE
        auto& cred = registry.emplace<StorageReqCredComponent>(req_ent);
        cred.clrn = clrn;
        cred.perm = perm;
        ase::utils::str_copy(cred.user_id, 64u, user_id);

        auto& pnd = registry.emplace<StorageCredAcssPndComponent>(req_ent);
        pnd.req_id = req_id;
        pnd.conn_id = 0u;

        log::info("[StorageCredAcssRcv] access-check queued req_id={} project='{}' provider='{}' relm_ref={} user='{}'",
                  req_id, project_id, provider, relm_ref, user_id);
        msg_len = 0u;
    }
}

void StorageCredAcssRcvSystem::on_stop(ecs::Registry& registry) {
    (void)registry;
    log::debug("[StorageCredAcssRcv] Stopped");
}

}  // namespace ase::storage
