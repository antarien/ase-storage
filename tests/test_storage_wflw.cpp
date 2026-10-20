#include <doctest/doctest.h>

/**
 * ASE Storage — Workflow-Transition Tests (doctest, Phase 12 Task 12.3)
 *
 * Drives the REAL StorageWflwTranSystem over the data-driven transition graph
 * (die Kanten): the allowed edges draft→review→approved→released→retired are
 * seeded exactly as StorageWflwEdgeIniSystem seeds them, requests are the same
 * StorageReqWflwTranComponent + StorageWflwPendTag entities StorageWflwStgeSystem
 * stages, and every verdict is asserted on the REAL side effects — the ACL rule
 * label, the owner-scoped STG_WFLW_RES Hub verdict, the attributed audit entity
 * (AUD_PROMOTE + requester user_id) and the frame-112 persist buffer. No
 * filesystem and no wire are needed: the rules are pre-seeded, so the draft
 * bootstrap (the only file_exists caller) never runs.
 */

#include <ase/storage/storage_module.hpp>
#include <ase/storage/systems/workflow/storage_wflw_tran_sys.hpp>
#include <ase/storage/systems/workflow/storage_wflw_drn_sys.hpp>
#include <ase/storage/systems/workflow/storage_wflw_stge_sys.hpp>
#include <ase/storage/components/tag/storage_wflw_rcv_tag.hpp>
#include <ase/storage/systems/workflow/storage_wflw_perm_sys.hpp>
#include <ase/storage/components/request/storage_req_wflw_tran_comp.hpp>
#include <ase/storage/components/state/storage_wflw_edge_comp.hpp>
#include <ase/storage/components/state/storage_acss_rule_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/state/storage_relm_quot_comp.hpp>
#include <ase/storage/components/state/storage_relm_idn_comp.hpp>
#include <ase/storage/components/state/storage_rule_idn_comp.hpp>
#include <ase/storage/components/tag/storage_relm_edge_tag.hpp>
#include <ase/storage/systems/acl/storage_acss_idx_sys.hpp>
#include <ase/storage/components/state/storage_buf_audt_comp.hpp>
#include <ase/storage/components/state/storage_audt_outc_comp.hpp>
#include <ase/storage/components/state/storage_buf_wflw_comp.hpp>
#include <ase/storage/components/tag/storage_wflw_pend_tag.hpp>
#include <ase/storage/components/tag/storage_wflw_gate_tag.hpp>
#include <ase/storage/components/tag/storage_wflw_pst_pend_tag.hpp>
#include <ase/storage/components/tag/storage_audt_pend_tag.hpp>
#include <ase/storage/components/state/storage_wflw_retr_comp.hpp>
#include <ase/storage/components/tag/storage_wflw_retr_tag.hpp>
#include <ase/storage/systems/workflow/storage_wflw_cln_sys.hpp>
#include <ase/storage/storage_resource_manager.hpp>
#include <ase/storage/types.hpp>
#include <ase/hub/api.hpp>
#include <ase/utils/strops.hpp>
#include <ase/ecs/app.hpp>
#include <entt/core/hashed_string.hpp>
#include <cstdint>
#include <cstring>

using namespace ase::ecs;
using namespace ase::storage;
using namespace entt::literals;

