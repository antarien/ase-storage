/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_wflw_tran_sys.cpp
 * @brief       StorageWflwTranSystem - Asset workflow label transitions
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @created     2026-04-05
 * @modified    2026-07-11
 * @version     2.0.0
 *
 * CAUSAL CHAIN (Workflow Transition)
 *
 *   [StorageWflwDrnSystem staged request; StorageWflwGateSystem passed released-gate]
 *          │
 *          │ Integration schedule processes label change
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageWflwTranSystem         │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageReqWflwTranComponent + PendTag  │
 *   │      (excludes StorageWflwGateTag)          │
 *   │    - StorageWflwEdgeComponent (die Kanten)  │
 *   │    - StorageAcssRuleComponent (label)       │
 *   │    - SES_KYCD_PERM (PERM_PROMOTE axis)      │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - StorageAcssRuleComponent.label         │
 *   │    - STG_WFLW_RES / STG_WFLW_STAGE          │
 *   │    - StorageBufAudtComponent (attribution)  │
 *   │    - StorageBufWflwComponent + PstPendTag   │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Label transitioned (draft → review → approved → released → retired)
 *          ▼
 *   StorageAcssChkSystem serves per new label; StorageWflwPstSystem persists (112)
 *
 * HUB Pattern (Active)
 *
 * READS (from Hub):
 *   - SES_KYCD_PERM (owner = hashed_string(requested_by)): PERM_PROMOTE axis
 *
 * WRITES (to Hub):
 *   - STG_WFLW_RES (owner = hashed_string(path)): WFLW_RES_* verdict
 *   - STG_WFLW_STAGE (owner = hashed_string(path)): WFLW_STAGE_* ordinal
 *
 * FLYWEIGHT PATTERN (Active - StorageResourceManager via ctx)
 *   Draft bootstrap checks on-disk asset presence; wall time for audit/persist.
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
#include <ase/storage/systems/workflow/storage_wflw_tran_sys.hpp>
// Components from same module
#include <ase/storage/components/request/storage_req_wflw_tran_comp.hpp>
#include <ase/storage/components/state/storage_wflw_edge_comp.hpp>
#include <ase/storage/storage_acss_index_resource_manager.hpp>
#include <ase/storage/components/state/storage_rule_idn_comp.hpp>
#include <ase/storage/components/state/storage_acss_rule_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/state/storage_buf_audt_comp.hpp>
#include <ase/storage/components/state/storage_audt_outc_comp.hpp>
#include <ase/storage/components/state/storage_buf_wflw_comp.hpp>
#include <ase/storage/components/state/storage_wflw_retr_comp.hpp>
#include <ase/storage/components/tag/storage_wflw_pend_tag.hpp>
#include <ase/storage/components/tag/storage_wflw_retr_tag.hpp>
#include <ase/storage/components/tag/storage_relm_edge_tag.hpp>
#include <ase/storage/components/tag/storage_wflw_gate_tag.hpp>
#include <ase/storage/components/tag/storage_wflw_pst_pend_tag.hpp>
#include <ase/storage/components/tag/storage_audt_pend_tag.hpp>
#include <ase/storage/storage_resource_manager.hpp>
// Workflow stage vocabulary (audit record, reason composer, stage ordinal) - shared with
// StorageWflwPermSystem and StorageWflwIniSystem since the split of 2026-08-29
#include <ase/storage/storage_wflw_stage.hpp>
#include <ase/storage/types.hpp>
// Hub API (perm read + verdict/stage publish)
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

void StorageWflwTranSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageWflwTran] Started");
}

void StorageWflwTranSystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    auto* mgr_ptr = registry.ctx().find<StorageResourceManager*>();
    if (!mgr_ptr || !(*mgr_ptr)) {
        return;  // manager not up yet (StorageIniSystem seeds it at Initialization)
    }
    auto& mgr = **mgr_ptr;

    // Deferred deletion: processed requests are collected, destroyed after the loop.
    ecs::Entity done[WFLW_REQ_BATCH];
    uint32_t done_n = 0;

    // Requests still carrying StorageWflwGateTag belong to StorageWflwGateSystem.
    auto* idx_ptr = registry.ctx().find<StorageAcssIndexResourceManager*>();
    if (!idx_ptr || !(*idx_ptr)) {
        // SCHEDULE_ORDER, woertlich: „System runs before its dependency". Der
        // freie String sagte dasselbe in Prosa. Ebene bleibt error, und das `return` darunter
        // ist unbedenklich: die ERR-Hilfetexte sind Diagnose-Checklisten („Check: 1) … 2) …"),
        // sie sagen — anders als die drei WRN-Texte — keine Korrektur des Wertes zu.
        // Der volle Klassenname wie in der bereits kategorisierten Zeile weiter unten.
        log::error(log::ERR::CAT::SCHEDULE_ORDER, "StorageWflwTranSystem",
                   "StorageAcssIndexResourceManager");
        return;
    }
    auto& idx = **idx_ptr;

    // Edge realm entity ref (rule scope + audit record). The answer does not depend on
    // the request, so it is resolved ONCE per tick instead of once per request. The tag
    // carries the identity (StorageEdgeIniSystem is its only producer), which replaces
    // the former scan over every realm with a string compare per request.
    uint32_t relm_ref = 0;
    for (auto relm_ent : registry.view<StorageStaRelmComponent, StorageRelmEdgeTag>()) {
        relm_ref = static_cast<uint32_t>(relm_ent);
        break;
    }

    auto req_view = registry.view<StorageReqWflwTranComponent, StorageWflwPendTag>(
        entt::exclude<StorageWflwGateTag>);
    for (auto [req_ent, req] : req_view.each()) {
        if (done_n >= WFLW_REQ_BATCH) break;

        const uint32_t owner = entt::hashed_string(req.path).value();
        const uint64_t now = mgr.get_wall_time_seconds();

        // DIE BERECHTIGUNG IST SEIT 2026-08-29 EIN EIGENES SYSTEM (StorageWflwPermSystem).
        //
        // "Darf dieser Anrufer befoerdern" ist eine A/ACS-Frage - dieselbe Familie wie die
        // Zugriffsleiter -, "ist dieser Uebergang im Graphen vorgesehen" eine Frage an den
        // Arbeitsablauf. Die erste hat eine eigene Quelle (die Keycard-Sitzung im Hub), eine
        // eigene Fehlerkategorie (ACCESS_DENIED) und einen eigenen Ausgang; sie lief hier als
        // Vorspann der zweiten, und damit war jede Aenderung am Graphen eine Aenderung an der
        // Datei, in der eine Sicherheitsentscheidung steht.
        //
        // Was hier ankommt, ist bereits berechtigt: das Berechtigungssystem schliesst seine
        // Ablehnungen im selben Tick ab, fail-closed, mit Pruefspur.

        // Locate the per-asset ACL rule (EXACT pattern match, realm-scoped). The
        // rule's label field IS the asset's current workflow stage.
        // Only the rules of THIS realm are examined - the realm filter is the bucket
        // itself, taken from the index StorageAcssIdxSystem rebuilt earlier this tick.
        // The former version walked every ACL rule in the registry once per request.
        ecs::Entity rule_ent_found = entt::null;
        const uint32_t req_path_hash = entt::hashed_string(req.path).value();
        const uint32_t rule_count = idx.get_rule_count(relm_ref);
        for (uint32_t rule_index = 0; rule_index < rule_count; ++rule_index) {
            const uint32_t rule_id = idx.get_rule(relm_ref, rule_index);
            if (rule_id == INVALID_ENTITY) continue;
            const auto rule_ent = static_cast<ecs::Entity>(rule_id);
            auto* r_idn = registry.try_get<StorageRuleIdnComponent>(rule_ent);
            if (r_idn == nullptr) continue;
            if (r_idn->pattern_hash != req_path_hash) continue;
            rule_ent_found = rule_ent;
            break;
        }

        // DIE ADOPTION IST SEIT 2026-08-29 EIN EIGENES SYSTEM (StorageWflwIniSystem).
        //
        // Ein Gut OHNE Regel ist der Entwurf: publish legt Builds direkt ins Revier, die erste
        // Beförderung nimmt sie auf. Das ist ein eigener Vorgang mit eigener Bedingung (liegt
        // die Datei ueberhaupt da?) und eigenem Ausgang (wflw_no_asset) - kein Sonderzweig des
        // Uebergangs. Er laeuft im selben Tick VOR diesem System.
        //
        // Fehlt die Regel hier dennoch, gab es nichts zu adoptieren: fail-closed, kein stiller
        // Uebergang auf ein Gut, das niemand kennt.
        if (rule_ent_found == entt::null) {
            hub::set(registry, owner, "STG_WFLW_RES"_hs, static_cast<float>(WFLW_RES_NOT_FOUND));
            log::error(log::ERR::CAT::RESOURCE_UNAVAIL, "StorageWflwTran", owner, req.path);
            done[done_n] = req_ent;
            ++done_n;
            continue;
        }

        auto& rule = registry.get<StorageAcssRuleComponent>(rule_ent_found);
        // Identity travels with the rule. A rule without it cannot be gated at all, so
        // this is an error the system reports rather than a case it works around.
        auto* rule_idn_ptr = registry.try_get<StorageRuleIdnComponent>(rule_ent_found);
        if (rule_idn_ptr == nullptr) {
            log::error(log::ERR::CAT::COMPONENT_MISSING, "StorageWflwTranSystem",
                       static_cast<uint32_t>(rule_ent_found), "StorageRuleIdnComponent");
            done[done_n] = req_ent;
            ++done_n;
            continue;
        }
        auto& rule_idn = *rule_idn_ptr;
        const uint32_t target_label_hash = entt::hashed_string(req.target_label).value();

        // Data-driven edge validation (die Kanten): allowed IFF a seeded edge
        // entity matches (from == rule.label && to == request.target). No switch,
        // no if-chain over labels — adding a transition = seeding one entity.
        // The edges are indexed by the label they lead AWAY from, so only the outgoing
        // transitions of the asset's current label are examined. The from_label is still
        // compared in full: the hash narrows the candidates, it never decides them.
        bool allowed = false;
        const uint32_t edge_count = idx.get_edge_count(rule_idn.label_hash);
        for (uint32_t edge_index = 0; edge_index < edge_count; ++edge_index) {
            const uint32_t edge_id = idx.get_edge(rule_idn.label_hash, edge_index);
            if (edge_id == INVALID_ENTITY) continue;
            auto* edge =
                registry.try_get<StorageWflwEdgeComponent>(static_cast<ecs::Entity>(edge_id));
            if (edge == nullptr) continue;
            if (edge->from_label_hash == rule_idn.label_hash &&
                edge->to_label_hash == target_label_hash) {
                allowed = true;
                break;
            }
        }

        char reason[MAX_REASON_LEN] = {};
        if (!allowed) {
            hub::set(registry, owner, "STG_WFLW_RES"_hs, static_cast<float>(WFLW_RES_DENIED_EDGE));
            compose_edge_reason(reason, MAX_REASON_LEN, "wflw_edge", rule.label, req.target_label);
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
            ase::utils::str_copy(outc.reason, MAX_REASON_LEN, reason);
            registry.emplace<StorageAudtPendTag>(aud_ent);
            // MIGRIERT, zusammen mit der PERM_PROMOTE-Zeile oben — beide in EINEM Schrieb, weil
            // das Tor das Dateiergebnis prueft und ein Teilschritt geblockt haette.
            //
            // ANDERE KATEGORIE ALS OBEN, und das ist der Punkt: dort fehlte dem Anfragenden ein
            // RECHT, hier ist die verlangte KANTE nicht vorgesehen. Der Anfragende darf
            // promoten; er hat nur einen Uebergang benannt, den der Graph nicht kennt. Das ist
            // INPUT_REJECTED — ein fremder Aufrufer hat etwas Unbedienbares geschickt — und die
            // ERR-Haelfte davon, weil der Vorgang hier endet statt weiterzulaufen. Wer
            // Ablehnungen im Fehlerstrom auswertet, findet beide Sorten dort und kann sie an
            // der Kategorie AUSEINANDERHALTEN; ein gemeinsames "DENIED" konnte das nie.
            //
            // WORK_PRECLUDED waere die Alternative gewesen (der Zustand erlaubt die Arbeit
            // nicht) und ist bewusst nicht gewaehlt: der Zustand des Assets ist in Ordnung, es
            // ist die ANFRAGE, die ein unmoegliches Ziel nennt. `reason` traegt bereits
            // "wflw_edge(von->nach)" und damit beide Label exakt; der detail haengt den
            // Anfragenden an, value_id fuehrt den Pfad, owner seinen Hash. Alle vier bleiben.
            char edge_detail[256] = {};
            ase::utils::str_copy(edge_detail, 256, reason);
            ase::utils::str_append(edge_detail, 256, " requester=");
            ase::utils::str_append(edge_detail, 256, req.requested_by);
            log::error(log::ERR::CAT::INPUT_REJECTED, "StorageWflwTran", owner, req.path,
                       edge_detail);
            done[done_n] = req_ent;
            ++done_n;
            continue;
        }

        // APPLY: write the new label, publish verdict + stage, audit with the
        // keycard attribution, stage the durable frame-112 persist buffer.
        compose_edge_reason(reason, MAX_REASON_LEN, "wflw", rule.label, req.target_label);
        ase::utils::str_copy(rule.label, MAX_LABEL_LEN, req.target_label);
        // The label MOVED, so its identity moves with it. Updating the record and
        // leaving the hash standing would make the rule answer the OLD label at every
        // downstream gate: the asset would sit at "released" and still be gated as draft.
        rule_idn.label_hash = target_label_hash;

        hub::set(registry, owner, "STG_WFLW_RES"_hs, static_cast<float>(WFLW_RES_APPLIED));
        hub::set(registry, owner, "STG_WFLW_STAGE"_hs, stage_ordinal(rule_idn.label_hash));
        // Public-servable verdict — the SINGLE place this policy lives (ARCH_ASE_REASONING_EDGE Section 6.4:
        // download-access runs over the ase-storage A/ACS infrastructure). A customer download is
        // public ONLY at the released stage (EDGE_LABEL_RELEASED = "Public download", types.hpp). The
        // edge-webserver serving gate READS this boolean; it never re-decides the label policy itself.
        hub::set(registry, owner, "STG_WFLW_PUB"_hs,
                 rule_idn.label_hash == EDGE_LABEL_RELEASED_HASH ? 1.0f : 0.0f);

        // EINE Audit-Entity je Entscheidung, mit StorageAudtPendTag fuer den
        // Preservation-Stapelschreiber — hier der GEWAEHRUNGSpfad, dieselbe Form wie die
        // Ablehnung oben und aus demselben Grund: eine Entscheidung ohne Spur ist von einer
        // ausgebliebenen nicht zu unterscheiden. proj_ref bleibt 0, action bleibt AUD_PROMOTE.
        auto aud_ent = registry.create();
        auto& aud = registry.emplace<StorageBufAudtComponent>(aud_ent);
        aud.relm_ref = relm_ref;
        aud.proj_ref = 0;
        ase::utils::str_copy(aud.user_id, MAX_OWNER_ID, req.requested_by);
        ase::utils::str_copy(aud.path, MAX_PATH_LEN, req.path);
        aud.timestamp = now;
        auto& outc = registry.emplace<StorageAudtOutcComponent>(aud_ent);
        outc.action = AUD_PROMOTE;
        outc.result = AUD_GRANTED;
        ase::utils::str_copy(outc.reason, MAX_REASON_LEN, reason);
        registry.emplace<StorageAudtPendTag>(aud_ent);

        auto buf_ent = registry.create();
        auto& buf = registry.emplace<StorageBufWflwComponent>(buf_ent);
        ase::utils::str_copy(buf.realm, MAX_REALM_ID, EDGE_REALM_ID);
        ase::utils::str_copy(buf.path, MAX_PATH_LEN, req.path);
        ase::utils::str_copy(buf.label, MAX_LABEL_LEN, rule.label);
        ase::utils::str_copy(buf.updated_by, MAX_OWNER_ID, req.requested_by);
        buf.updated_at = now;
        registry.emplace<StorageWflwPstPendTag>(buf_ent);

        // A transition INTO retired starts the retention clock: one record entity
        // per retired build (Entity-per-Item); StorageWflwClnSystem deletes the
        // files once WFLW_RETIRED_RETENTION_S elapses.
        if (rule_idn.label_hash == EDGE_LABEL_RETIRED_HASH) {
            auto retr_ent = registry.create();
            auto& retr = registry.emplace<StorageWflwRetrComponent>(retr_ent);
            retr.rule_ref = static_cast<uint32_t>(rule_ent_found);
            ase::utils::str_copy(retr.path, MAX_PATH_LEN, req.path);
            retr.retired_at = now;
            registry.emplace<StorageWflwRetrTag>(retr_ent);
        }

        log::info("[StorageWflwTran] APPLIED {} for {} (by {})", reason, req.path, req.requested_by);

        done[done_n] = req_ent;
        ++done_n;
    }

    for (uint32_t i = 0; i < done_n; ++i) {
        registry.destroy(done[i]);
    }
}

void StorageWflwTranSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageWflwTran] Stopped");
}

}  // namespace ase::storage
