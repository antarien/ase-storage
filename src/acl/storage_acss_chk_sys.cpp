/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_acss_chk_sys.cpp
 * @brief       StorageAcssChkSystem - Run the KEY gates on a resolved request
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @created     2026-04-04
 * @modified    2026-08-31
 * @version     3.0.0
 *
 * TRENNUNG 2026-08-29 — NACHSCHLAGEN UND ENTSCHEIDEN WAREN EINE tick()
 *
 * Die Leiter nach ARCH Section 14.1 stellt zwei Arten von Frage, und sie standen in derselben
 * Funktion. StorageAcssRslvSystem beantwortet die eine - welches Revier, wem gehoert es, welche
 * ACL-Regel regiert diesen Pfad -, dieses System die andere: DARF DIESER ANRUFER.
 *
 * WAS DIE TRENNUNG WERT IST: eine Aenderung am Regelabgleich - etwa daran, welche von zwei
 * ueberlappenden Regeln gewinnt - war vorher eine Aenderung an derselben Datei, in der auch die
 * Freigabestufen-Pruefung steht. Das ist die teuerste Nachbarschaft, die es hier geben kann.
 *
 * TRENNUNG 2026-08-31 — DER ANRUFER UND DER GEGENSTAND WAREN EINE tick()
 *
 * Dieselbe Bewegung ein zweites Mal, eine Ebene tiefer. Die Torleiter wog beides: WER DARF
 * (Ausweis, Revier, Gitter, Schutzstufe, Codewort, Recht) und WAS GILT FUER DIESEN GEGENSTAND
 * (Label, laufende Aufgabe, Kontingent). Die erste Frage haengt am ANRUFER und aendert sich mit
 * seiner Keycard; die zweite haengt am GUT und aendert sich mit dem Arbeitsstand. DIESES SYSTEM
 * TRAEGT SEITHER NUR NOCH DIE ERSTE — Sprossen 1 bis 6. Die Sprossen 7 bis 10 stehen in
 * StorageAcssPolSystem.
 *
 * DER ANLASS WAR EIN VERSTECKTER HELFER: `emit_audit(ecs::Registry&, ...)` stellte die
 * Pruefspur-Zeile aus storage_acss_ladder.hpp heraus - eine freie Funktion mit der Registry im
 * Argument, also ECS-Arbeit hinter einem Namen. Ausgeschrieben traegt jede Ausgangsstelle sie
 * selbst; mit allen zehn Stellen fiel dieser Rumpf in das GOD_SYSTEM_UNSPLIT_BAND. Die ZAHL war
 * der Anlass, die NAHT war schon vorher da.
 *
 * DIE LEITER IST STRENG GEORDNET UND ERTEILT AUSSCHLIESSLICH AUF SPROSSE 10 — die liegt jetzt
 * drueben. Es gibt keine Abkuerzung davor: der oeffentliche Raum und die Eigentuemer-Macht sind
 * INNERHALB der Leiter modelliert (PUBLIC-Schutzregel, Eigentuemer-Vorgabe als wirksame Keycard)
 * - sie ueberspringen kein Tor, sie machen die Tore fuer diesen Fall gegenstandslos.
 *
 * WER DIE FUENF TORE HAELT, BEKOMMT StorageAcssPassTag UND KEINE PRUEFSPUR. Das ist kein
 * vergessener Ausgang, sondern die Naht: die Entscheidung faellt drueben, und eine Pruefspur ohne
 * Entscheidung waere eine Zeile, die etwas behauptet, das noch niemand entschieden hat. DESHALB
 * WIRD DIE PRUEFSPUR-ZEILE HIER AN JEDER ABLEHNUNG EINZELN GESTELLT und nicht wie drueben einmal
 * je Anfrage vorgezogen: dort endet JEDER Pfad in einer Entscheidung, hier nicht. Wer das
 * angleichen will, laesst hier eine halbe Pruefspur-Zeile ohne Ergebnis zurueck.
 *
 * WAS AUS DER AUFLOESUNG KOMMT UND WAS NICHT. Die Komponente traegt drei Zahlen - Schutzstufe,
 * Regel, Etikett -, die Marke StorageAcssOwnrTag die Eigentuemer-Klassifikation. Alles andere
 * leitet dieses System selbst ab: die wirksame Keycard aus Marke plus Keycard-Komponente, die
 * Pfad-Hashes aus dem Pfad, die Revier-Kennung aus dem Revier-Datensatz. Ein Wert, der an zwei
 * Orten steht, ist ein Wert, der an einem davon falsch sein kann - und in einer
 * Zugriffsentscheidung ist das die teuerste Sorte Fehler.
 *
 * DIE TARIFSTUFE DES REVIERS WIRD HIER NICHT MEHR GELESEN: sie trug ausschliesslich das
 * Kontingent-Tor, und das ist mitgezogen. Ein abgeleiteter Wert ohne Leser ist eine Stelle, an
 * der er falsch sein kann, ohne dass es je auffaellt.
 *
 * A MISSING KEYCARD OR REALM NEVER REACHES HERE. Beide Faelle weist die Aufloesung ab, mit
 * Pruefspur und ohne Leck: eine verschleierte Zeile faellt als realm_not_found, nie als
 * access_denied.
 *
 * CAUSAL CHAIN (CHAIN_STORAGE_ACSS_CHK: Resolution → Key gates → Pass or Deny)
 *
 *   StorageAcssRslvSystem (Revier und Regel aufgeloest)
 *          │
 *          │ StorageStaAcssRslvComponent + StorageAcssRslvTag
 *          ▼
 *   ┌─────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageAcssChkSystem                          │
 *   │                                                             │
 *   │  READS:                                                     │
 *   │    StorageStaAcssRslvComponent (Schutzstufe, Regel, Etikett)│
 *   │    StorageAcssOwnrTag (Eigentuemer-Vorgabe)                 │
 *   │    StorageReqAcssComponent + StorageReqCredComponent        │
 *   │    StorageLatLnkComponent + StorageLnkCnstComponent (Gitter)│
 *   │                                                             │
 *   │  WRITES:                                                    │
 *   │    StorageAcssPassTag oder StorageAcssDenyTag               │
 *   │    Pruefspur je ABLEHNUNG (StorageBufAudtComponent)         │
 *   └─────────────────────────────────────────────────────────────┘
 *          │
 *          │ StorageAcssPassTag
 *          ▼
 *   StorageAcssPolSystem (Sprossen 7-10) → Grant oder Deny → StorageFileWritSystem
 *
 * HUB Pattern (ARCH_ASE_HUB)
 *
 * READS (from ase-storage internal):
 *   StorageStaAcssRslvComponent → the resolution: protection level, rule, workflow label
 *   StorageReqCredComponent     → the caller's keycard, unless the owner preset overrides it
 *   StorageStaRelmComponent     → realm id, for the lattice key
 *
 * WRITES (to ase-storage internal):
 *   StorageAcssPassTag / StorageAcssDenyTag → das Zwischenergebnis der fuenf Schluessel-Tore
 *   StorageBufAudtComponent                 → die Pruefspur-Zeile jeder Ablehnung
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
 * [ ] NO switch/case statements? (use Tag-filtered Views!)
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
#include <ase/storage/systems/acl/storage_acss_chk_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_req_acss_comp.hpp>
#include <ase/storage/components/state/storage_req_cred_comp.hpp>
#include <ase/storage/components/state/storage_acss_cwrd_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/state/storage_sta_acss_rslv_comp.hpp>
#include <ase/storage/components/state/storage_lat_lnk_comp.hpp>
#include <ase/storage/components/state/storage_lnk_cnst_comp.hpp>
#include <ase/storage/components/state/storage_lnk_idn_comp.hpp>
// Die Pruefspur-Zeile wird seit dem 2026-08-31 in DIESEM Rumpf gestellt, nicht mehr von einem
// Helfer mit `ecs::Registry&` im Argument. Sie kam bis dahin transitiv ueber storage_acss_ladder;
// wer einen Typ selbst benutzt, fuehrt seine Kante selbst.
#include <ase/storage/components/state/storage_buf_audt_comp.hpp>
#include <ase/storage/components/state/storage_audt_outc_comp.hpp>
#include <ase/storage/components/tag/storage_audt_pend_tag.hpp>
#include <ase/storage/components/tag/storage_acss_grnt_tag.hpp>
#include <ase/storage/components/tag/storage_acss_deny_tag.hpp>
#include <ase/storage/components/tag/storage_acss_rslv_tag.hpp>
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

void StorageAcssChkSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageAcssChk] Started");
}

void StorageAcssChkSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto* mgr_ptr = registry.ctx().find<StorageResourceManager*>();
    if (!mgr_ptr || !(*mgr_ptr)) {
        return;
    }
    auto& mgr = **mgr_ptr;
    uint64_t now = mgr.get_wall_time_seconds();

    auto* idx_ptr = registry.ctx().find<StorageAcssIndexResourceManager*>();
    if (!idx_ptr || !(*idx_ptr)) {
        // SCHEDULE_ORDER: der Erzeuger (StorageAcssIdxSystem) hat den ctx-Halter noch nicht angelegt.
        log::error(log::ERR::CAT::SCHEDULE_ORDER, "StorageAcssChkSystem",
                   "StorageAcssIndexResourceManager");
        return;
    }
    auto& idx = **idx_ptr;

    // Die Eigentuemer-Klassifikation als Tag-gefilterte View, EINMAL gebaut. Die Zugehoerigkeit
    // ist dann ein O(1)-Test auf der gefilterten Menge - nie ein all_of<Tag> im Rumpf
    // (WRFL_ASE_TAGGED_VIEWS), und dieselbe Form, die die Aufloesung fuer Revier-Klassen nutzt.
    auto own_view = registry.view<StorageAcssOwnrTag>();

    // Nur aufgeloeste, noch unentschiedene Anfragen. Ohne Aufloesung gibt es weder Schutzstufe
    // noch Regel - die View laesst sie gar nicht erst herein, was dieselbe Wirkung hat wie eine
    // Abweisung und keine zweite Stelle schafft, an der ueber Zulaessigkeit entschieden wird.
    auto req_view = registry.view<StorageReqAcssComponent, StorageReqCredComponent,
                                  StorageStaAcssRslvComponent, StorageAcssRslvTag>(
        entt::exclude<StorageAcssGrntTag, StorageAcssDenyTag>);
    for (auto entity : req_view) {
        auto& req  = req_view.get<StorageReqAcssComponent>(entity);
        auto& cred = req_view.get<StorageReqCredComponent>(entity);
        auto& rslv = req_view.get<StorageStaAcssRslvComponent>(entity);

        const bool owner_preset = own_view.contains(entity);

        // Action → required permission bitflag (used by the lattice and the permission gate)
        uint16_t required_perm = PERM_READ;
        if (req.action == AUD_WRITE)   { required_perm = PERM_WRITE; }
        if (req.action == AUD_DELETE)  { required_perm = PERM_DELETE; }
        if (req.action == AUD_PROMOTE) { required_perm = PERM_PROMOTE; }
        if (req.action == AUD_MANAGE)  { required_perm = PERM_MANAGE; }

        // Effective keycard attributes. The owner preset is the in-ladder model of former
        // realm_owner power: clearance 9, all permissions, wildcard codeword. Without the preset,
        // the auth-header values are used verbatim. The public realm needs no boost — its PUBLIC
        // protection rule lets the auth-header values pass.
        const uint8_t  eff_clrn = owner_preset ? ACSS_OWNER_CLEARANCE : cred.clrn;
        const uint16_t eff_perm = owner_preset ? ACSS_OWNER_PERMS     : cred.perm;

        // Die Revier-Kennung steht auf dem Revier-Datensatz, nicht in der Aufloesungs-Komponente:
        // ein zweiter Ort waere ein zweiter Ort, an dem sie falsch sein kann. Verschwindet die
        // Zeile zwischen den beiden Systemen, faellt der Zugriff fail-closed - und sagt dabei
        // nicht mehr, als er weiss.
        const auto relm_ent = static_cast<ecs::Entity>(req.relm_ref);
        auto* relm_ptr = registry.try_get<StorageStaRelmComponent>(relm_ent);
        if (relm_ptr == nullptr) {
            log::error(log::ERR::CAT::COMPONENT_MISSING, "StorageAcssChkSystem",
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

        // Identity is a lookup, and a lookup compares hashes, never characters
        // (WRFL_ASE_STRING_HANDLING Section 3). Hashed ONCE per request, like in the resolution.
        const uint32_t user_hash = entt::hashed_string(cred.user_id).value();

        // Die Praefix-Hashes des Pfades, einmal je Anfrage: zwei Tore fragen "liegt dieser Pfad
        // unter X" - der Gitter-Anteil hier, die ACL-Regel drueben - und beide lesen dieselben
        // Zahlen (storage_acss_ladder.hpp). Der Kenntnisnahme-Bereich, der sie frueher ebenfalls
        // las, steht seit dem 2026-08-31 in StorageAcssPolSystem und leitet sie dort neu ab.
        uint32_t pfx_hash[MAX_PATH_LEN + 1] = {};
        const uint32_t path_len = ase::utils::str_len(req.path, MAX_PATH_LEN);
        path_prefix_hashes(req.path, path_len, pfx_hash);

        // ── Step 3: LATTICE ─ cross-realm access requires a valid, bilateral link.
        // A request whose path lies in another realm's shared prefix is only admissible
        // through an approved, unexpired lattice link; the link caps clearance and perms.
        // Owner-preset access stays within the owner's own realm hierarchy (no link needed).
        if (!owner_preset) {
            bool lattice_required = false;
            bool lattice_ok       = false;
            // Links are indexed under the realm they name as target. The bucket key and
            // the link's own target_realm_hash are the same number, so agreeing on it IS
            // the confirmation - a character comparison would only re-walk the string.
            const uint32_t target_hash = entt::hashed_string(target_id).value();
            const uint32_t link_count = idx.get_link_count(static_cast<uint64_t>(target_hash));
            for (uint32_t link_index = 0; link_index < link_count; ++link_index) {
                const uint32_t link_id =
                    idx.get_link(static_cast<uint64_t>(target_hash), link_index);
                if (link_id == INVALID_ENTITY) { continue; }
                const auto link_ent = static_cast<ecs::Entity>(link_id);
                auto* link_ptr = registry.try_get<StorageLatLnkComponent>(link_ent);
                if (link_ptr == nullptr) { continue; }
                auto& link = *link_ptr;
                auto* link_idn = registry.try_get<StorageLnkIdnComponent>(link_ent);
                if (link_idn == nullptr) {
                    log::error(log::ERR::CAT::COMPONENT_MISSING, "StorageAcssChkSystem",
                               link_id, "StorageLnkIdnComponent");
                    continue;
                }
                // The consent row is the second half of the same link. A link entity
                // without it is malformed, not un-approved - it is skipped like a link
                // without an identity row, never read as "both sides said no".
                auto* link_cnst = registry.try_get<StorageLnkCnstComponent>(link_ent);
                if (link_cnst == nullptr) {
                    log::error(log::ERR::CAT::COMPONENT_MISSING, "StorageAcssChkSystem",
                               link_id, "StorageLnkCnstComponent");
                    continue;
                }
                if (link_idn->target_realm_hash != target_hash) { continue; }
                if (link_idn->prefix_len < 1u) { continue; }
                if (link_idn->prefix_len > path_len) { continue; }
                if (pfx_hash[link_idn->prefix_len] != link_idn->prefix_hash) { continue; }
                lattice_required = true;
                bool approved = link_cnst->approved_by_source != 0 && link_cnst->approved_by_target != 0;
                bool live     = link.expires_at == 0 || link.expires_at > now;
                bool perm_ok  = (link.permissions & required_perm) != 0;
                bool clrn_ok  = rslv.required_protection <= link.max_clearance;
                if (approved && live && perm_ok && clrn_ok) {
                    lattice_ok = true;
                    break;
                }
            }
            if (lattice_required && !lattice_ok) {
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
                ase::utils::str_copy(outc.reason, MAX_REASON_LEN, "no_lattice_link");
                registry.emplace<StorageAudtPendTag>(aud_ent);
                continue;
            }
        }

        // ── Step 4: CLEARANCE ─ vertical Schutzstufe gate (public realm rule keeps PUBLIC)
        if (eff_clrn < rslv.required_protection) {
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
            ase::utils::str_copy(outc.reason, MAX_REASON_LEN, "insufficient_clearance");
            registry.emplace<StorageAudtPendTag>(aud_ent);
            continue;
        }

        // ── Step 5: CODEWORD ─ horizontal gate; keycard must hold EVERY required codeword.
        // The owner-preset wildcard satisfies any requirement; a held "ALL" codeword too.
        // The public realm carries no required codewords, so public reads pass unaffected.
        if (rslv.matched_rule != INVALID_ENTITY && !owner_preset) {
            // Three nested walks - required codewords, the user's keycards, and the
            // codewords each keycard carries - collapse into one walk over the rule's
            // own requirements plus four membership questions. The four cases are the
            // same the nested version tested: the keycard may be scoped to this realm or
            // global (scope 0), and a held wildcard satisfies any requirement.
            bool missing_codeword = false;
            const uint32_t required_count = idx.get_required_count(rslv.matched_rule);
            for (uint32_t required_index = 0; required_index < required_count; ++required_index) {
                const uint32_t cwrd_id = idx.get_required(rslv.matched_rule, required_index);
                if (cwrd_id == INVALID_ENTITY) { continue; }
                auto* required =
                    registry.try_get<StorageAcssCwrdComponent>(static_cast<ecs::Entity>(cwrd_id));
                if (required == nullptr) { continue; }
                const uint32_t needed = required->required_cwrd_hash;
                const bool held =
                    idx.has_held(StorageAcssIndexResourceManager::compose_held_key(
                        user_hash, req.relm_ref, needed)) ||
                    idx.has_held(StorageAcssIndexResourceManager::compose_held_key(
                        user_hash, 0u, needed)) ||
                    idx.has_held(StorageAcssIndexResourceManager::compose_held_key(
                        user_hash, req.relm_ref, ACSS_CWRD_WILDCARD_HASH)) ||
                    idx.has_held(StorageAcssIndexResourceManager::compose_held_key(
                        user_hash, 0u, ACSS_CWRD_WILDCARD_HASH));
                if (!held) { missing_codeword = true; break; }
            }
            if (missing_codeword) {
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
                ase::utils::str_copy(outc.reason, MAX_REASON_LEN, "missing_codeword");
                registry.emplace<StorageAudtPendTag>(aud_ent);
                continue;
            }
        }

        // ── Step 6: PERMISSION ─ action bitflag gate (owner preset holds all flags)
        if (!(eff_perm & required_perm)) {
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
            ase::utils::str_copy(outc.reason, MAX_REASON_LEN, "permission_denied");
            registry.emplace<StorageAudtPendTag>(aud_ent);
            continue;
        }

        // ── DIE FUENF SCHLUESSEL-TORE HABEN GEHALTEN ─ die Anfrage geht an die Politik-Tore.
        // KEINE PRUEFSPUR HIER, und das ist die Naht: entschieden ist noch nichts. Eine Zeile,
        // die "gewaehrt" oder "abgelehnt" behauptet, gehoert an den Ausgang, der es entscheidet,
        // und der liegt in StorageAcssPolSystem auf Sprosse 10.
        registry.emplace<StorageAcssPassTag>(entity);
    }
}

void StorageAcssChkSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageAcssChk] Stopped");
}

}  // namespace ase::storage