namespace {

constexpr const char* kAsset    = "release/linux-x86_64/ase-edge-daemon-test";
constexpr const char* kOperator = "op-user-1";

// Seed the fixed release-pipeline graph exactly as StorageWflwEdgeIniSystem does.
void seed_edges(Registry& reg) {
    const char* chain_from[4] = {EDGE_LABEL_DRAFT, EDGE_LABEL_REVIEW,
                                 EDGE_LABEL_APPROVED, EDGE_LABEL_RELEASED};
    const char* chain_to[4]   = {EDGE_LABEL_REVIEW, EDGE_LABEL_APPROVED,
                                 EDGE_LABEL_RELEASED, EDGE_LABEL_RETIRED};
    for (uint32_t i = 0; i < 4; ++i) {
        auto e = reg.create();
        auto& edge = reg.emplace<StorageWflwEdgeComponent>(e);
        ase::utils::str_copy(edge.from_label, MAX_LABEL_LEN, chain_from[i]);
        edge.from_label_hash = entt::hashed_string(chain_from[i]).value();
        ase::utils::str_copy(edge.to_label, MAX_LABEL_LEN, chain_to[i]);
        edge.to_label_hash = entt::hashed_string(chain_to[i]).value();
    }
}

// Edge realm + per-asset rule at the given starting label; returns the rule entity.
// Seeded EXACTLY as production does it - the realm and its tag by StorageEdgeIniSystem,
// the per-asset rule by StorageAcssEdgeIniSystem: the identity hashes and the edge
// realm tag are part of the production shape, not extras. A realm without the tag is
// invisible to the transition system, and a rule without its identity component cannot
// be gated - the test would then assert against a system that never saw its data.
Entity seed_realm_and_rule(Registry& reg, const char* start_label) {
    auto relm_ent = reg.create();
    auto& relm = reg.emplace<StorageStaRelmComponent>(relm_ent);
    ase::utils::str_copy(relm.id, MAX_REALM_ID, EDGE_REALM_ID);
    // The accounting row belongs to every realm, seeded or not - the quota scan
    // joins on it and a seeded realm without it would silently leave the view.
    reg.emplace<StorageRelmQuotComponent>(relm_ent);
    auto& relm_idn = reg.emplace<StorageRelmIdnComponent>(relm_ent);
    relm_idn.id_hash = EDGE_REALM_HASH;
    reg.emplace<StorageRelmEdgeTag>(relm_ent);

    auto rule_ent = reg.create();
    auto& rule = reg.emplace<StorageAcssRuleComponent>(rule_ent);
    rule.relm_ref = static_cast<uint32_t>(relm_ent);
    rule.proj_ref = 0;
    ase::utils::str_copy(rule.path_pattern, MAX_PATH_LEN, kAsset);
    rule.protection_level = PROTECTION_PUBLIC;
    ase::utils::str_copy(rule.label, MAX_LABEL_LEN, start_label);
    auto& rule_idn = reg.emplace<StorageRuleIdnComponent>(rule_ent);
    // The asset pattern carries no wildcard, so pattern and literal match coincide.
    rule_idn.pattern_hash = entt::hashed_string(kAsset).value();
    rule_idn.match_hash = rule_idn.pattern_hash;
    rule_idn.match_len = ase::utils::str_len(kAsset, MAX_PATH_LEN);
    rule_idn.label_hash = entt::hashed_string(start_label).value();
    return rule_ent;
}

// Stage a promote request exactly as StorageWflwStgeSystem does (sans gate tag —
// these tests exercise the EDGE validation, not the released artifact gate).
Entity stage_request(Registry& reg, const char* target) {
    auto req_ent = reg.create();
    auto& req = reg.emplace<StorageReqWflwTranComponent>(req_ent);
    ase::utils::str_copy(req.path, MAX_PATH_LEN, kAsset);
    ase::utils::str_copy(req.target_label, MAX_LABEL_LEN, target);
    ase::utils::str_copy(req.requested_by, MAX_OWNER_ID, kOperator);
    reg.emplace<StorageWflwPendTag>(req_ent);
    return req_ent;
}

// Grant (or narrow) the PERM session axis for the test operator.
void grant_promote(Registry& reg, uint16_t perm) {
    const uint32_t owner = entt::hashed_string(kOperator).value();
    ase::hub::set(reg, owner, "SES_KYCD_PERM"_hs, static_cast<float>(perm));
}

// The contract widths of HUB_STG_WFLW_REQ as the hub loader stands them from hub_constants.json; this
// test does not run the loader, so it stands the values itself (usecase:100329).
void stand_wflw_widths(Registry& reg) {
    ase::hub::set(reg, ase::hub::GLOBAL, "STG_CONST_WFLW_PATH_BYTES"_hs, 255.0f);
    ase::hub::set(reg, ase::hub::GLOBAL, "STG_CONST_WFLW_LABEL_BYTES"_hs, 31.0f);
    ase::hub::set(reg, ase::hub::GLOBAL, "STG_CONST_WFLW_REQUESTER_BYTES"_hs, 63.0f);
}

// A promote row as a plugin files it with the general SDK blocks (row_create, set_bytes at each
// string's own length); here reached as hub::row_create and hub::set_bytes.
Entity file_promote(Registry& reg, const char* path, uint32_t path_len, const char* label) {
    const auto bridge = ase::hub::row_create(reg, "HUB_STG_WFLW_REQ"_hs);
    const uint32_t row = static_cast<uint32_t>(bridge);
    ase::hub::set_bytes(reg, row, "HUB_STG_WFLW_REQ_PATH"_hs, path, path_len);
    ase::hub::set_bytes(reg, row, "HUB_STG_WFLW_REQ_TARGET_LABEL"_hs, label,
                        ase::utils::str_len(label, MAX_LABEL_LEN));
    ase::hub::set_bytes(reg, row, "HUB_STG_WFLW_REQ_REQUESTED_BY"_hs, kOperator,
                        ase::utils::str_len(kOperator, MAX_OWNER_ID));
    return bridge;
}

// DIE ANWESENHEIT WIRD HIER ZUGESICHERT UND NICHT AN DIE AUFRUFER DELEGIERT. Bei sdk::get darf
// keine Pruefung stehen, weil der AUFRUFER weiss, ob ein fehlender Wert ein Fehler ist; eine
// Pruefung dort naehme jedem Aufrufer den Rohwert weg. Hier ist die Lage umgekehrt: jede
// Aufrufstelle von read_verdict vergleicht gegen ein erwartetes Urteil, keine haelt sein Fehlen
// fuer zulaessig, und -1.0f kommt in der ganzen Datei nur in dieser einen Zeile vor.
//
// WAS DIE ZEILE VERHINDERT: ohne sie ist "das System hat ein falsches Urteil veroeffentlicht"
// von "das System hat ueberhaupt nichts veroeffentlicht" nicht zu unterscheiden — beide enden
// im selben fehlgeschlagenen Vergleich, und die zweite Ursache liegt woanders als die erste.
float read_verdict(Registry& reg) {
    const uint32_t owner = entt::hashed_string(kAsset).value();
    const float verdict = ase::hub::get(reg, owner, "STG_WFLW_RES"_hs, -1.0f);
    REQUIRE(ase::hub::is_measured(reg, owner, "STG_WFLW_RES"_hs));
    return verdict;
}

}  // namespace

