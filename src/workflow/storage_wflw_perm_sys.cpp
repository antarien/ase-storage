/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_wflw_perm_sys.cpp
 * @brief       StorageWflwPermSystem - Enforce the PERM_PROMOTE axis on transition requests
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @created     2026-08-29
 * @modified    2026-08-29
 * @version     1.0.0
 *
 * TRENNUNG 2026-08-29 — BERECHTIGEN UND BEWEGEN WAREN EINE tick()
 *
 * "Darf dieser Anrufer befoerdern" ist eine A/ACS-Frage - dieselbe Familie wie die Zugriffsleiter
 * in storage_acss_chk_sys.cpp. "Ist dieser Uebergang im Graphen vorgesehen" ist eine Frage an den
 * Arbeitsablauf. Beide standen in derselben Funktion, und die erste sah dort aus wie ein Vorspann
 * der zweiten - dabei hat sie eine eigene Quelle (die Keycard-Sitzung im Hub), eine eigene
 * Fehlerkategorie (ACCESS_DENIED) und einen eigenen Ausgang.
 *
 * DER UNTERSCHIED IST NICHT KOSMETISCH. Wer die Kantenpruefung aendert, aendert den Graphen; wer
 * diese Pruefung aendert, aendert eine Sicherheitsentscheidung. Dass beides in einer Datei lag,
 * hiess: jede Aenderung am einen war eine Aenderung an der Datei des anderen.
 *
 * FAIL-CLOSED, NIE EIN STILLER GRANT. Ein fehlender Schluessel - keine lebende Sitzung auf diesem
 * Dist - heisst KEINE Rechte, nicht "unbekannt, also durchlassen". Ein negativer Wert wird
 * gemeldet und wie null behandelt: eine Rechtemaske kann nicht negativ sein, und was sie
 * stattdessen ist, darf nicht geraten werden.
 *
 * CAUSAL CHAIN (CHAIN_STORAGE_WFLW_PERM: Request → Permission verdict)
 *
 *   [HTTP-Route legt die Beförderungs-Anfrage an]
 *          │
 *          │ StorageReqWflwTranComponent + StorageWflwPendTag
 *          ▼
 *   ┌─────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageWflwPermSystem                         │
 *   │                                                             │
 *   │  READS:                                                     │
 *   │    StorageReqWflwTranComponent (Pfad, Ziel, Antragsteller)  │
 *   │    SES_KYCD_PERM (owner = hashed_string(requested_by))      │
 *   │                                                             │
 *   │  WRITES:                                                    │
 *   │    STG_WFLW_RES = DENIED_PERM bei fehlendem Recht           │
 *   │    StorageBufAudtComponent (Pruefspur der Ablehnung)        │
 *   └─────────────────────────────────────────────────────────────┘
 *          │
 *          │ die berechtigten Anfragen bleiben offen
 *          ▼
 *   StorageWflwIniSystem → StorageWflwTranSystem
 *
 * HUB Pattern (Active)
 *
 * READS (from Hub):
 *   - SES_KYCD_PERM (owner = hashed_string(requested_by)): PERM_PROMOTE axis
 *
 * WRITES (to Hub):
 *   - STG_WFLW_RES (owner = hashed_string(path)): WFLW_RES_DENIED_PERM on refusal
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
#include <ase/storage/systems/workflow/storage_wflw_perm_sys.hpp>
// Components from same module
#include <ase/storage/components/request/storage_req_wflw_tran_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/tag/storage_wflw_pend_tag.hpp>
#include <ase/storage/components/tag/storage_relm_edge_tag.hpp>
#include <ase/storage/components/tag/storage_wflw_gate_tag.hpp>
#include <ase/storage/storage_resource_manager.hpp>
#include <ase/storage/storage_wflw_stage.hpp>
#include <ase/storage/types.hpp>
// Hub API (perm read + verdict publish)
#include <ase/hub/api.hpp>
// Types (L0 — is_not_found / is_neg_float sentinel checks on hub::get reads)
#include <ase/types/types.hpp>
// String ops (L0)
#include <ase/utils/strops.hpp>

#include <entt/core/hashed_string.hpp>
// Logging
#include <ase/log/log.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
namespace {

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageWflwPermSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageWflwPerm] Started");
}

void StorageWflwPermSystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    auto* mgr_ptr = registry.ctx().find<StorageResourceManager*>();
    if (!mgr_ptr || !(*mgr_ptr)) {
        return;  // manager not up yet (StorageIniSystem seeds it at Initialization)
    }
    auto& mgr = **mgr_ptr;

    // Deferred deletion: only the refused requests are collected here; a permitted one stays
    // pending and is carried on in the same tick.
    ecs::Entity done[WFLW_REQ_BATCH];
    uint32_t done_n = 0;

    // Edge realm entity ref (audit record scope). The answer does not depend on the request,
    // so it is resolved ONCE per tick instead of once per request.
    uint32_t relm_ref = 0;
    for (auto relm_ent : registry.view<StorageStaRelmComponent, StorageRelmEdgeTag>()) {
        relm_ref = static_cast<uint32_t>(relm_ent);
        break;
    }

    // Requests still carrying StorageWflwGateTag belong to StorageWflwGateSystem.
    auto req_view = registry.view<StorageReqWflwTranComponent, StorageWflwPendTag>(
        entt::exclude<StorageWflwGateTag>);
    for (auto [req_ent, req] : req_view.each()) {
        if (done_n >= WFLW_REQ_BATCH) break;

        const uint32_t owner = entt::hashed_string(req.path).value();
        const uint64_t now = mgr.get_wall_time_seconds();

        // A/ACS permission axis: the requester keycard session must hold
        // PERM_PROMOTE (published owner-scoped by the keycard pipeline). Clearance
        // was gated at the route (operator mint-gate); permission is enforced HERE.
        // A missing key (no live session on this dist) means NO permissions —
        // fail-closed, never a silent grant.
        const uint32_t requester = entt::hashed_string(req.requested_by).value();
        float perm_f = hub::get(registry, requester, "SES_KYCD_PERM"_hs);
        if (ase::types::is_not_found(perm_f)) {
            perm_f = 0.0f;
        }
        if (ase::types::is_neg_float(perm_f)) {
            log::warn(log::WRN::CAT::VALUE_NEGATIVE, "StorageWflwPermSystem", requester,
                      "SES_KYCD_PERM", perm_f);
            perm_f = 0.0f;
        }
        const uint16_t perm = static_cast<uint16_t>(perm_f);
        if ((perm & PERM_PROMOTE) != 0u) {
            continue;  // permitted - the transition systems take it from here
        }

        hub::set(registry, owner, "STG_WFLW_RES"_hs, static_cast<float>(WFLW_RES_DENIED_PERM));
        // EINE Audit-Entity je Entscheidung, mit StorageAudtPendTag fuer den
        // Preservation-Stapelschreiber. Die Identitaet des Anfragenden IST die
        // Keycard-Zuschreibung, die der Vertrag verlangt. proj_ref bleibt 0 (ein Uebergang
        // gehoert einem Realm, keinem Projekt), action bleibt AUD_PROMOTE.
        auto aud_ent = registry.create();
        auto& aud = registry.emplace<StorageBufAudtComponent>(aud_ent);
        aud.relm_ref = relm_ref;
        aud.proj_ref = 0;
        ase::utils::str_copy(aud.user_id, MAX_OWNER_ID, req.requested_by);
        ase::utils::str_copy(aud.path, MAX_PATH_LEN, req.path);
        aud.timestamp = now;
        auto& outc = registry.emplace<StorageAudtOutcComponent>(aud_ent);
        outc.action = AUD_PROMOTE;
        outc.result = AUD_DENIED;
        ase::utils::str_copy(outc.reason, MAX_REASON_LEN, "missing_perm(PROMOTE)");
        registry.emplace<StorageAudtPendTag>(aud_ent);
        /**
         * ACCESS_DENIED auf ERR-Ebene: hier geht es um eine BERECHTIGUNG — der Anfragende hat
         * PERM_PROMOTE nicht. Die Kategorie liegt laut ihrer Abgrenzung bewusst im Fehlerstrom,
         * damit eine Ablehnung aus keinem ERR-Filter faellt; davon lebt eine Sicherheitsspur.
         * Der Vorgang endet hier auch wirklich: die Anfrage wird abgeschlossen, kein spaeterer
         * Durchlauf holt sie nach.
         *
         * DREI BEZEICHNER MUESSEN MIT, und die detail-Ueberladung traegt sie: `owner` den
         * Pfad-Hash, value_id den lesbaren Pfad, der detail Ziel-Etikett und Antragsteller. Eine
         * Ablehnung, die man nicht mehr einer Anfrage zuordnen kann, ist als Sicherheitsspur
         * wertlos - der Auditsatz daneben haelt sie vollstaendig, diese Zeile ist ihr lesbares
         * Echo.
         */
        char perm_detail[256] = {};
        ase::utils::str_copy(perm_detail, 256, "target=");
        ase::utils::str_append(perm_detail, 256, req.target_label);
        ase::utils::str_append(perm_detail, 256, " requester=");
        ase::utils::str_append(perm_detail, 256, req.requested_by);
        ase::utils::str_append(perm_detail, 256, " missing_perm=PERM_PROMOTE");
        log::error(log::ERR::CAT::ACCESS_DENIED, "StorageWflwPerm", owner, req.path,
                   perm_detail);
        done[done_n] = req_ent;
        ++done_n;
    }

    for (uint32_t i = 0; i < done_n; ++i) {
        registry.destroy(done[i]);
    }
}

void StorageWflwPermSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageWflwPerm] Stopped");
}

}  // namespace ase::storage
