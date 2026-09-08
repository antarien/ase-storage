/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_edge_kycd_res_drn_sys.cpp
 * @brief       StorageEdgeKycdResDrnSystem - Parses Replica keycard documents into the gate session
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Reception
 * @created     2026-06-24
 * @modified    2026-06-24
 * @version     1.0.0
 *
 * CAUSAL CHAIN (EDGE_KYCD_RES_DRN: Replica keycard document → gate session)
 *
 *   [Replica ReplicaEdgeKycdSystem ships BIN_MSG_EDGE_KYCD_RES over Binary WS]
 *          │
 *          │ L2 KernelWbskDspcSystem routes id 34 → LANE_KYC
 *          ▼
 *   ┌───────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageEdgeKycdResDrnSystem (dist tier)         │
 *   │                                                               │
 *   │  POP:     drain LANE_KYC frames                               │
 *   │  PARSE:   STATUS_OK → scan the keycard document (user_id,     │
 *   │           clearance, permission, revoked, codewords[])        │
 *   │  PUBLISH: owner = hashed_string(user_id):                     │
 *   │           SES_CLEARANCE, SES_KYCD_PERM,                       │
 *   │           SES_KYCD_HOLDS_<cw> A/ACS hold verdicts (no string) │
 *   └───────────────────────────────────────────────────────────────┘
 *          │
 *          │ next install.sh poll/retry: the edge A/ACS gate reads the
 *          │ published session and authorizes the download
 *          ▼
 *   [ase-pl-edge-webserver acl_gate enforces clearance + codeword + permission]
 *
 * HUB Pattern (Active - publishes the gate session)
 *
 * READS (from Hub):
 *   (none)
 *
 * WRITES (to Hub):
 *   SES_CLEARANCE         - keycard clearance, owner = hashed_string(user_id)
 *   SES_KYCD_PERM         - keycard permission bitflags, same owner
 *   SES_KYCD_HOLDS_<cw>   - owner-scoped A/ACS hold verdict (1.0 iff the recovered
 *                           keycard holds the exact edge codeword; exact-string compare
 *                           server-internal, the codeword STRING never re-enters the Hub)
 *
 * FLYWEIGHT Pattern (inbound lane)
 *   The keycard document never enters a Component. It is popped from the L1
 *   transport InboundQueueResourceManager (LANE_KYC, via ctx) into a bounded
 *   stack buffer, scanned, and projected onto owner-keyed Hub values. The dist
 *   host links NO data client — it publishes only what the Replica returned.
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
#include <ase/storage/systems/keycard/storage_edge_kycd_res_drn_sys.hpp>
// Module constants (EDGE_KYCD_* frame layout, codeword key buffers)
#include <ase/storage/types.hpp>
// Bounded field extraction out of the JSON payload (module-internal, pure byte scans;
// a separate concern from draining the lane and publishing the session)
#include <ase/storage/storage_kycd_payload.hpp>
// Hub API for owner-keyed session publication
#include <ase/hub/api.hpp>
// Utils (L0 — safe C-string operations)
#include <ase/utils/strops.hpp>
// Transport inbound lane (L1 via ctx — the L2 demux fills LANE_KYC; this module
// NEVER touches the socket). L3→L1 erlaubt.
#include <ase/transport/inbound_queue_resource_manager.hpp>
#include <ase/transport/types.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>
#include <cstring>

#include <entt/core/hashed_string.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
// NO STRUCTS HERE! NO View/Query operations in helpers! Only pure byte/char math!
//
// EMPTY ON PURPOSE. The three payload scans that used to sit here - find_token,
// parse_num_field, parse_str_field - moved to storage_kycd_payload.hpp. They were never
// the concern of this file: this system drains a transport lane, validates a frame and
// publishes a session, and pulling fields out of a JSON blob is a different job. As long
// as both lived in one file that boundary existed but had no name.
namespace {

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageEdgeKycdResDrnSystem::on_start(ecs::Registry& registry) {
    log::debug("[StorageEdgeKycdResDrn] Started");

    // Register the edge keycard-authz response wire-type onto the shared inbound
    // lane so the L2 demux routes it here. Emplace the queue if the demux on_start
    // has not run yet (order-independent: whoever runs first creates it).
    if (registry.ctx().find<transport::InboundQueueResourceManager>() == nullptr) {
        registry.ctx().emplace<transport::InboundQueueResourceManager>();
    }
    auto& queue = registry.ctx().get<transport::InboundQueueResourceManager>();
    queue.register_route(EDGE_KYCD_BIN_MSG_RES, transport::LANE_KYC);
}

void StorageEdgeKycdResDrnSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto* queue = registry.ctx().find<transport::InboundQueueResourceManager>();
    if (queue == nullptr || !queue->has_inbound(transport::LANE_KYC)) return;