// DER VERWALTER LIEGT IN ALLEN SECHS FAELLEN AUF DEM STAPEL, der ctx bekommt seine ADRESSE.
// Diese Begruendung gilt fuer die ganze Datei und steht deshalb einmal hier.
//
// Der ctx nimmt einen ZEIGER (StorageResourceManager*), so wie der Produktivcode ihn erwartet;
// den Besitz haelt der Stapel. Ein Stapelobjekt raeumt auf jedem Weg aus dem Block auf, auch
// dann, wenn ein REQUIRE den Fall vorzeitig abbricht.
//
// ZUR LEBENSDAUER, weil die Reihenfolge hier zaehlt: `mgr` steht NACH `App app;`, wird also VOR
// ihm zerstoert. Der ctx haelt danach einen ungueltigen Zeiger — unbedenklich, weil ein roher
// Zeiger beim Zerstoeren nicht dereferenziert wird und die Systeme zu diesem Zeitpunkt ueber
// app.shutdown() bereits stehen. Wer hier etwas einbaut, das den Zeiger IM Destruktor von App
// liest, muss `mgr` vor `app` deklarieren.
//
// DIE KETTE IST DIE DER PRODUKTION, in allen vier Kettenfaellen: Index, Rechtetor, Uebergang.
// Der Index ist Produktionsverdrahtung und kein Geruest: StorageWflwTranSystem liest seine
// Realm-Regeln aus ihm, ohne ihn liefe ein System, das nichts findet, und der Fall bestuende oder
// scheiterte aus dem falschen Grund. Das Rechtetor StorageWflwPermSystem prueft die
// PERM_PROMOTE-Achse der Keycard-Sitzung, schliesst seine Ablehnungen im selben Tick ab und laesst
// nur Berechtigte weiter; der Uebergang setzt voraus, dass, was bei ihm ankommt, berechtigt ist,
// und storage_module.hpp verdrahtet die Kette Perm → Ini → Tran. Ohne das Tor waere
// `grant_promote()` in jedem Fall wirkungslos, und die Zusage „ohne PERM_PROMOTE wird
// fail-closed abgelehnt“ wuerde von niemandem geprueft: der Uebergang wendete dann nach seinem
// eigenen Vertrag korrekt an, und STG_WFLW_RES stuende auf WFLW_RES_APPLIED statt
// WFLW_RES_DENIED_PERM. Ein Test, der die Produktionskette nur zur Haelfte aufbaut, misst eine
// Ordnung, die es nicht gibt.
TEST_CASE("workflow edges: allowed transition applies label + attributed audit + persist buffer") {
    App app;
    app.set_source("ase-storage");
    app.add_system<StorageAcssIdxSystem>(Schedule::Integration);
    app.add_system_with<StorageWflwPermSystem>(Schedule::Integration)
        .run_after("StorageAcssIdxSystem");
    app.add_system_with<StorageWflwTranSystem>(Schedule::Integration)
        .run_after("StorageWflwPermSystem");
    app.startup();
    auto& reg = app.registry();
    StorageResourceManager mgr;
    reg.ctx().emplace<StorageResourceManager*>(&mgr);

    seed_edges(reg);
    auto rule_ent = seed_realm_and_rule(reg, EDGE_LABEL_DRAFT);
    grant_promote(reg, static_cast<uint16_t>(PERM_READ | PERM_PROMOTE));
    stage_request(reg, EDGE_LABEL_REVIEW);

    app.run_schedule(Schedule::Integration, 1.0f);

    // The rule label transitioned draft → review.
    auto& rule = reg.get<StorageAcssRuleComponent>(rule_ent);
    CHECK(ase::utils::str_equal(rule.label, EDGE_LABEL_REVIEW, MAX_LABEL_LEN));
    // Verdict published owner-scoped.
    CHECK(read_verdict(reg) == static_cast<float>(WFLW_RES_APPLIED));
    // Request consumed (deferred destroy).
    CHECK(reg.view<StorageReqWflwTranComponent>().size() == 0);
    // Attributed audit entity: AUD_PROMOTE + GRANTED + the operator user_id.
    uint32_t granted_audits = 0;
    for (auto [e, aud, outc] :
         reg.view<StorageBufAudtComponent, StorageAudtOutcComponent, StorageAudtPendTag>()
             .each()) {
        if (outc.action != AUD_PROMOTE) continue;
        if (outc.result != AUD_GRANTED) continue;
        if (!ase::utils::str_equal(aud.user_id, kOperator, MAX_OWNER_ID)) continue;
        if (!ase::utils::str_equal(aud.path, kAsset, MAX_PATH_LEN)) continue;
        ++granted_audits;
    }
    CHECK(granted_audits == 1);
    // Frame-112 persist buffer staged with the new label.
    uint32_t persist_buffers = 0;
    for (auto [e, buf] : reg.view<StorageBufWflwComponent, StorageWflwPstPendTag>().each()) {
        CHECK(ase::utils::str_equal(buf.label, EDGE_LABEL_REVIEW, MAX_LABEL_LEN));
        CHECK(ase::utils::str_equal(buf.updated_by, kOperator, MAX_OWNER_ID));
        ++persist_buffers;
    }
    CHECK(persist_buffers == 1);

    app.shutdown();
}

