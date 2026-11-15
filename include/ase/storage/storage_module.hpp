#pragma once

/**
 * ASE MODULE DEFINITION
 *
 * @file        storage_module.hpp
 * @brief       StorageModule - A/ACS platform storage with Realms, Keycards, ACL
 * @description Registers all 15 storage systems across 4 schedules.
 *              Applies DSGN_109 patterns to real-world engine asset management.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @usecase     usecase:100265 - check access clearance
 * @created     2026-04-05
 * @modified    2026-11-07
 * @version     00.00.39.00039 [core]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
 *
 * Systems by schedule:
 *
 * Initialization:
 *   StorageIniSystem (creates manager, seeds realms, loads JWT secret)
 *
 * Ingestion (Developer Keycard Pipeline):
 *   StorageKycdDrnSystem (drains developer API tokens from ResourceManager)
 *   StorageKycdVldSystem (validates developer JWT, tags entities)
 *     run_after: StorageKycdDrnSystem
 *   StorageKycdLnkSystem (links validated developer keycards to client entities)
 *     run_after: StorageKycdVldSystem
 *   StorageKycdRevSynSystem (marks the keycard a revocation hub row names)
 *
 * Integration (ACL + Storage):
 *   StorageAcssChkSystem (10-step Mandatory Access Control)
 *   StorageFileWritSystem (filesystem CRUD via ResourceManager)
 *     run_after: StorageAcssChkSystem
 *   StorageWflwTranSystem (workflow label transitions)
 *     run_after: StorageFileWritSystem
 *   StorageCncmFltSystem (realm concealment filter)
 *     run_after: StorageAcssChkSystem
 *
 * Preservation:
 *   StorageKycdExpSystem (checks keycard expiration)
 *   StorageKycdRevSystem (files the durable revoke record)
 *     run_after: StorageKycdExpSystem
 *   StorageKycdPstEmitSystem (ships keycard persist records on frame 35)
 *     run_after: StorageKycdRevSystem
 *   StorageAudtWritSystem (batch-writes audit entries to MongoDB)
 *   StorageLatcSyncSystem (synchronizes lattice links)
 *
 * Reception (operator workflow console):
 *   StorageEdgeWflwFwdRcvSystem (files the forwarded operator commands)
 *   StorageEdgeWflwRplySystem (answers every filed command)
 *     run_after: StorageEdgeWflwFwdRcvSystem
 *
 * Observation:
 *   StorageQuotChkSystem (quota monitoring per realm)
 *   StorageRelmStatPubSystem (announces tier and default protection per realm)
 *   StorageVotePrcSystem (Vote of Confidence evaluation)
 *   StorageSrvlLogSystem (surveillance anomaly detection)
 *
 * ECS MODULE/PLUGIN DEFINITION COMPLIANCE
 *
 * [ ] name() returns correct name (ase-{module} or ase-pl-{plugin})
 * [ ] build() registers all systems in correct schedules
 * [ ] Startup systems registered first (run once at start)
 * [ ] Initialization systems registered (entity creation)
 * [ ] Integration/Dynamics systems registered with run_after() ordering
 * [ ] Transmission systems registered (network sync)
 * [ ] Preservation systems registered (database writes)
 * [ ] Finalization systems registered (cleanup)
 * [ ] All system includes present
 * [ ] No circular dependencies
 */

#include <ase/ecs/app.hpp>
#include <ase/storage/version.hpp>

// Initialization
#include <ase/storage/systems/keycard/storage_ini_sys.hpp>

// Ingestion (Keycard Pipeline)
#include <ase/storage/systems/keycard/storage_kycd_drn_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_ntfy_drn_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_ntfy_syn_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_req_drn_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_stat_pub_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_tier_seed_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_vld_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_lnk_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_lnk_cnt_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_sess_cln_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_rev_syn_sys.hpp>
// Keycard-keyed gate projection (replaces the session-keyed cwrd_pub): projects a
// minted keycard's A/ACS axes (clearance + permission + codewords) to the edge gate
// owner = hashed_string(issued_to) WITHOUT requiring a live validated session.

// Edge keycard-authz response drain (Phase 12 — dist tier customer-auth download).
// The REQUEST is sent directly by the dist edge routes (no storage emit system).
#include <ase/storage/systems/keycard/storage_edge_kycd_res_drn_sys.hpp>

