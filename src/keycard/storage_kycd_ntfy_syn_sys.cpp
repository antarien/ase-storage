/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_kycd_ntfy_syn_sys.cpp
 * @brief       StorageKycdNtfySynSystem - mirrors the star lines of one keycard request
 * @description Hub READ only. Sechs Zeilen hinein, zwei Bruecken-Components hinaus, keine Rechnung.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
 * @created     2026-09-21
 * @modified    2026-09-21
 * @version     00.00.00.00000 [seed]
 *
 * CAUSAL CHAIN (CAUSA_STG_KYCD_NTFY_SYN: the star is read here, the numbers are built there)
 *
 *   plugins/ase-pl-webserver auth_routes / sdk::emplace_keycard_request
 *          │
 *          │ hub::HubStgKycdPendTag + sechs SES_KYCD_NTF_*-Zeilen unter der Anfrage
 *          ▼
 *   ┌─────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageKycdNtfySynSystem                      │
 *   │                                                             │
 *   │  READS:                                                     │
 *   │    - hub::HubStgKycdPendTag (die Anfragen)                  │
 *   │    - Hub: SES_KYCD_NTF_USER_ID_HI/_LO, _REALM_ID_HI/_LO,    │
 *   │           _EXP_AT, _CLRN (owner = die Anfrage)              │
 *   │                                                             │
 *   │  WRITES:                                                    │
 *   │    - StorageInpKycdNtfyIdnComponent  (vier Haelften, roh)   │
 *   │    - StorageInpKycdNtfyGrntComponent (Stufe, Ablauf)        │
 *   └─────────────────────────────────────────────────────────────┘
 *          │
 *          │ StorageKycdNtfyDrnSystem setzt die Hashes zusammen
 *          ▼
 *   StorageReqKycdComponent, und von dort die bestehende Keycard-Kette
 *
 * WARUM DIESES SYSTEM EXISTIERT — GEMESSEN 2026-09-21. Der Validator meldete an
 * storage_kycd_ntfy_drn_sys.cpp HUB_IO_MIXED_WITH_MATH: sechs `hub::get` und die Rekonstruktion
 * der beiden uint32-Hashes aus je zwei 16-Bit-Haelften standen in EINEM tick(). Die `suggestion`
 * der Regel ist woertlich der Bauplan dieses Systems — eine SERVER-ONLY Seite liest den Stern in
 * eine `*_inp_*`-Zeile, die rechnende Seite nimmt sie von dort.
 *
 * DIE MELDUNG WANDERT MIT DEM LESEN. Die vier `log::error(HUB_NOT_FOUND)` gehoeren zu dem, der
 * den Stern befragt, und stehen deshalb hier. Fehlt eine Pflichtzeile, entsteht KEINE Bruecke;
 * der Drain filtert auf ihre Anwesenheit und sieht die Anfrage gar nicht erst. Das ist dasselbe
 * Verhalten wie das fruehere `continue` im Drain, nur an der Stelle, die es verantwortet.
 *
 * WARUM DIE HAELFTEN ROH BLEIBEN. Ein uint32-Hash passt nicht in die 24-Bit-Mantisse eines float;
 * die SDK-Seite zerlegt ihn in zwei exakte 16-Bit-Haelften. Sie hier zusammenzusetzen hiesse, die
 * Rechnung zurueck an die Hub-Seite zu holen - also genau den Schnitt aufzuheben, den dieses
 * System herstellt.
 *
 * DIE PRUEFUNG STEHT NEBEN IHREM GRIFF, AUCH BEI DEN DREI FREIWILLIGEN ZEILEN. Ein Sentinel, der
 * erst mehrere Zeilen spaeter abgefangen wird, ist bis dahin eine Zahl wie jede andere - und das
 * Schreibtor meldet genau diese Distanz.
 *
 * HUB Pattern (MIG_ASE_HUB_API v2.0):
 *
 * READS (from the star, owner = the request entity):
 *   "SES_KYCD_NTF_USER_ID_HI"_hs   ← obere 16 Bit des Benutzer-Hashes
 *   "SES_KYCD_NTF_USER_ID_LO"_hs   ← untere 16 Bit des Benutzer-Hashes
 *   "SES_KYCD_NTF_EXP_AT"_hs       ← Ablauf
 *   "SES_KYCD_NTF_CLRN"_hs         ← Freigabestufe, Abwesenheit = 0
 *   "SES_KYCD_NTF_REALM_ID_HI"_hs  ← obere 16 Bit des Realm-Hashes, Abwesenheit = 0
 *   "SES_KYCD_NTF_REALM_ID_LO"_hs  ← untere 16 Bit des Realm-Hashes, Abwesenheit = 0
 *
 * WRITES (to the star):
 *   (none)
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
#include <ase/storage/systems/keycard/storage_kycd_ntfy_syn_sys.hpp>
// Components from same module
#include <ase/storage/components/request/storage_req_kycd_comp.hpp>
#include <ase/storage/components/state/storage_inp_kycd_ntfy_idn_comp.hpp>
#include <ase/storage/components/state/storage_inp_kycd_ntfy_grnt_comp.hpp>
// Abwesenheit ist eine Frage an die SSOT, kein Vergleich: types::is_not_found statt == NOT_FOUND.
#include <ase/types/types.hpp>
// Hub: api.hpp ist der EINZIGE erlaubte Hub-Header und traegt auch die Entdeckungsmarke mit.
#include <ase/hub/api.hpp>
// Logging
#include <ase/log/log.hpp>

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

void StorageKycdNtfySynSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdNtfySynSystem] Started");
}

void StorageKycdNtfySynSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * DIESELBE FILTERMENGE WIE DER DRAIN, UM EINE BEDINGUNG ERWEITERT.
     *
     * Anfragen mit der Entdeckungsmarke, die noch keine Nutzlast haben - und noch keine Bruecke.
     * Der Ausschluss der Bruecke macht den Durchlauf idempotent: eine Anfrage wird genau einmal
     * gespiegelt, und ein zweiter Takt vor dem Drain schreibt nichts neu.
     */
    auto view = registry.view<hub::HubStgKycdPendTag>(
        entt::exclude<StorageReqKycdComponent, StorageInpKycdNtfyIdnComponent>);
    for (auto req_entity : view) {
        const uint32_t owner = static_cast<uint32_t>(req_entity);

        /**
         * VIER PFLICHTZEILEN, JEDE EINZELN UND SOFORT GEPRUEFT.
         *
         * HUB_NOT_FOUND ist hier die exakte Kategorie und die Ebene bleibt error: der Wert FEHLT,
         * er ist nicht ungueltig. Fehlt eine, entsteht KEINE Bruecke - der Drain sieht die
         * Anfrage dann gar nicht, was dem frueheren `continue` entspricht.
         */
        const float user_hash_hi = hub::get(registry, owner, "SES_KYCD_NTF_USER_ID_HI"_hs);
        if (ase::types::is_not_found(user_hash_hi)) {
            log::error(log::ERR::CAT::HUB_NOT_FOUND, "StorageKycdNtfySynSystem", owner,
                       "SES_KYCD_NTF_USER_ID_HI");
            continue;
        }
        const float user_hash_lo = hub::get(registry, owner, "SES_KYCD_NTF_USER_ID_LO"_hs);
        if (ase::types::is_not_found(user_hash_lo)) {
            log::error(log::ERR::CAT::HUB_NOT_FOUND, "StorageKycdNtfySynSystem", owner,
                       "SES_KYCD_NTF_USER_ID_LO");
            continue;
        }
        const float expires_at = hub::get(registry, owner, "SES_KYCD_NTF_EXP_AT"_hs);
        if (ase::types::is_not_found(expires_at)) {
            log::error(log::ERR::CAT::HUB_NOT_FOUND, "StorageKycdNtfySynSystem", owner,
                       "SES_KYCD_NTF_EXP_AT");
            continue;
        }

        /**
         * DREI ZEILEN DUERFEN SCHWEIGEN, und ihr Schweigen ist eine Aussage, keine Stoerung:
         * der Altpfad des Auth-Gates schickt weder Freigabestufe noch Realm mit. Abwesenheit
         * faellt deshalb auf null - so stand es vor dem Schnitt und so steht es jetzt. Die
         * Pruefung steht direkt am Griff, damit der Sentinel keine Zeile weit als Zahl reist.
         */
        float clearance = hub::get(registry, owner, "SES_KYCD_NTF_CLRN"_hs);
        if (ase::types::is_not_found(clearance)) clearance = 0.0f;
        float realm_hash_hi = hub::get(registry, owner, "SES_KYCD_NTF_REALM_ID_HI"_hs);
        if (ase::types::is_not_found(realm_hash_hi)) realm_hash_hi = 0.0f;
        float realm_hash_lo = hub::get(registry, owner, "SES_KYCD_NTF_REALM_ID_LO"_hs);
        if (ase::types::is_not_found(realm_hash_lo)) realm_hash_lo = 0.0f;

        auto& idn = registry.emplace<StorageInpKycdNtfyIdnComponent>(req_entity);
        idn.user_hash_hi = user_hash_hi;
        idn.user_hash_lo = user_hash_lo;
        idn.realm_hash_hi = realm_hash_hi;
        idn.realm_hash_lo = realm_hash_lo;

        auto& grnt = registry.emplace<StorageInpKycdNtfyGrntComponent>(req_entity);
        grnt.clearance = clearance;
        grnt.expires_at = expires_at;
    }
}

void StorageKycdNtfySynSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdNtfySynSystem] Stopped");
}

}  // namespace ase::storage