TEST_CASE("workflow edges: forbidden edge (draft to released) is denied, label untouched") {
    App app;
    app.set_source("ase-storage");
    app.add_system<StorageAcssIdxSystem>(Schedule::Integration);
    app.add_system_with<StorageWflwPermSystem>(Schedule::Integration)
        .run_after("StorageAcssIdxSystem");
    app.add_system_with<StorageWflwTranSystem>(Schedule::Integration)
        .run_after("StorageWflwPermSystem");
    app.startup();
    auto& reg = app.registry();
    StorageResourceManager mgr;
    reg.ctx().emplace<StorageResourceManager*>(&mgr);

    seed_edges(reg);
    auto rule_ent = seed_realm_and_rule(reg, EDGE_LABEL_DRAFT);
    grant_promote(reg, static_cast<uint16_t>(PERM_READ | PERM_PROMOTE));
    stage_request(reg, EDGE_LABEL_RELEASED);  // draft → released is NOT a seeded edge

    app.run_schedule(Schedule::Integration, 1.0f);

    auto& rule = reg.get<StorageAcssRuleComponent>(rule_ent);
    CHECK(ase::utils::str_equal(rule.label, EDGE_LABEL_DRAFT, MAX_LABEL_LEN));
    CHECK(read_verdict(reg) == static_cast<float>(WFLW_RES_DENIED_EDGE));
    CHECK(reg.view<StorageReqWflwTranComponent>().size() == 0);
    // The deny is audited with the edge named in the reason.
    uint32_t denied_audits = 0;
    for (auto [e, aud, outc] :
         reg.view<StorageBufAudtComponent, StorageAudtOutcComponent, StorageAudtPendTag>()
             .each()) {
        (void)aud;
        if (outc.result != AUD_DENIED) continue;
        CHECK(std::strncmp(outc.reason, "wflw_edge(", 10) == 0);
        ++denied_audits;
    }
    CHECK(denied_audits == 1);
    // NO persist buffer for a denied transition.
    CHECK(reg.view<StorageBufWflwComponent>().size() == 0);

    app.shutdown();
}

