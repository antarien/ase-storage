#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

/**
 * ASE Storage — Workflow-Transition Tests (doctest, Phase 12 Task 12.3)
 *
 * Drives the REAL StorageWflwTranSystem over the data-driven transition graph
 * (die Kanten): the allowed edges draft→review→approved→released→retired are
 * seeded exactly as StorageEdgeIniSystem seeds them, requests are the same
 * StorageReqWflwTranComponent + StorageWflwPendTag entities StorageWflwDrnSystem
 * stages, and every verdict is asserted on the REAL side effects — the ACL rule
 * label, the owner-scoped STG_WFLW_RES Hub verdict, the attributed audit entity
 * (AUD_PROMOTE + requester user_id) and the frame-112 persist buffer. No
 * filesystem and no wire are needed: the rules are pre-seeded, so the draft
 * bootstrap (the only file_exists caller) never runs.
 */

#include <ase/storage/storage_module.hpp>
#include <ase/storage/systems/workflow/storage_wflw_tran_sys.hpp>
#include <ase/storage/components/request/storage_req_wflw_tran_comp.hpp>
#include <ase/storage/components/state/storage_wflw_edge_comp.hpp>
#include <ase/storage/components/state/storage_acss_rule_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/state/storage_buf_audt_comp.hpp>
#include <ase/storage/components/state/storage_buf_wflw_comp.hpp>
#include <ase/storage/components/tag/storage_tag_wflw_pend.hpp>
#include <ase/storage/components/tag/storage_tag_wflw_gate.hpp>
#include <ase/storage/components/tag/storage_tag_wflw_pst_pend.hpp>
#include <ase/storage/components/tag/storage_tag_audt_pend.hpp>
#include <ase/storage/components/state/storage_wflw_retr_comp.hpp>
#include <ase/storage/components/tag/storage_tag_wflw_retr.hpp>
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

// Seed the fixed release-pipeline graph exactly as StorageEdgeIniSystem does.
void seed_edges(Registry& reg) {
    const char* chain_from[4] = {EDGE_LABEL_DRAFT, EDGE_LABEL_REVIEW,
                                 EDGE_LABEL_APPROVED, EDGE_LABEL_RELEASED};
    const char* chain_to[4]   = {EDGE_LABEL_REVIEW, EDGE_LABEL_APPROVED,
                                 EDGE_LABEL_RELEASED, EDGE_LABEL_RETIRED};
    for (uint32_t i = 0; i < 4; ++i) {
        auto e = reg.create();
        auto& edge = reg.emplace<StorageWflwEdgeComponent>(e);
        ase::utils::str_copy(edge.from_label, MAX_LABEL_LEN, chain_from[i]);
        ase::utils::str_copy(edge.to_label, MAX_LABEL_LEN, chain_to[i]);
    }
}

// Edge realm + per-asset rule at the given starting label; returns the rule entity.
Entity seed_realm_and_rule(Registry& reg, const char* start_label) {
    auto relm_ent = reg.create();
    auto& relm = reg.emplace<StorageStaRelmComponent>(relm_ent);
    ase::utils::str_copy(relm.id, MAX_REALM_ID, EDGE_REALM_ID);
    auto rule_ent = reg.create();
    auto& rule = reg.emplace<StorageAcssRuleComponent>(rule_ent);
    rule.relm_ref = static_cast<uint32_t>(relm_ent);
    rule.proj_ref = 0;
    ase::utils::str_copy(rule.path_pattern, MAX_PATH_LEN, kAsset);
    rule.protection_level = PROTECTION_PUBLIC;
    ase::utils::str_copy(rule.label, MAX_LABEL_LEN, start_label);
    return rule_ent;
}

// Stage a promote request exactly as StorageWflwDrnSystem does (sans gate tag —
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

float read_verdict(Registry& reg) {
    const uint32_t owner = entt::hashed_string(kAsset).value();
    return ase::hub::get(reg, owner, "STG_WFLW_RES"_hs, -1.0f);
}

}  // namespace

