/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_acss_pol_sys.cpp
 * @brief       StorageAcssPolSystem - die Politik-Tore der A/ACS-Leiter (Step 7-10)
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @created     2026-08-31
 * @modified    2026-08-31
 * @version     1.0.0
 *
 * TRENNUNG 2026-08-31 — DER ANRUFER UND DER GEGENSTAND WAREN EINE tick()
 *
 * Die Torleiter wog beides in derselben Funktion: WER DARF (Ausweis, Revier, Gitter, Schutzstufe,
 * Codewort, Recht) und WAS GILT FUER DIESEN GEGENSTAND (Label, laufende Aufgabe, Kontingent). Die
 * erste Frage haengt am ANRUFER und aendert sich mit seiner Keycard; die zweite haengt am GUT und
 * aendert sich mit dem Arbeitsstand. Zwei Fragen, zwei Fehlerbilder, zwei Dateien — dieselbe
 * Trennung, die am 2026-08-29 schon Aufloesung und Torleiter geschieden hat.
 *
 * WAS DIE TRENNUNG WERT IST: eine Aenderung am Kontingent-Tarif - etwa eine neue Stufe - war
 * vorher eine Aenderung an derselben Datei, in der auch die Codewort-Pruefung steht. Das ist die
 * teuerste Nachbarschaft, die es in einer Zugriffsentscheidung geben kann.
 *
 * DER ANLASS WAR EIN VERSTECKTER HELFER, und das gehoert hierher, weil es sonst wie eine
 * Zeilenoptimierung aussieht: `emit_audit(ecs::Registry&, ...)` stellte die Pruefspur-Zeile aus
 * einem storage_acss_ladder-Header heraus - eine freie Funktion mit der Registry im Argument, also
 * ECS-Arbeit hinter einem Namen. Ausgeschrieben traegt jede der zehn Ausgangsstellen sie selbst,
 * und der Rumpf fiel damit in das GOD_SYSTEM_UNSPLIT_BAND. Die ZAHL war der Anlass, die NAHT war
 * schon vorher da.
 *
 * DIE LEITER BLEIBT STRENG GEORDNET UND ERTEILT AUSSCHLIESSLICH AUF SPROSSE 10. Der Schnitt
 * aendert daran nichts: er verlegt die Sprossen 7 bis 10 in eine eigene Datei, nicht ihre
 * Reihenfolge. Was hier ankommt, hat die Sprossen 1 bis 6 gehalten und traegt StorageAcssPassTag.
 *
 * WAS DIESES SYSTEM SELBST ABLEITET UND WARUM NICHT MEHR. Es braucht vier Groessen aus dem
 * Anrufer-Teil - die Eigentuemer-Vorgabe, die wirksame Freigabestufe, den Nutzer-Hash und die
 * Praefix-Hashes des Pfades - und leitet sie aus DENSELBEN Eingaben mit DENSELBEN reinen Formen
 * neu ab. `required_perm` und die wirksamen Rechte leitet es NICHT ab: die gehoeren den
 * Schluessel-Toren und werden hier von keinem Tor gelesen. Ein Wert, der an zwei Orten steht, ist
 * ein Wert, der an einem davon falsch sein kann.
 *
 * CAUSAL CHAIN (CHAIN_STORAGE_ACSS_POL: Key gates held → Grant or Deny)
 *
 *   StorageAcssChkSystem (Sprossen 1-6 gehalten)
 *          │
 *          │ StorageAcssPassTag
 *          ▼
 *   ┌─────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageAcssPolSystem                          │
 *   │                                                             │
 *   │  READS:                                                     │
 *   │    StorageStaAcssRslvComponent (Etikett, Schutzstufe)       │
 *   │    StorageAcssOwnrTag (Eigentuemer-Vorgabe)                 │
 *   │    StorageReqAcssComponent + StorageReqCredComponent        │
 *   │    StorageStaRelmComponent (Revier-Kennung und Tarifstufe)  │
 *   │    StorageStaTaskComponent + StorageTaskIdnComponent        │
 *   │                                                             │
 *   │  WRITES:                                                    │
 *   │    StorageAcssGrntTag oder StorageAcssDenyTag               │
 *   │    Pruefspur je Ausgang (StorageBufAudtComponent)           │
 *   └─────────────────────────────────────────────────────────────┘
 *          │
 *          │ Request entity tagged with grant or deny
 *          ▼
 *   StorageFileWritSystem processes granted requests
 *
 * WER NACH DIESEM SYSTEM LAEUFT, LIEF FRUEHER NACH StorageAcssChkSystem. Die Gewaehrungsmarke
 * entsteht seit dem 2026-08-31 hier; StorageFileWritSystem, StorageCncmFltSystem und
 * StorageCurPrcSystem tragen ihre Kante deshalb auf dieses System. Eine Kante, die auf der halben
 * Leiter stehen bliebe, saehe NULL Gewaehrungen und taete still gar nichts — kein Absturz, keine
 * Meldung, nur ein Dateischreiber, der nie schreibt.
 *
 * HUB Pattern (ARCH_ASE_HUB)
 *
 * READS (from ase-storage internal):
 *   StorageStaAcssRslvComponent → Etikett und Schutzstufe der aufgeloesten Regel
 *   StorageReqCredComponent     → die Keycard des Anrufers, sofern die Eigentuemer-Vorgabe nicht greift
 *   StorageStaRelmComponent     → Revier-Kennung und Tarifstufe fuer die Kontingent-Grenze
 *
 * WRITES (to ase-storage internal):
 *   StorageAcssGrntTag / StorageAcssDenyTag → die Entscheidung, genau eine je Anfrage
 *   StorageBufAudtComponent                 → die Pruefspur-Zeile dieser Entscheidung
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
#include <ase/storage/systems/acl/storage_acss_pol_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_req_acss_comp.hpp>
#include <ase/storage/components/state/storage_req_cred_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/state/storage_sta_acss_rslv_comp.hpp>
#include <ase/storage/components/state/storage_sta_task_comp.hpp>
#include <ase/storage/components/state/storage_task_idn_comp.hpp>
// Die Pruefspur-Zeile wird in DIESEM Rumpf gestellt, nicht von einem Helfer mit ecs::Registry&.
#include <ase/storage/components/state/storage_buf_audt_comp.hpp>
#include <ase/storage/components/state/storage_audt_outc_comp.hpp>
#include <ase/storage/components/tag/storage_audt_pend_tag.hpp>
#include <ase/storage/components/tag/storage_acss_grnt_tag.hpp>
#include <ase/storage/components/tag/storage_acss_deny_tag.hpp>
#include <ase/storage/components/tag/storage_acss_pass_tag.hpp>
#include <ase/storage/components/tag/storage_acss_ownr_tag.hpp>
#include <ase/storage/storage_resource_manager.hpp>
#include <ase/storage/storage_acss_index_resource_manager.hpp>
#include <ase/storage/storage_acss_ladder.hpp>
#include <ase/storage/types.hpp>
#include <ase/utils/strops.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>