TEST_CASE("workflow edges: requester without PERM_PROMOTE is denied fail-closed") {
    App app;
    app.set_source("ase-storage");
    app.add_system<StorageAcssIdxSystem>(Schedule::Integration);
    app.add_system_with<StorageWflwPermSystem>(Schedule::Integration)
        .run_after("StorageAcssIdxSystem");
    app.add_system_with<StorageWflwTranSystem>(Schedule::Integration)
        .run_after("StorageWflwPermSystem");
    app.startup();
    auto& reg = app.registry();
    StorageResourceManager mgr;
    reg.ctx().emplace<StorageResourceManager*>(&mgr);

    seed_edges(reg);
    auto rule_ent = seed_realm_and_rule(reg, EDGE_LABEL_DRAFT);
    grant_promote(reg, PERM_READ);  // holds READ, lacks PROMOTE
    stage_request(reg, EDGE_LABEL_REVIEW);

    app.run_schedule(Schedule::Integration, 1.0f);

    auto& rule = reg.get<StorageAcssRuleComponent>(rule_ent);
    CHECK(ase::utils::str_equal(rule.label, EDGE_LABEL_DRAFT, MAX_LABEL_LEN));
    CHECK(read_verdict(reg) == static_cast<float>(WFLW_RES_DENIED_PERM));
    CHECK(reg.view<StorageReqWflwTranComponent>().size() == 0);

    app.shutdown();
}

TEST_CASE("workflow edges: full chain draft to review to approved to released to retired") {
    App app;
    app.set_source("ase-storage");
    app.add_system<StorageAcssIdxSystem>(Schedule::Integration);
    app.add_system_with<StorageWflwPermSystem>(Schedule::Integration)
        .run_after("StorageAcssIdxSystem");
    app.add_system_with<StorageWflwTranSystem>(Schedule::Integration)
        .run_after("StorageWflwPermSystem");
    app.startup();
    auto& reg = app.registry();
    StorageResourceManager mgr;
    reg.ctx().emplace<StorageResourceManager*>(&mgr);

    seed_edges(reg);
    auto rule_ent = seed_realm_and_rule(reg, EDGE_LABEL_DRAFT);
    grant_promote(reg, static_cast<uint16_t>(PERM_READ | PERM_PROMOTE));

    const char* chain[4] = {EDGE_LABEL_REVIEW, EDGE_LABEL_APPROVED,
                            EDGE_LABEL_RELEASED, EDGE_LABEL_RETIRED};
    for (uint32_t i = 0; i < 4; ++i) {
        stage_request(reg, chain[i]);
        app.run_schedule(Schedule::Integration, 1.0f);
        auto& rule = reg.get<StorageAcssRuleComponent>(rule_ent);
        CHECK(ase::utils::str_equal(rule.label, chain[i], MAX_LABEL_LEN));
        CHECK(read_verdict(reg) == static_cast<float>(WFLW_RES_APPLIED));
    }

    // Four applied transitions yield four attributed persist buffers staged.
    CHECK(reg.view<StorageBufWflwComponent>().size() == 4);

    app.shutdown();
}