    char buf[transport::LANE_BUF_SZ] = {};
    uint32_t msg_len = 0;

    while (queue->pop_inbound(transport::LANE_KYC, buf, transport::LANE_BUF_SZ, msg_len)) {
        // DIE FRAME-PRUEFUNGEN TRAGEN INPUT_REJECTED, und dieser Block sagt warum — inklusive
        // der beiden Kandidaten, die hier NICHT passen.
        //
        // Massgeblich ist nicht die Semantik allein, sondern der Hilfetext, den log.hpp bei
        // JEDEM Aufruf mitgibt. Die drei aelteren WRN-Kategorien sagen eine KORREKTUR zu:
        //
        //     VALUE_OUT_OF_RANGE  "Fix: Value will be clamped to valid range"
        //     VALUE_NEGATIVE      "Fix: Value will be set to 0"
        //     VALUE_INVALID       "Fix: Value will be set to default"
        //
        // Jede dieser Pruefungen macht das Gegenteil: `msg_len = 0; continue;` — der Frame wird
        // VERWORFEN, nichts geklemmt, nichts auf einen Vorgabewert gesetzt. Alle drei schrieben
        // hier eine Unwahrheit in jede einzelne Logzeile.
        //
        //     INPUT_REJECTED       "Fix: Request rejected, caller must correct it -
        //                           not an engine fault"
        //
        // (Hier stand frueher eine Nummer hinter dem Namen. Sie ist entfernt und NICHT durch
        //  eine neue ersetzt: seit dem Hash-Umbau IST die Kategorie ihr Name, und der Bestand
        //  steht in core/ase-log/data/log_categories.json. Eine Zahl im Kommentar altert beim
        //  naechsten Generatorlauf still — kein Tor liest Prosa.)
        //
        // Der sagt genau das Richtige und verspricht nichts: der Aufrufer hat etwas Unbedienbares
        // geschickt, der Motor arbeitet korrekt. Die Dreier-Form ohne `owner` ist die dafuer
        // gebaute — der Ausloeser ist ein fremder Aufrufer, kein Hub-Owner.
        //
        // WAS DABEI VERLOREN GEHT, und es ist bewusst: die ZAHLEN. Der freie String trug
        // `msg_len` beziehungsweise das Typ-Byte; keine kategorisierte Ueberladung nimmt einen
        // freien Text oder eine Zahl ohne `owner`. Der value_id benennt deshalb, WELCHE Zusage
        // des Kontrakts gebrochen wurde, statt mit welchem Wert. Das ist der Tausch, den
        // kategorisiertes Logging hier macht: Struktur gegen Detail.
        if (msg_len < EDGE_KYCD_RES_HDR) {
            log::warn(log::WRN::CAT::INPUT_REJECTED, "StorageEdgeKycdResDrn",
                      "EDGE_KYCD_RES_frame_len");
            msg_len = 0;
            continue;
        }

        uint8_t msg_type = static_cast<uint8_t>(buf[0]);
        if (msg_type != EDGE_KYCD_BIN_MSG_RES) {
            log::warn(log::WRN::CAT::INPUT_REJECTED, "StorageEdgeKycdResDrn",
                      "EDGE_KYCD_RES_msg_type");
            msg_len = 0;
            continue;
        }

        uint64_t req_id = 0;
        std::memcpy(&req_id, buf + 1, 8);
        uint8_t status = static_cast<uint8_t>(buf[9]);
        uint32_t payload_len = 0;
        std::memcpy(&payload_len, buf + 10, 4);

        // A NOT_FOUND / ERROR response publishes NOTHING: the gate stays 401 so a
        // missing or backend-failed keycard never silently grants a download. Each
        // guard is a single comparison (bounds checks, not a value range test).
        if (status != EDGE_KYCD_STATUS_OK) {
            log::info("[StorageEdgeKycdResDrn] EDGE_KYCD_RES status={} req_id={} — no session published",
                      static_cast<uint32_t>(status), req_id);
            msg_len = 0;
            continue;
        }
        if (payload_len < 1u) {
            log::warn(log::WRN::CAT::INPUT_REJECTED, "StorageEdgeKycdResDrn",
                      "EDGE_KYCD_RES_payload_len");
            msg_len = 0;
            continue;
        }
        uint32_t payload_end = EDGE_KYCD_RES_HDR + payload_len;
        if (payload_end > msg_len) {
            log::warn(log::WRN::CAT::INPUT_REJECTED, "StorageEdgeKycdResDrn",
                      "EDGE_KYCD_RES_payload_end");
            msg_len = 0;
            continue;
        }
        if (payload_len > EDGE_KYCD_PAYLOAD_MAX) {
            payload_len = EDGE_KYCD_PAYLOAD_MAX;
        }

        const char* doc = buf + EDGE_KYCD_RES_HDR;

        // The Replica FIND returns a JSON array '[{...}]'; the keycard document is
        // the first (single) object. All field lookups scan the whole payload, so
        // the array brackets need no explicit unwrap — the keyed scan finds each
        // field wherever it sits.

        // Recover the recipient user_id — the owner the gate session is keyed on.
        // Without it there is no owner to publish against (no silent default).
        char user_id[MAX_OWNER_ID] = {};
        if (!parse_str_field(doc, payload_len, "\"user_id\"", user_id, sizeof(user_id))) {
            // DIESE DREI DOKUMENTFELDER BLEIBEN log::error MIT FREIEM STRING, UND HIER IST DER
            // GRUND EIN ANDERER ALS OBEN — er ist betrieblich, nicht formal.
            //
            // Fehlt user_id, clearance oder permission, bleibt das Tor auf 401 beziehungsweise
            // faellt auf den verweigernden Boden. Das ist ein SICHERHEITSEREIGNIS.
            //
            // BIS 2026-08-23 STAND HIER EIN FREIER STRING, und die Begruendung war richtig: die
            // einzige kategorisierte Form fuer diesen Fall war eine WARNUNG, und die haette die
            // Zeile aus dem ERR-Filter geworfen — Filterbarkeit gewonnen, Sichtbarkeit verloren
            // (Betreiber-Entscheid 2026-08-22).
            //
            // GEMESSEN am 2026-08-23: ACCESS_DENIED (ERR::CAT; die Nummer, die hier stand, ist
            // entfernt und nicht ersetzt — Bestand in core/ase-log/data/log_categories.json,
            // die Kategorie IST ihr Name) ist eine FEHLER-Kategorie, und
            // ihr Hilfetext sagt ausdruecklich "Decision belongs in the error stream, not as a
            // warning". Die PRAEMISSE des Entscheids — Kategorie hiesse Abstieg auf WRN —
            // trifft damit nicht mehr zu.
            //
            // WAS DARAUS FOLGT, IST EINE ABWAEGUNG UND KEINE MESSUNG, und sie steht hier als
            // solche: dass eine weggefallene Praemisse den Entscheid aufhebt, ist meine
            // Schlussfolgerung, kein nachgeschlagener Grundsatz — im Wissensbestand ist dazu
            // nichts zu finden. Die Umstellung wurde dem Auftraggeber am 2026-08-23 mit genau
            // dieser Kennzeichnung zum Widerruf vorgelegt. Wer sie zurueckdreht, dreht eine
            // Abwaegung zurueck und keinen Messfehler.
            //
            // WAS DER UMBAU KOSTET, damit es niemand suchen muss: req_id faellt aus der Zeile.
            // Es ist ein uint64_t, und die kategorisierten Formen fuehren uint32_t owner — eine
            // abgeschnittene 64-Bit-Kennung waere eine ANDERE Kennung und damit eine
            // Falschaussage in der strukturierten Form. Dieselbe Abwaegung wie bei den
            // Frame-Pruefungen oben.
            //
            // OHNE owner, und das ist hier kein Mangel: user_id ist GENAU DAS, was fehlt. Die
            // Zeile sagt damit wahrheitsgemaess, dass die Identitaet nicht aufgeloest werden
            // konnte — Punkt 1 des Hilfetexts.
            log::error(log::ERR::CAT::ACCESS_DENIED, "StorageEdgeKycdResDrn",
                       "EDGE_KYCD_RES_user_id");
            msg_len = 0;
            continue;
        }
        uint32_t owner = entt::hashed_string{user_id}.value();

        // A revoked keycard never grants: publish NOTHING so the gate stays 401.
        uint32_t revoked = 0;
        if (parse_num_field(doc, payload_len, "\"revoked\"", revoked) && revoked != 0u) {
            log::info("[StorageEdgeKycdResDrn] keycard revoked owner={} user='{}' — no session published",
                      owner, user_id);
            msg_len = 0;
            continue;
        }

        // Clearance + permission axes (gate steps 4 + 6). A missing axis is corrupt
        // state — log it and default to the denying floor so the gate refuses
        // rather than grants on partial data.
        uint32_t clearance = 0;
        if (!parse_num_field(doc, payload_len, "\"clearance\"", clearance)) {
            // MIT owner, anders als der user_id-Fall oben: ab Zeile 383 ist die Identitaet
            // aufgeloest, und sie ist genau die Achse, nach der die Kategorie fragt
            // ("Required clearance/permission recorded for that identity"). req_id faellt aus
            // demselben Grund wie oben weg (uint64_t gegen uint32_t owner).
            log::error(log::ERR::CAT::ACCESS_DENIED, "StorageEdgeKycdResDrn", owner,
                       "EDGE_KYCD_RES_clearance");
            clearance = 0;
        }
        uint32_t permission = 0;
        if (!parse_num_field(doc, payload_len, "\"permission\"", permission)) {
            log::error(log::ERR::CAT::ACCESS_DENIED, "StorageEdgeKycdResDrn", owner,
                       "EDGE_KYCD_RES_permission");
            permission = 0;
        }

        hub::set(registry, owner, "SES_CLEARANCE"_hs, static_cast<float>(clearance));
        hub::set(registry, owner, "SES_KYCD_PERM"_hs, static_cast<float>(permission));

        // Reset the owner-scoped edge A/ACS hold-verdicts before the pass so a revoked keycard
        // re-resolution cannot leave a stale grant (the gate reads only these booleans).
        hub::set(registry, owner, "SES_KYCD_HOLDS_BINARY"_hs, 0.0f);
        hub::set(registry, owner, "SES_KYCD_HOLDS_SIG"_hs, 0.0f);
        hub::set(registry, owner, "SES_KYCD_HOLDS_SBOM"_hs, 0.0f);
        hub::set(registry, owner, "SES_KYCD_HOLDS_METADATA"_hs, 0.0f);

        // Codeword set (gate step 5): the '"codewords":[...]' array. Walk each quoted element
        // after the array open-bracket and compare it EXACTLY (server-internal) against the fixed
        // edge codewords, setting the matching owner-scoped hold-verdict boolean.
        uint32_t count = 0;
        int32_t arr_at = find_token(doc, payload_len, "\"codewords\"");
        if (arr_at >= 0) {
            uint32_t i = static_cast<uint32_t>(arr_at) +
                         ase::utils::str_len("\"codewords\"", EDGE_KYCD_PAYLOAD_MAX);
            // advance to the array open-bracket
            while (i < payload_len && doc[i] != '[' && doc[i] != ']') ++i;
            if (i < payload_len && doc[i] == '[') {
                ++i;
                while (i < payload_len && doc[i] != ']' && count < KYCD_DECODE_CWRD_MAX) {
                    if (doc[i] == '"') {
                        ++i;  // opening quote
                        char cw[MAX_CODEWORD_LEN] = {};
                        uint32_t o = 0;
                        while (i < payload_len && doc[i] != '"' && o + 1u < sizeof(cw)) {
                            cw[o++] = doc[i++];
                        }
                        cw[o] = '\0';
                        if (i < payload_len && doc[i] == '"') ++i;  // closing quote
                        if (o > 0u) {
                            // Hashed at the point the codeword is parsed out of the payload:
                            // that is where the string comes into being, so it is also where
                            // its identity is fixed. The codeword itself never re-enters the
                            // Hub; only the fixed hold-verdict booleans do.
                            const uint32_t cwrd_hash = entt::hashed_string::value(cw, o);

                            // Compare the recovered codeword against the fixed
                            // edge-distribution codewords and, on a match, set the matching
                            // owner-scoped A/ACS hold-verdict boolean. The codeword itself
                            // never re-enters the Hub (server-internal, A/ACS step 5): only
                            // these fixed, contract-registered booleans reach the L4 edge gate.
                            //
                            // The comparison is on HASHES, against the compile-time constants
                            // in types.hpp. Identity is a lookup, and a lookup compares hashes,
                            // never characters (WRFL_ASE_STRING_HANDLING Section 3). Nothing is
                            // re-derived: the hash is the one computed one line above.
                            //
                            // FOUR INDEPENDENT ifs, not an else-chain: a codeword set is a SET,
                            // the four axes are orthogonal grants, and an else would make them
                            // mutually exclusive - a different access policy than this gate has.
                            if (cwrd_hash == EDGE_CWRD_BINARY_HASH) {
                                hub::set(registry, owner, "SES_KYCD_HOLDS_BINARY"_hs, 1.0f);
                            }
                            if (cwrd_hash == EDGE_CWRD_SIG_HASH) {
                                hub::set(registry, owner, "SES_KYCD_HOLDS_SIG"_hs, 1.0f);
                            }
                            if (cwrd_hash == EDGE_CWRD_SBOM_HASH) {
                                hub::set(registry, owner, "SES_KYCD_HOLDS_SBOM"_hs, 1.0f);
                            }
                            if (cwrd_hash == EDGE_CWRD_METADATA_HASH) {
                                hub::set(registry, owner, "SES_KYCD_HOLDS_METADATA"_hs, 1.0f);
                            }
                            ++count;
                        }
                    } else {
                        ++i;  // commas / whitespace between elements
                    }
                }
            }
        }

        log::info("[StorageEdgeKycdResDrn] session published owner={} user='{}' clrn={} perm={} cwrds={} (req_id={})",
                  owner, user_id, clearance, permission, count, req_id);

        msg_len = 0;
    }
}

void StorageEdgeKycdResDrnSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageEdgeKycdResDrn] Stopped");
}

}  // namespace ase::storage
