/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_kycd_ntfy_syn_sys.cpp
 * @brief       StorageKycdNtfySynSystem - mirrors the star lines of one keycard request
 * @description Hub READ only. Eine Hub-Zeile hinein, zwei Bruecken-Components hinaus, keine Rechnung.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
 * @usecase     usecase:100264 - issue customer keycard
 * @created     2026-09-21
 * @modified    2026-10-12
 * @version     00.00.00.00001 [seed]
 *
 * CAUSAL CHAIN (CAUSA_STG_KYCD_NTFY_SYN: the star is read here, the numbers are built there)
 *
 *   ase-pl-webserver (Ausgabe, HUB_STG_KYCD_REQ) / ase-pl-edge-webserver (Praegung, HUB_STG_KYCD_MINT_REQ)
 *          │
 *          │ eine Hub-Zeile mit der Familienmarke HUB_STG_KYCD
 *          ▼
 *   ┌─────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageKycdNtfySynSystem                      │
 *   │                                                             │
 *   │  READS:                                                     │
 *   │    - die Zeilen mit der Marke HUB_STG_KYCD, Feld fuer Feld  │
 *   │                                                             │
 *   │  WRITES:                                                    │
 *   │    - StorageInpKycdNtfyIdnComponent  (Hashes, Benutzer-ID)  │
 *   │    - StorageInpKycdNtfyGrntComponent (Stufe, Rechte, Ablauf)│
 *   └─────────────────────────────────────────────────────────────┘
 *          │
 *          │ StorageKycdNtfyDrnSystem baut daraus die Anfrage
 *          ▼
 *   StorageReqKycdComponent, und von dort die bestehende Keycard-Kette
 *
 * WARUM DIESES SYSTEM EXISTIERT. Die Regel HUB_IO_MIXED_WITH_MATH trennt das Lesen des Sterns von
 * der Rechnung; ihre `suggestion` ist woertlich der Bauplan dieses Systems — eine SERVER-ONLY Seite
 * liest den Stern in eine `*_inp_*`-Zeile, die rechnende Seite nimmt sie von dort.
 *
 * DIE MELDUNG WANDERT MIT DEM LESEN. Fehlt ein Pflichtfeld (Benutzer-Hash, Benutzer-ID, Ablauf),
 * meldet es dieses System und legt KEINE Bruecke an; der Drain filtert auf ihre Anwesenheit und
 * sieht die Anfrage gar nicht erst.
 *
 * DREI FELDER DUERFEN FEHLEN, und ihr Fehlen ist eine Aussage, keine Stoerung: die Ausstellung des
 * Auth-Gates traegt keine Rechte, und Freigabestufe und Realm fallen bei Abwesenheit auf null.
 *
 * HUB Pattern (MIG_ASE_HUB_API v2.0):
 *
 * READS (from the star, every row with the family mark HUB_STG_KYCD, of either kind, on the request
 *        entity; both kinds carry the same field ids):
 *   HUB_STG_KYCD_REQ_USER_HASH, _USER_ID, _EXP_AT   ← Pflicht
 *   HUB_STG_KYCD_REQ_CLEARANCE, _REALM_HASH, _PERM  ← Abwesenheit = 0
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
// Module constants (MAX_OWNER_ID, the width of a user id)
#include <ase/storage/types.hpp>
// Hub: api.hpp ist der EINZIGE erlaubte Hub-Header; die Zeile wird ueber ihn gelesen.
#include <ase/hub/api.hpp>
// Bounded string copy into the bridge (L0)
#include <ase/utils/strops.hpp>
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
     * DIE ZEILEN, DIE NOCH KEINE BRUECKE UND KEINE NUTZLAST HABEN.
     *
     * Der Ausschluss macht den Durchlauf idempotent: eine Anfrage wird genau einmal gespiegelt,
     * und ein zweiter Takt vor dem Drain schreibt nichts neu.
     */
    hub::mark_each(registry, "HUB_STG_KYCD"_hs, [&](ecs::Entity req_entity) {
        if (registry.any_of<StorageReqKycdComponent, StorageInpKycdNtfyIdnComponent>(req_entity)) return;
        const uint32_t row = static_cast<uint32_t>(req_entity);

        /**
         * DREI PFLICHTFELDER, gemeinsam geprueft. Fehlt eines, entsteht KEINE Bruecke - der
         * Drain sieht die Anfrage dann gar nicht. Die Benutzer-ID wird in ihrer Laenge gelesen;
         * die Bruecke ist ein Byte breiter, damit sie terminiert bleibt.
         */
        uint32_t user_hash = 0u;
        uint64_t expires_at = 0ull;
        const uint32_t user_id_len = hub::bytes_len(registry, row, "HUB_STG_KYCD_REQ_USER_ID"_hs);
        char user_id[MAX_OWNER_ID] = {};
        if (!hub::try_get_u32(registry, row, "HUB_STG_KYCD_REQ_USER_HASH"_hs, user_hash) ||
            !hub::try_get_u64(registry, row, "HUB_STG_KYCD_REQ_EXP_AT"_hs, expires_at) ||
            user_id_len == 0u || user_id_len >= MAX_OWNER_ID ||
            !hub::try_get_bytes(registry, row, "HUB_STG_KYCD_REQ_USER_ID"_hs, user_id, user_id_len)) {
            log::warn(log::WRN::CAT::INPUT_TRUNCATED, "StorageKycdNtfySynSystem", row, "HUB_STG_KYCD_REQ",
                      "the keycard request lacks a field and is not mirrored");
            return;
        }

        /**
         * DREI FELDER DUERFEN FEHLEN und fallen dann auf null: die Ausstellung des Auth-Gates
         * traegt keine Rechte und unter Umstaenden weder Freigabestufe noch Realm.
         */
        uint32_t clearance = 0u;
        uint32_t realm_hash = 0u;
        uint32_t perm = 0u;
        if (!hub::try_get_u32(registry, row, "HUB_STG_KYCD_REQ_CLEARANCE"_hs, clearance)) clearance = 0u;
        if (!hub::try_get_u32(registry, row, "HUB_STG_KYCD_REQ_REALM_HASH"_hs, realm_hash)) realm_hash = 0u;
        if (!hub::try_get_u32(registry, row, "HUB_STG_KYCD_REQ_PERM"_hs, perm)) perm = 0u;

        auto& idn = registry.emplace<StorageInpKycdNtfyIdnComponent>(req_entity);
        idn.user_hash = user_hash;
        idn.realm_hash = realm_hash;
        ase::utils::str_copy(idn.user_id, MAX_OWNER_ID, user_id);

        auto& grnt = registry.emplace<StorageInpKycdNtfyGrntComponent>(req_entity);
        grnt.clearance = clearance;
        grnt.perm = perm;
        grnt.expires_at = expires_at;
    });
}

void StorageKycdNtfySynSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdNtfySynSystem] Stopped");
}

}  // namespace ase::storage