TEST_CASE("workflow retention: retired build older than 90 days is swept with rule + AUD_DELETE") {
    App app;
    app.set_source("ase-storage");
    app.add_system<StorageWflwClnSystem>(Schedule::Observation);
    app.startup();
    auto& reg = app.registry();
    StorageResourceManager mgr;
    reg.ctx().emplace<StorageResourceManager*>(&mgr);

    // Edge realm (audit ref) plus the retiring ACL rule that dies with the build.
    auto rule_ent = seed_realm_and_rule(reg, EDGE_LABEL_RETIRED);

    // get_wall_time_seconds is time(nullptr); guard against a zero clock so the
    // subtraction below stays a real past instant, never an unsigned wrap.
    const uint64_t now = mgr.get_wall_time_seconds();
    REQUIRE(now > WFLW_RETIRED_RETENTION_S);

    // One retired record, retired well before the 90-day window closed. No file is
    // written: remove_if_present treats a missing target as already-gone, so the
    // record + rule sweep and the AUD_DELETE audit are exercised without disk I/O.
    auto retr_ent = reg.create();
    auto& retr = reg.emplace<StorageWflwRetrComponent>(retr_ent);
    ase::utils::str_copy(retr.path, MAX_PATH_LEN, kAsset);
    retr.rule_ref = static_cast<uint32_t>(rule_ent);
    const uint64_t retired_long_ago = now - WFLW_RETIRED_RETENTION_S - 100;
    retr.retired_at = retired_long_ago;
    reg.emplace<StorageWflwRetrTag>(retr_ent);

    app.run_schedule(Schedule::Observation, 1.0f);

    // Record swept and the retiring rule destroyed with it.
    CHECK(reg.view<StorageWflwRetrComponent>().size() == 0);
    CHECK(!reg.valid(rule_ent));
    // Exactly one AUD_DELETE audit, path-attributed, carrying the retention reason.
    uint32_t del_audits = 0;
    for (auto [e, aud, outc] :
         reg.view<StorageBufAudtComponent, StorageAudtOutcComponent, StorageAudtPendTag>()
             .each()) {
        if (outc.action != AUD_DELETE) continue;
        CHECK(std::strncmp(outc.reason, "wflw_retention", 14) == 0);
        CHECK(ase::utils::str_equal(aud.path, kAsset, MAX_PATH_LEN));
        ++del_audits;
    }
    CHECK(del_audits == 1);

    app.shutdown();
}

TEST_CASE("workflow retention: retired build within 90 days is kept, not swept") {
    App app;
    app.set_source("ase-storage");
    app.add_system<StorageWflwClnSystem>(Schedule::Observation);
    app.startup();
    auto& reg = app.registry();
    StorageResourceManager mgr;
    reg.ctx().emplace<StorageResourceManager*>(&mgr);

    auto rule_ent = seed_realm_and_rule(reg, EDGE_LABEL_RETIRED);
    const uint64_t now = mgr.get_wall_time_seconds();
    REQUIRE(now > WFLW_RETIRED_RETENTION_S);

    // Just retired, deep inside the retention window.
    auto retr_ent = reg.create();
    auto& retr = reg.emplace<StorageWflwRetrComponent>(retr_ent);
    ase::utils::str_copy(retr.path, MAX_PATH_LEN, kAsset);
    retr.rule_ref = static_cast<uint32_t>(rule_ent);
    const uint64_t retired_just_now = now - 100;
    retr.retired_at = retired_just_now;
    reg.emplace<StorageWflwRetrTag>(retr_ent);

    app.run_schedule(Schedule::Observation, 1.0f);

    // Retained: record + rule alive, and no delete was audited.
    CHECK(reg.view<StorageWflwRetrComponent>().size() == 1);
    CHECK(reg.valid(rule_ent));
    uint32_t del_audits = 0;
    // A zero here would also come out of an empty view, so this check alone cannot
    // tell "nothing was deleted" from "nobody emplaces the verdict row any more".
    // The three positive counts above run against the same pairing and go red first.
    for (auto [e, aud, outc] :
         reg.view<StorageBufAudtComponent, StorageAudtOutcComponent, StorageAudtPendTag>()
             .each()) {
        (void)aud;
        if (outc.action == AUD_DELETE) ++del_audits;
    }
    CHECK(del_audits == 0);

    app.shutdown();
}