// Integration (ACL + Storage)
#include <ase/storage/systems/acl/storage_acss_chk_sys.hpp>
#include <ase/storage/systems/acl/storage_acss_pol_sys.hpp>
#include <ase/storage/systems/acl/storage_acss_rslv_sys.hpp>
#include <ase/storage/systems/acl/storage_acss_idx_sys.hpp>
#include <ase/storage/systems/acl/storage_idn_idx_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_cwrd_pub_sys.hpp>
#include <ase/storage/systems/acl/storage_cred_acss_rcv_sys.hpp>
#include <ase/storage/systems/acl/storage_cred_acss_rsp_sys.hpp>
#include <ase/storage/systems/acl/storage_cncm_flt_sys.hpp>
#include <ase/storage/systems/fs/storage_file_writ_sys.hpp>
#include <ase/storage/systems/workflow/storage_wflw_drn_sys.hpp>
#include <ase/storage/systems/workflow/storage_wflw_stge_sys.hpp>
#include <ase/storage/systems/workflow/storage_edge_wflw_fwd_rcv_sys.hpp>
#include <ase/storage/systems/workflow/storage_edge_wflw_rply_sys.hpp>
#include <ase/storage/systems/workflow/storage_wflw_gate_sys.hpp>
#include <ase/storage/systems/workflow/storage_wflw_tran_sys.hpp>
#include <ase/storage/systems/workflow/storage_wflw_perm_sys.hpp>
#include <ase/storage/systems/workflow/storage_wflw_ini_sys.hpp>
#include <ase/storage/systems/workflow/storage_wflw_pst_sys.hpp>
#include <ase/storage/systems/workflow/storage_wflw_cln_sys.hpp>

// Initialization (Edge Distribution) - three objects, three systems: the realm,
// the ACL rules scoped to it, and the release-pipeline transition graph. The realm's
// license tier is written in Integration, once the hub constants exist.
#include <ase/storage/systems/edge/storage_edge_ini_sys.hpp>
#include <ase/storage/systems/edge/storage_edge_tier_sys.hpp>
#include <ase/storage/systems/acl/storage_acss_edge_ini_sys.hpp>
#include <ase/storage/systems/workflow/storage_wflw_edge_ini_sys.hpp>

// Preservation
#include <ase/storage/systems/keycard/storage_kycd_exp_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_rev_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_pst_emit_sys.hpp>
#include <ase/storage/systems/audit/storage_audt_writ_sys.hpp>
#include <ase/storage/systems/lattice/storage_latc_sync_sys.hpp>

// Curator
#include <ase/storage/systems/curator/storage_cur_prc_sys.hpp>

// Observation
#include <ase/storage/systems/quota/storage_quot_chk_sys.hpp>
#include <ase/storage/systems/realm/storage_relm_stat_pub_sys.hpp>
#include <ase/storage/systems/vote/storage_vote_prc_sys.hpp>
#include <ase/storage/systems/audit/storage_srvl_log_sys.hpp>
#include <ase/storage/systems/audit/storage_edge_audt_drn_sys.hpp>

namespace ase::storage {

struct StorageModule {
    static constexpr const char* name() { return "ase-storage"; }
    static constexpr const char* version() { return MODULE_VERSION; }