#include <entt/core/hashed_string.hpp>

namespace ase::storage {

using namespace entt::literals;

// Anonymous namespace for helper FUNCTIONS (NOT static!)
// NO View/Query operations in helpers! Only pure math!
namespace {

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageAcssPolSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageAcssPol] Started");
}

void StorageAcssPolSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto* mgr_ptr = registry.ctx().find<StorageResourceManager*>();
    if (!mgr_ptr || !(*mgr_ptr)) {
        return;
    }
    auto& mgr = **mgr_ptr;
    uint64_t now = mgr.get_wall_time_seconds();

    auto* idx_ptr = registry.ctx().find<StorageAcssIndexResourceManager*>();
    if (!idx_ptr || !(*idx_ptr)) {
        // SCHEDULE_ORDER: der Erzeuger (StorageAcssIdxSystem) hat den ctx-Halter noch nicht angelegt.
        log::error(log::ERR::CAT::SCHEDULE_ORDER, "StorageAcssPolSystem",
                   "StorageAcssIndexResourceManager");
        return;
    }
    auto& idx = **idx_ptr;

    // Die Eigentuemer-Klassifikation als Tag-gefilterte View, EINMAL gebaut. Die Zugehoerigkeit
    // ist dann ein O(1)-Test auf der gefilterten Menge - nie ein all_of<Tag> im Rumpf
    // (WRFL_ASE_TAGGED_VIEWS), und dieselbe Form, die die Schluessel-Tore davor nutzen.
    auto own_view = registry.view<StorageAcssOwnrTag>();

    // Nur Anfragen, deren SCHLUESSEL-Tore gehalten haben und die noch unentschieden sind. Die
    // Marke laesst nichts anderes herein: was an Sprosse 1 bis 6 gescheitert ist, traegt
    // StorageAcssDenyTag und faellt schon an der Ausschlussliste heraus.
    auto req_view = registry.view<StorageReqAcssComponent, StorageReqCredComponent,
                                  StorageStaAcssRslvComponent, StorageAcssPassTag>(
        entt::exclude<StorageAcssGrntTag, StorageAcssDenyTag>);
    for (auto entity : req_view) {
        auto& req  = req_view.get<StorageReqAcssComponent>(entity);
        auto& cred = req_view.get<StorageReqCredComponent>(entity);
        auto& rslv = req_view.get<StorageStaAcssRslvComponent>(entity);

        const bool owner_preset = own_view.contains(entity);

        // Effective clearance. The owner preset is the in-ladder model of former realm_owner
        // power: clearance 9. Without the preset, the auth-header value is used verbatim.
        // DIE WIRKSAMEN RECHTE WERDEN HIER NICHT ABGELEITET: kein Tor dieses Durchgangs liest
        // sie, und ein ungenutzter Wert an einem zweiten Ort ist eine Stelle, an der er falsch
        // sein kann, ohne dass es je auffaellt.
        const uint8_t eff_clrn = owner_preset ? ACSS_OWNER_CLEARANCE : cred.clrn;

        // Die Revier-Kennung und die Tarifstufe stehen auf dem Revier-Datensatz, nicht in der
        // Aufloesungs-Komponente: ein zweiter Ort waere ein zweiter Ort, an dem sie falsch sein
        // koennen. Verschwindet die Zeile zwischen den Systemen, faellt der Zugriff fail-closed -
        // und sagt dabei nicht mehr, als er weiss.
        const auto relm_ent = static_cast<ecs::Entity>(req.relm_ref);
        auto* relm_ptr = registry.try_get<StorageStaRelmComponent>(relm_ent);
        if (relm_ptr == nullptr) {
            log::error(log::ERR::CAT::COMPONENT_MISSING, "StorageAcssPolSystem",
                       req.relm_ref, "StorageStaRelmComponent");
            registry.emplace<StorageAcssDenyTag>(entity);
            auto aud_ent = registry.create();
            auto& aud = registry.emplace<StorageBufAudtComponent>(aud_ent);
            aud.relm_ref = req.relm_ref;
            aud.proj_ref = req.proj_ref;
            ase::utils::str_copy(aud.user_id, MAX_OWNER_ID, cred.user_id);
            ase::utils::str_copy(aud.path, MAX_PATH_LEN, req.path);
            aud.timestamp = now;
            auto& outc = registry.emplace<StorageAudtOutcComponent>(aud_ent);
            outc.action = req.action;
            outc.result = AUD_DENIED;
            ase::utils::str_copy(outc.reason, MAX_REASON_LEN, "realm_not_found");
            registry.emplace<StorageAudtPendTag>(aud_ent);
            continue;
        }
        char target_id[MAX_REALM_ID] = {};
        ase::utils::str_copy(target_id, MAX_REALM_ID, relm_ptr->id);
        const uint8_t target_tier = relm_ptr->tier;

        // Identity is a lookup, and a lookup compares hashes, never characters
        // (WRFL_ASE_STRING_HANDLING Section 3). Hashed ONCE per request, like in the key gates.
        const uint32_t user_hash = entt::hashed_string(cred.user_id).value();

        // Die Praefix-Hashes des Pfades, einmal je Anfrage: der Kenntnisnahme-Bereich unten fragt
        // "liegt dieser Pfad unter X" und liest dieselben Zahlen (storage_acss_ladder.hpp).
        uint32_t pfx_hash[MAX_PATH_LEN + 1] = {};
        const uint32_t path_len = ase::utils::str_len(req.path, MAX_PATH_LEN);
        path_prefix_hashes(req.path, path_len, pfx_hash);

        // DIE PRUEFSPUR-ZEILE ENTSTEHT EINMAL JE ANFRAGE, NICHT EINMAL JE AUSGANG. Ihre
        // anfrageabgeleitete Haelfte - Revier, Projekt, Nutzer, Pfad, Zeitstempel - ist an JEDEM
        // Ausgang dieselbe; nur Ergebnis und Grund unterscheiden sich. Jede Anfrage dieses
        // Durchgangs erreicht genau einen Ausgang, also entsteht genau eine Zeile. Der frueher
        // benutzte Helfer stellte sie an jeder Stelle neu und wiederholte dabei fuenf Felder
        // fuenfmal - genau die Stelle, an der ein kuenftiges sechstes Feld in vier von fuenf
        // Faellen gefehlt haette.
        auto aud_ent = registry.create();
        auto& aud = registry.emplace<StorageBufAudtComponent>(aud_ent);
        aud.relm_ref = req.relm_ref;
        aud.proj_ref = req.proj_ref;
        ase::utils::str_copy(aud.user_id, MAX_OWNER_ID, cred.user_id);
        ase::utils::str_copy(aud.path, MAX_PATH_LEN, req.path);
        aud.timestamp = now;

        // ── Step 7: LABEL ─ workflow-status gate.
        // retired = withdrawn build (no access); draft/review = team-only (clearance >= TEAM).
        if (rslv.rule_label_hash == EDGE_LABEL_RETIRED_HASH) {
            registry.emplace<StorageAcssDenyTag>(entity);
            auto& outc = registry.emplace<StorageAudtOutcComponent>(aud_ent);
            outc.action = req.action;
            outc.result = AUD_DENIED;
            ase::utils::str_copy(outc.reason, MAX_REASON_LEN, "retired_asset");
            registry.emplace<StorageAudtPendTag>(aud_ent);
            continue;
        }
        if ((rslv.rule_label_hash == EDGE_LABEL_DRAFT_HASH ||
             rslv.rule_label_hash == EDGE_LABEL_REVIEW_HASH) &&
            eff_clrn < PROTECTION_TEAM) {
            registry.emplace<StorageAcssDenyTag>(entity);
            auto& outc = registry.emplace<StorageAudtOutcComponent>(aud_ent);
            outc.action = req.action;
            outc.result = AUD_DENIED;
            ase::utils::str_copy(outc.reason, MAX_REASON_LEN, "label_restricted");
            registry.emplace<StorageAudtPendTag>(aud_ent);
            continue;
        }

        // ── Step 8: NEED-TO-KNOW ─ active task scoping (Enterprise).
        // When the assignee has any active need-to-know task in this project, access is
        // restricted to that task's path scope. Owner-preset governs all and is exempt.
        if (!owner_preset && req.proj_ref != 0) {
            bool has_active_task = false;
            bool path_in_scope   = false;
            // Tasks are indexed under their project, so the project filter is the bucket
            // itself and only the assignee still has to be compared.
            const uint32_t task_count = idx.get_task_count(req.proj_ref);
            for (uint32_t task_index = 0; task_index < task_count; ++task_index) {
                const uint32_t task_id = idx.get_task(req.proj_ref, task_index);
                if (task_id == INVALID_ENTITY) { continue; }
                auto* task_ptr =
                    registry.try_get<StorageStaTaskComponent>(static_cast<ecs::Entity>(task_id));
                if (task_ptr == nullptr) { continue; }
                auto& task = *task_ptr;
                auto* task_idn =
                    registry.try_get<StorageTaskIdnComponent>(static_cast<ecs::Entity>(task_id));
                if (task_idn == nullptr) {
                    log::error(log::ERR::CAT::COMPONENT_MISSING, "StorageAcssPolSystem",
                               task_id, "StorageTaskIdnComponent");
                    continue;
                }
                if (task_idn->assignee_hash != user_hash) { continue; }
                bool live = (task.starts_at == 0 || task.starts_at <= now) &&
                            (task.expires_at == 0 || task.expires_at > now);
                if (!live) { continue; }
                has_active_task = true;
                if (task_idn->scope_len < 1u) { continue; }
                if (task_idn->scope_len > path_len) { continue; }
                if (pfx_hash[task_idn->scope_len] == task_idn->scope_hash) {
                    path_in_scope = true;
                    break;
                }
            }
            if (has_active_task && !path_in_scope) {
                registry.emplace<StorageAcssDenyTag>(entity);
                auto& outc = registry.emplace<StorageAudtOutcComponent>(aud_ent);
                outc.action = req.action;
                outc.result = AUD_DENIED;
                ase::utils::str_copy(outc.reason, MAX_REASON_LEN, "need_to_know");
                registry.emplace<StorageAudtPendTag>(aud_ent);
                continue;
            }
        }

        // ── Step 9: QUOTA ─ realm storage budget gate (WRITE only).
        // A write is refused when the realm's measured usage already meets its tier limit.
        if (req.action == AUD_WRITE) {
            uint64_t tier_limit = QUOTA_INDIE_STORAGE;
            if (target_tier == TIER_PRO)        { tier_limit = QUOTA_PRO_STORAGE; }
            if (target_tier == TIER_ENTERPRISE) { tier_limit = QUOTA_ENT_STORAGE; }
            uint64_t used = mgr.get_realm_usage(target_id);
            if (used >= tier_limit) {
                registry.emplace<StorageAcssDenyTag>(entity);
                auto& outc = registry.emplace<StorageAudtOutcComponent>(aud_ent);
                outc.action = req.action;
                outc.result = AUD_DENIED;
                ase::utils::str_copy(outc.reason, MAX_REASON_LEN, "quota_exceeded");
                registry.emplace<StorageAudtPendTag>(aud_ent);
                continue;
            }
        }

        // ── Step 10: GRANT + AUDIT(GRANTED) ─ reached only after every applicable step passed
        registry.emplace<StorageAcssGrntTag>(entity);
        auto& outc = registry.emplace<StorageAudtOutcComponent>(aud_ent);
        outc.action = req.action;
        outc.result = AUD_GRANTED;
        ase::utils::str_copy(outc.reason, MAX_REASON_LEN, "");
        registry.emplace<StorageAudtPendTag>(aud_ent);
    }
}

void StorageAcssPolSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageAcssPol] Stopped");
}

}  // namespace ase::storage