// usecase:100329 - stage workflow promotion. The row below is the one a plugin files through the
// general SDK blocks, sdk::row_create and sdk::set_bytes (the contract of HUB_STG_WFLW_REQ in
// modules/ase-hub/data/hub_rows.json): the three strings as byte fields at their own length
// (register A0902). Inside this L3 test the same blocks are reached as hub::row_create and
// hub::set_bytes.
TEST_CASE("the drain takes a filed promote row into a received request the staging marks pending") {
    Registry reg;
    StorageWflwDrnSystem drn;
    StorageWflwStgeSystem stge;
    stand_wflw_widths(reg);
    file_promote(reg, kAsset, ase::utils::str_len(kAsset, MAX_PATH_LEN), EDGE_LABEL_RELEASED);

    drn.tick(reg, 0.0f);

    // The row is taken and the request stands received with all three strings; nothing is
    // staged and no verdict is published before the staging runs.
    CHECK(ase::hub::row_count(reg, "HUB_STG_WFLW_REQ"_hs) == 0u);
    CHECK(reg.view<StorageReqWflwTranComponent, StorageWflwRcvTag>().size_hint() == 1u);
    CHECK(reg.view<StorageReqWflwTranComponent, StorageWflwPendTag>().size_hint() == 0u);
    CHECK_FALSE(ase::hub::is_measured(reg, entt::hashed_string(kAsset).value(), "STG_WFLW_RES"_hs));

    stge.tick(reg, 0.0f);

    // Staged: pending, no longer received, and the released target carries the gate tag the
    // transition system leaves to StorageWflwGateSystem.
    uint32_t staged = 0u;
    for (auto [e, req] : reg.view<StorageReqWflwTranComponent, StorageWflwPendTag>().each()) {
        ++staged;
        CHECK(std::strcmp(req.path, kAsset) == 0);
        CHECK(std::strcmp(req.target_label, EDGE_LABEL_RELEASED) == 0);
        CHECK(std::strcmp(req.requested_by, kOperator) == 0);
        CHECK(reg.all_of<StorageWflwGateTag>(e));
        CHECK_FALSE(reg.all_of<StorageWflwRcvTag>(e));
    }
    CHECK(staged == 1u);
    CHECK(read_verdict(reg) == doctest::Approx(static_cast<float>(WFLW_RES_PENDING)));
}

TEST_CASE("a request the frame-113 door files is staged the same way, without a released gate") {
    // usecase:100329 - StorageEdgeWflwFwdRcvSystem files the request component marked received;
    // the staging is the one the hub-row door feeds. A review target takes no gate tag.
    Registry reg;
    StorageWflwStgeSystem stge;
    auto req_ent = reg.create();
    auto& req = reg.emplace<StorageReqWflwTranComponent>(req_ent);
    ase::utils::str_copy(req.path, MAX_PATH_LEN, kAsset);
    ase::utils::str_copy(req.target_label, MAX_LABEL_LEN, EDGE_LABEL_REVIEW);
    ase::utils::str_copy(req.requested_by, MAX_OWNER_ID, kOperator);
    reg.emplace<StorageWflwRcvTag>(req_ent);
    // A request already pending is no longer received and stays untouched.
    const auto pending = stage_request(reg, EDGE_LABEL_APPROVED);

    stge.tick(reg, 0.0f);

    CHECK(reg.all_of<StorageWflwPendTag>(req_ent));
    CHECK_FALSE(reg.all_of<StorageWflwRcvTag>(req_ent));
    CHECK_FALSE(reg.all_of<StorageWflwGateTag>(req_ent));
    CHECK(reg.all_of<StorageWflwPendTag>(pending));
    CHECK(reg.view<StorageReqWflwTranComponent, StorageWflwPendTag>().size_hint() == 2u);
    CHECK(read_verdict(reg) == doctest::Approx(static_cast<float>(WFLW_RES_PENDING)));
}