TEST_CASE("workflow edges: allowed transition applies label + attributed audit + persist buffer") {
    App app;
    app.set_source("ase-storage");
    app.add_system<StorageWflwTranSystem>(Schedule::Integration);
    app.startup();
    auto& reg = app.registry();
    auto* mgr = new StorageResourceManager();
    reg.ctx().emplace<StorageResourceManager*>(mgr);

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
    for (auto [e, aud] : reg.view<StorageBufAudtComponent, StorageAudtPendTag>().each()) {
        if (aud.action != AUD_PROMOTE) continue;
        if (aud.result != AUD_GRANTED) continue;
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
    delete mgr;
}

TEST_CASE("workflow edges: forbidden edge (draft to released) is denied, label untouched") {
    App app;
    app.set_source("ase-storage");
    app.add_system<StorageWflwTranSystem>(Schedule::Integration);
    app.startup();
    auto& reg = app.registry();
    auto* mgr = new StorageResourceManager();
    reg.ctx().emplace<StorageResourceManager*>(mgr);

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
    for (auto [e, aud] : reg.view<StorageBufAudtComponent, StorageAudtPendTag>().each()) {
        if (aud.result != AUD_DENIED) continue;
        CHECK(std::strncmp(aud.reason, "wflw_edge(", 10) == 0);
        ++denied_audits;
    }
    CHECK(denied_audits == 1);
    // NO persist buffer for a denied transition.
    CHECK(reg.view<StorageBufWflwComponent>().size() == 0);

    app.shutdown();
    delete mgr;
}

TEST_CASE("workflow edges: requester without PERM_PROMOTE is denied fail-closed") {
    App app;
    app.set_source("ase-storage");
    app.add_system<StorageWflwTranSystem>(Schedule::Integration);
    app.startup();
    auto& reg = app.registry();
    auto* mgr = new StorageResourceManager();
    reg.ctx().emplace<StorageResourceManager*>(mgr);

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
    delete mgr;
}

TEST_CASE("workflow edges: full chain draft to review to approved to released to retired") {
    App app;
    app.set_source("ase-storage");
    app.add_system<StorageWflwTranSystem>(Schedule::Integration);
    app.startup();
    auto& reg = app.registry();
    auto* mgr = new StorageResourceManager();
    reg.ctx().emplace<StorageResourceManager*>(mgr);

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
    delete mgr;
}

TEST_CASE("workflow retention: retired build older than 90 days is swept with rule + AUD_DELETE") {
    App app;
    app.set_source("ase-storage");
    app.add_system<StorageWflwClnSystem>(Schedule::Observation);
    app.startup();
    auto& reg = app.registry();
    auto* mgr = new StorageResourceManager();
    reg.ctx().emplace<StorageResourceManager*>(mgr);

    // Edge realm (audit ref) plus the retiring ACL rule that dies with the build.
    auto rule_ent = seed_realm_and_rule(reg, EDGE_LABEL_RETIRED);

    // get_wall_time_seconds is time(nullptr); guard against a zero clock so the
    // subtraction below stays a real past instant, never an unsigned wrap.
    const uint64_t now = mgr->get_wall_time_seconds();
    REQUIRE(now > WFLW_RETIRED_RETENTION_S);

    // One retired record, retired well before the 90-day window closed. No file is
    // written: remove_if_present treats a missing target as already-gone, so the
    // record + rule sweep and the AUD_DELETE audit are exercised without disk I/O.
    auto retr_ent = reg.create();
    auto& retr = reg.emplace<StorageWflwRetrComponent>(retr_ent);
    ase::utils::str_copy(retr.path, MAX_PATH_LEN, kAsset);
    retr.rule_ref = static_cast<uint32_t>(rule_ent);
    retr.retired_at = now - WFLW_RETIRED_RETENTION_S - 100;
    reg.emplace<StorageWflwRetrTag>(retr_ent);

    app.run_schedule(Schedule::Observation, 1.0f);

    // Record swept and the retiring rule destroyed with it.
    CHECK(reg.view<StorageWflwRetrComponent>().size() == 0);
    CHECK(!reg.valid(rule_ent));
    // Exactly one AUD_DELETE audit, path-attributed, carrying the retention reason.
    uint32_t del_audits = 0;
    for (auto [e, aud] : reg.view<StorageBufAudtComponent, StorageAudtPendTag>().each()) {
        if (aud.action != AUD_DELETE) continue;
        CHECK(std::strncmp(aud.reason, "wflw_retention", 14) == 0);
        CHECK(ase::utils::str_equal(aud.path, kAsset, MAX_PATH_LEN));
        ++del_audits;
    }
    CHECK(del_audits == 1);

    app.shutdown();
    delete mgr;
}

TEST_CASE("workflow retention: retired build within 90 days is kept, not swept") {
    App app;
    app.set_source("ase-storage");
    app.add_system<StorageWflwClnSystem>(Schedule::Observation);
    app.startup();
    auto& reg = app.registry();
    auto* mgr = new StorageResourceManager();
    reg.ctx().emplace<StorageResourceManager*>(mgr);

    auto rule_ent = seed_realm_and_rule(reg, EDGE_LABEL_RETIRED);
    const uint64_t now = mgr->get_wall_time_seconds();
    REQUIRE(now > WFLW_RETIRED_RETENTION_S);

    // Just retired, deep inside the retention window.
    auto retr_ent = reg.create();
    auto& retr = reg.emplace<StorageWflwRetrComponent>(retr_ent);
    ase::utils::str_copy(retr.path, MAX_PATH_LEN, kAsset);
    retr.rule_ref = static_cast<uint32_t>(rule_ent);
    retr.retired_at = now - 100;
    reg.emplace<StorageWflwRetrTag>(retr_ent);

    app.run_schedule(Schedule::Observation, 1.0f);

    // Retained: record + rule alive, and no delete was audited.
    CHECK(reg.view<StorageWflwRetrComponent>().size() == 1);
    CHECK(reg.valid(rule_ent));
    uint32_t del_audits = 0;
    for (auto [e, aud] : reg.view<StorageBufAudtComponent, StorageAudtPendTag>().each()) {
        if (aud.action == AUD_DELETE) ++del_audits;
    }
    CHECK(del_audits == 0);

    app.shutdown();
    delete mgr;
}