    void build(ecs::App& app) {
        // Initialization: manager entity, JWT secret, data dir, realm seeding
        app.add_system<StorageIniSystem>(ecs::Schedule::Initialization);
        // Edge-distribution realm seeding runs after the manager + data dir exist
        app.add_system_with<StorageEdgeIniSystem>(ecs::Schedule::Initialization)
            .run_after("StorageIniSystem");
        // The edge ACL rules are SCOPED to the realm entity and find it through
        // StorageRelmEdgeTag, so the realm has to exist first. run_after orders only
        // WITHIN a schedule and both sit in Initialization, so this constraint is the
        // whole guarantee: without it Kahn/FIFO may place the rule seeder first, and
        // it would log SCHEDULE_ORDER and seed nothing.
        app.add_system_with<StorageAcssEdgeIniSystem>(ecs::Schedule::Initialization)
            .run_after("StorageEdgeIniSystem");
        // The transition graph depends on NEITHER of the two: it is seeded from the
        // EDGE_LABEL_* chain in types.hpp alone and touches no realm and no rule.
        // It carries no ordering constraint on purpose - an unnecessary run_after
        // would claim a dependency that does not exist.
        app.add_system<StorageWflwEdgeIniSystem>(ecs::Schedule::Initialization);

        // Configuration: the service keycards of the tier services (KCD_SERVICE).
        //
        // WHY NOT Initialization, although it is a start-up step: the manager is CREATED
        // there (StorageIniSystem), and this system reads its wall clock. Configuration is
        // the following lifecycle schedule - "Cross-module initialization" (schedule.hpp) -
        // and it also runs exactly once per process. A run_after would be inert here: it
        // orders only WITHIN one schedule; between two schedules the number decides, and
        // Configuration is number 1.
        //
        // NO run_after ON StorageKycdReqDrnSystem, although that system collects the
        // requests: the drain sits in Ingestion and runs every frame. The request rows wait
        // until it takes them - minting one schedule earlier is the ordering itself.
        app.add_system<StorageKycdTierSeedSystem>(ecs::Schedule::Configuration);

        // Ingestion (60Hz): Developer Keycard pipeline (drain → validate → link)
        // The identity index is built FIRST in the frame's ingestion stage: the notify
        // drain and the keycard link both resolve by key, and a key they cannot look up
        // is a scan. Realms come from Reception, so they are already there.
        app.add_system<StorageIdnIdxSystem>(ecs::Schedule::Ingestion);
        app.add_system_with<StorageKycdDrnSystem>(ecs::Schedule::Ingestion)
            .run_after("StorageIdnIdxSystem");
        // SDK Hub-bridge drain: the hub rows HUB_STG_KYCD_REQ become
        // StorageReqKycdComponent on the request entity before the main
        // keycard-req drain sees it (same tick).
        //
        // NO run_after ON HubRcvDrnSystem, AND THE ABSENCE IS THE POINT — without this note the
        // next reader adds it back here and at StorageWflwDrnSystem below. Such an edge is
        // inert: run_after orders only WITHIN one schedule (dependency_sorter), and
        // HubRcvDrnSystem is registered in Reception (hub_module.hpp) while these drains run
        // in Ingestion. A name from another schedule is looked up, not found, and dropped
        // without a word.
        //
        // THE ORDERING IS UNCONDITIONAL ANYWAY, BY ARRAY POSITION — not by frequency. Reception
        // and Ingestion sit in the SAME tier: FRAME_SCHEDULES lists Reception at index 0 and
        // Ingestion at index 1 (tick_scheduler.cpp), and a tier runs its schedules in
        // array order. Frame carries interval 0.0f and therefore runs every tick without an
        // accumulator (tick_scheduler.cpp). "Same tick" in the line above is thus
        // guaranteed by the tier layout, not by an edge — and there is no delay a stall could
        // widen, so this note carries no "in steady operation" qualifier and must not be given
        // one. That qualifier belongs to cross-tier edges only.
        //
        // The edges BELOW that name Storage* systems are a different case and MUST STAY: both
        // ends run in Ingestion, same schedule, so the sorter resolves them.
        /**
         * SYN CUT AT THE KEYCARD REQUEST: the star first, then the arithmetic.
         *
         * The sync system reads the hub row HUB_STG_KYCD_REQ into two bridge components, and
         * the drain builds the request from them. Both run in Ingestion, the drain `run_after`
         * the sync system - otherwise it would read, in the same tick, a bridge that does not
         * exist yet, and the request would wait one tick longer than necessary.
         */
        app.add_system<StorageKycdNtfySynSystem>(ecs::Schedule::Ingestion);
        app.add_system_with<StorageKycdNtfyDrnSystem>(ecs::Schedule::Ingestion)
            .run_after("StorageKycdNtfySynSystem");
        // HTTP-posted keycard issuance drain runs after the notify bridge so it
        // sees the StorageReqKycdComponent the bridge laid on in the same tick.
        app.add_system_with<StorageKycdReqDrnSystem>(ecs::Schedule::Ingestion)
            .run_after("StorageKycdNtfyDrnSystem");
        /**
         * THE ISSUE COUNT IS ANNOUNCED, NOT COUNTED ALONG.
         *
         * Dissemination (70) lies behind Ingestion (11), so the announcement of every tick
         * already carries the keycards of THIS tick. The counter itself is kept by the drain,
         * on the manager row; keeping it in the star would mix hub I/O with arithmetic.
         */
        app.add_system<StorageKycdStatPubSystem>(ecs::Schedule::Dissemination);
        app.add_system_with<StorageKycdVldSystem>(ecs::Schedule::Ingestion)
            .run_after("StorageKycdDrnSystem");
        app.add_system_with<StorageKycdLnkSystem>(ecs::Schedule::Ingestion)
            .run_after("StorageKycdVldSystem");
        // The link system writes the star and does no arithmetic; it marks each link and this
        // system counts the marks into the module's own row (StorageKycdStatPubSystem announces).
        app.add_system_with<StorageKycdLnkCntSystem>(ecs::Schedule::Ingestion)
            .run_after("StorageKycdLnkSystem");
        // Keycard revocation, star side: the operator route files a hub row with the mark
        // HUB_STG_KYCD_REV, and this system tags the keycard whose digest the row names with
        // StorageKycdRevTag. StorageKycdRevSystem reads that tag in Preservation, which runs
        // after Ingestion in every frame, so no edge is needed between the two.
        app.add_system<StorageKycdRevSynSystem>(ecs::Schedule::Ingestion);
        // Workflow-promote hub-row drain: converts the hub rows HUB_STG_WFLW_REQ (a plugin files
        // them with sdk::row_create and sdk::set_bytes) into module-local StorageReqWflwTranComponent
        // requests marked StorageWflwRcvTag - the form StorageEdgeWflwFwdRcvSystem (frame 113,
        // `ase edge promote`) files in Reception.
        //
        // NO run_after ON HubRcvDrnSystem — same case as StorageKycdNtfyDrnSystem above, and
        // the reasoning is written out there once: Reception and Ingestion share the Frame tier
        // and run in array order, so the ordering holds without an edge.
        app.add_system<StorageWflwDrnSystem>(ecs::Schedule::Ingestion);
        // One staging for both doors: pending tag, released-gate tag and the STG_WFLW_RES
        // verdict PENDING, after the drain so a row drained this tick is staged in it.
        app.add_system_with<StorageWflwStgeSystem>(ecs::Schedule::Ingestion)
            .run_after("StorageWflwDrnSystem");

        // Integration (60Hz): index → ACL → file ops → workflow → concealment
        // The index is built FIRST and read by the ladder in the same tick. Ordering it
        // after StorageAcssChkSystem would leave the ladder reading last tick's relations,
        // which is exactly the stale-grant failure the full rebuild exists to prevent.
        app.add_system<StorageAcssIdxSystem>(ecs::Schedule::Integration);
        // The ladder asks two kinds of question. The resolution looks up (which realm, who
        // owns it, which ACL rule governs this path), the gate ladder decides (may THIS caller
        // pass). The channel between the two is StorageStaAcssRslvComponent with its marks -
        // in the same tick, so no slower.
        app.add_system_with<StorageAcssRslvSystem>(ecs::Schedule::Integration)
            .run_after("StorageAcssIdxSystem");
        app.add_system_with<StorageAcssChkSystem>(ecs::Schedule::Integration)
            .run_after("StorageAcssRslvSystem");
        // The gate ladder weighs TWO questions. The key gates weigh the CALLER (credential,
        // realm, lattice, protection level, codeword, permission), the policy gates weigh the
        // SUBJECT (label, running task, quota). The channel between the two is
        // StorageAcssPassTag - in the same tick, so no slower.
        //
        // THE GRANT FALLS HERE, NOT IN StorageAcssChkSystem. Every edge that waits for the
        // result of the ladder therefore points at THIS system; one that stopped halfway up
        // the ladder would see zero grants and silently do nothing.
        // The quota step of the policy gates weighs a write against the realm's tier, so the
        // edge realm's tier is written from ENG_CONST_TIER_ENTERPRISE first - after the hub
        // laid its constants, before the gates read it.
        app.add_system_with<StorageEdgeTierSystem>(ecs::Schedule::Integration)
            .run_after("HubIniCnstSystem");
        app.add_system_with<StorageAcssPolSystem>(ecs::Schedule::Integration)
            .run_after("StorageAcssChkSystem")
            .run_after("StorageEdgeTierSystem");
        app.add_system_with<StorageFileWritSystem>(ecs::Schedule::Integration)
            .run_after("StorageAcssPolSystem");
        // released-gate artifact precondition runs BEFORE the transition system:
        // requests targeting "released" keep StorageWflwGateTag until the
        // .sig/.sha256/.spdx.json/.smoke companions are verified on disk.
        app.add_system_with<StorageWflwGateSystem>(ecs::Schedule::Integration)
            .run_after("StorageFileWritSystem");
        // The promotion path is three steps. AUTHORISE (A/ACS: does the caller hold
        // PERM_PROMOTE) runs first and closes its own denials; ADOPT lays the draft rule for an
        // unmanaged asset that lies on disk; only then does the transition MOVE the asset along
        // an edge of the graph. Three questions, three failure modes, three files.
        app.add_system_with<StorageWflwPermSystem>(ecs::Schedule::Integration)
            .run_after("StorageWflwGateSystem");
        app.add_system_with<StorageWflwIniSystem>(ecs::Schedule::Integration)
            .run_after("StorageWflwPermSystem");
        app.add_system_with<StorageWflwTranSystem>(ecs::Schedule::Integration)
            .run_after("StorageWflwIniSystem");
        // AFTER THE WHOLE LADDER, not after its first half: the concealment filters on the
        // DECIDED result.
        app.add_system_with<StorageCncmFltSystem>(ecs::Schedule::Integration)
            .run_after("StorageAcssPolSystem");
        // The codeword projection is what the L4 edge gate reads: SES_KYCD_HOLDS_*,
        // SES_CLEARANCE and SES_KYCD_PERM at owner = hashed_string(user_id). The edge routes
        // (edge_binary_routes.cpp) and the hub metrics contract name this system as the
        // producer, so it must be registered here; without it those axes stand at NOT_FOUND
        // and the codeword step of the A/ACS ladder has no effect at that edge.
        // It runs in Integration because the session set it must ask is built there.
        app.add_system_with<StorageKycdCwrdPubSystem>(ecs::Schedule::Integration)
            .run_after("StorageAcssIdxSystem");

        // Integration (60Hz): Curator request processing (after ACL)
        // AFTER THE WHOLE LADDER: the curator works on granted requests, and the grant falls
        // in StorageAcssPolSystem.
        app.add_system_with<StorageCurPrcSystem>(ecs::Schedule::Integration)
            .run_after("StorageAcssPolSystem");

        // Preservation (1Hz): expiry, revocation, audit write, lattice sync
        app.add_system<StorageKycdExpSystem>(ecs::Schedule::Preservation);
        app.add_system_with<StorageKycdRevSystem>(ecs::Schedule::Preservation)
            .run_after("StorageKycdExpSystem");
        // Keycard durable persist: the revoke system files a record entity
        // (StorageBufKycdComponent + StorageKycdPstPendTag), and this system ships it on frame
        // 35 as the REPLACE-upsert document the Replica keys by user_id. The edge orders both
        // within Preservation, so a record filed this pass leaves in the same pass.
        app.add_system_with<StorageKycdPstEmitSystem>(ecs::Schedule::Preservation)
            .run_after("StorageKycdRevSystem");
        // Access-decision surveillance runs HERE, not in Observation, and it runs
        // BEFORE the writer. StorageAudtWritSystem is the sole owner of the audit
        // entity's lifetime: it ships the decision on frame 122 and destroys the
        // entity in the same pass. Preservation is schedule 71 and Observation is
        // 72, so an Observation-stage reader would arrive after the retirement and
        // see an empty view every single frame — the surveillance would report a
        // permanently quiet system while denials were streaming through. Ordering
        // is the fix; a retention flag would only be a second owner of the same
        // lifetime.
        app.add_system<StorageSrvlLogSystem>(ecs::Schedule::Preservation);
        app.add_system_with<StorageAudtWritSystem>(ecs::Schedule::Preservation)
            .run_after("StorageSrvlLogSystem");
        app.add_system<StorageLatcSyncSystem>(ecs::Schedule::Preservation);
        // Session hub-family retirement: StorageKycdLnkSystem publishes six SES_*
        // keys with owner = client entity, NetworkHubSyncSystem retires only the
        // three it wrote itself. Without this system clearance, realm and the user
        // hash of every session that ever authenticated stay in the hub for the
        // life of the process — and HubSessLifeClnSystem, which waits for the
        // owner's value count to reach zero, never withdraws the row HUB_SESS_LIFE.
        app.add_system<StorageKycdSessClnSystem>(ecs::Schedule::Preservation);
        // Workflow-label durable persist: drains the applied-transition buffers
        // (StorageBufWflwComponent + StorageWflwPstPendTag) into frame-112
        // REPLACE-upserts (storage_workflow_labels, keyed {realm,path}).
        app.add_system<StorageWflwPstSystem>(ecs::Schedule::Preservation);
        // The MINT side of the keycard persist frame lies in the dist L4 plugin mint sites
        // (BIN_MSG_EDGE_KYCD_PERSIST, the document rides the wire as DATA); the REVOKE side
        // is StorageKycdPstEmitSystem above. A hub signal could not carry the recipient
        // user_id string across to the Replica, so both sides ship the document as a frame.

        // Observation (1Hz): quota monitoring, vote evaluation
        // (anomaly detection runs in Preservation — see StorageSrvlLogSystem above)
        app.add_system<StorageQuotChkSystem>(ecs::Schedule::Observation);
        // Tier and default protection of every realm, under the same owner as the quota rows
        // (hashed realm id), so GET /api/realms/:realmId finds all rows of a realm by one key.
        // Pure pass-through, hub::set is change-based; the edge only keeps the realm rows of
        // one tick together in the order the quota announcement already has.
        app.add_system_with<StorageRelmStatPubSystem>(ecs::Schedule::Observation)
            .run_after("StorageQuotChkSystem");
        // Retired-build retention sweep (Tag-filtered, 90 days): the cleanup —
        // not the quota ceiling — is what keeps the edge_binaries realm small.
        app.add_system<StorageWflwClnSystem>(ecs::Schedule::Observation);
        app.add_system<StorageVotePrcSystem>(ecs::Schedule::Observation);
        // Drain edge A/ACS gate audit-signals (SES_EDGE_AUDIT_*) emitted by the
        // L4 edge-distribution gate into the storage audit buffer so the
        // Preservation-stage StorageAudtWritSystem persists every gate decision.
        app.add_system<StorageEdgeAudtDrnSystem>(ecs::Schedule::Observation);

        // The edge keycard-authz REQUEST is not a storage system: the dist edge routes
        // (edge_keycard_routes/edge_binary_routes trigger_keycard_fetch) build the
        // BIN_MSG_EDGE_KYCD_REQ frame from the held user_id and push it onto the L1
        // transport outbound queue directly — the user_id is string DATA on the binary wire,
        // never the numeric Hub. The RESPONSE drain (StorageEdgeKycdResDrnSystem) publishes
        // the SES_CLEARANCE session the gate reads.

        // The matching receiver: pops BIN_MSG_EDGE_KYCD_RES off the keycard lane,
        // parses the keycard document the Replica found, and publishes the
        // SES_CLEARANCE + SES_KYCD_PERM + SES_KYCD_HOLDS_* session the gate reads —
        // owner = hashed_string(user_id), byte-for-byte the StorageKycdCwrdPubSystem
        // projection. Reception, alongside the kernel WS inbound demux (uniform with
        // RsnMemResDrnSystem). A NOT_FOUND / revoked keycard publishes nothing.
        app.add_system<StorageEdgeKycdResDrnSystem>(ecs::Schedule::Reception);
        // Operator release-workflow WS console receiver (Phase 12): pops BIN_MSG_EDGE_WFLW_FWD off
        // LANE_WFLW (the Replica already verified the operator YK-JWT) and files every frame as a
        // StorageReqWflwFwdComponent marked promote, status or refused; a promote also files the
        // promote request (StorageWflwStge/Gate/Tran/Pst drive it). Reception, alongside the
        // kernel WS inbound demux, built the same way as StorageEdgeKycdResDrnSystem.
        app.add_system<StorageEdgeWflwFwdRcvSystem>(ecs::Schedule::Reception);
        // The answer to every filed command: promote_requested, the live STG_WFLW_RES /
        // STG_WFLW_STAGE verdict or the refusal, shipped as BIN_MSG_EDGE_WFLW_RES for the Replica
        // to relay to the ase-cli. Both sit in Reception, so this edge is the whole guarantee
        // that a command filed this tick is answered this tick.
        app.add_system_with<StorageEdgeWflwRplySystem>(ecs::Schedule::Reception)
            .run_after("StorageEdgeWflwFwdRcvSystem");
        // Credential A/ACS access-check: drain CACSS_WIRE_REQ (LANE_CACSS) from the Replica, resolve
        // keycard + project realm, emplace StorageReqAcssComponent so the Integration ladder decides.
        app.add_system<StorageCredAcssRcvSystem>(ecs::Schedule::Reception);
        // Ship the ladder verdict (Grant/DenyTag) back to the Replica as CACSS_WIRE_RES, after the receiver.
        app.add_system_with<StorageCredAcssRspSystem>(ecs::Schedule::Reception)
            .run_after("StorageCredAcssRcvSystem");

        // Operator bootstrap lives in the dist L4 plugin (EdgeOperSeedSystem):
        // it mints a DURABLE operator keycard via the SDK pipeline instead of an
        // in-memory per-launch session — so it survives restarts and the existing
        // /edge/keycard/session → Replica-fetch chain publishes SES_CLEARANCE.
    }
};

}  // namespace ase::storage