TEST_CASE("the drain refuses a promote row without its requester and releases it unstaged") {
    // usecase:100329 - a row the contract of HUB_STG_WFLW_REQ forbids a plugin to file: the
    // requester field is missing.
    Registry reg;
    StorageWflwDrnSystem drn;
    stand_wflw_widths(reg);
    const auto bridge = ase::hub::row_create(reg, "HUB_STG_WFLW_REQ"_hs);
    const uint32_t row = static_cast<uint32_t>(bridge);
    ase::hub::set_bytes(reg, row, "HUB_STG_WFLW_REQ_PATH"_hs, kAsset,
                        ase::utils::str_len(kAsset, MAX_PATH_LEN));
    ase::hub::set_bytes(reg, row, "HUB_STG_WFLW_REQ_TARGET_LABEL"_hs, EDGE_LABEL_REVIEW,
                        ase::utils::str_len(EDGE_LABEL_REVIEW, MAX_LABEL_LEN));

    drn.tick(reg, 0.0f);

    CHECK(ase::hub::row_count(reg, "HUB_STG_WFLW_REQ"_hs) == 0u);
    CHECK(reg.view<StorageReqWflwTranComponent>().size() == 0u);
    CHECK_FALSE(ase::hub::is_measured(reg, entt::hashed_string(kAsset).value(), "STG_WFLW_RES"_hs));
}

TEST_CASE("the drain refuses a promote path longer than the contract's width and releases it unstaged") {
    // usecase:100329 - STG_CONST_WFLW_PATH_BYTES is the contract's width; a row past it is one a
    // plugin must refuse to file, and the owner refuses it the same way instead of cutting it.
    Registry reg;
    StorageWflwDrnSystem drn;
    stand_wflw_widths(reg);
    ase::hub::set(reg, ase::hub::GLOBAL, "STG_CONST_WFLW_PATH_BYTES"_hs, 8.0f);
    file_promote(reg, kAsset, ase::utils::str_len(kAsset, MAX_PATH_LEN), EDGE_LABEL_REVIEW);

    drn.tick(reg, 0.0f);

    // RED when: a path past the contract's width is staged, cut or whole.
    CHECK(ase::hub::row_count(reg, "HUB_STG_WFLW_REQ"_hs) == 0u);
    CHECK(reg.view<StorageReqWflwTranComponent>().size() == 0u);
}

TEST_CASE("without the contract's widths the drain leaves every promote row waiting") {
    // usecase:100329 - the widths come from hub_constants.json; without them no row can be judged
    // against its bound, so nothing is drained, refused or staged until they stand.
    Registry reg;
    StorageWflwDrnSystem drn;
    file_promote(reg, kAsset, ase::utils::str_len(kAsset, MAX_PATH_LEN), EDGE_LABEL_REVIEW);

    drn.tick(reg, 0.0f);

    // RED when: a row is judged against a bound that is not there.
    CHECK(ase::hub::row_count(reg, "HUB_STG_WFLW_REQ"_hs) == 1u);
    CHECK(reg.view<StorageReqWflwTranComponent>().size() == 0u);

    // A width the request buffer cannot hold with its NUL is a broken contract: the rows still wait.
    stand_wflw_widths(reg);
    ase::hub::set(reg, ase::hub::GLOBAL, "STG_CONST_WFLW_LABEL_BYTES"_hs, static_cast<float>(MAX_LABEL_LEN));
    drn.tick(reg, 0.0f);
    CHECK(ase::hub::row_count(reg, "HUB_STG_WFLW_REQ"_hs) == 1u);

    stand_wflw_widths(reg);
    drn.tick(reg, 0.0f);
    CHECK(ase::hub::row_count(reg, "HUB_STG_WFLW_REQ"_hs) == 0u);
    CHECK(reg.view<StorageReqWflwTranComponent>().size() == 1u);
}
