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
 * @created     2026-04-05
 * @modified    2026-04-05
 * @version     1.0.0
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
 *   StorageKycdRevSystem (emits durable revoke signal to the Replica)
 *     run_after: StorageKycdExpSystem
 *   StorageAudtWritSystem (batch-writes audit entries to MongoDB)
 *   StorageLatcSyncSystem (synchronizes lattice links)
 *
 * Observation:
 *   StorageQuotChkSystem (quota monitoring per realm)
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
#include <ase/storage/systems/keycard/storage_kycd_req_drn_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_tier_seed_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_vld_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_lnk_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_sess_cln_sys.hpp>
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
#include <ase/storage/systems/workflow/storage_edge_wflw_fwd_rcv_sys.hpp>
#include <ase/storage/systems/workflow/storage_wflw_gate_sys.hpp>
#include <ase/storage/systems/workflow/storage_wflw_tran_sys.hpp>
#include <ase/storage/systems/workflow/storage_wflw_perm_sys.hpp>
#include <ase/storage/systems/workflow/storage_wflw_ini_sys.hpp>
#include <ase/storage/systems/workflow/storage_wflw_pst_sys.hpp>
#include <ase/storage/systems/workflow/storage_wflw_cln_sys.hpp>

// Initialization (Edge Distribution) - three objects, three systems: the realm,
// the ACL rules scoped to it, and the release-pipeline transition graph.
#include <ase/storage/systems/edge/storage_edge_ini_sys.hpp>
#include <ase/storage/systems/acl/storage_acss_edge_ini_sys.hpp>
#include <ase/storage/systems/workflow/storage_wflw_edge_ini_sys.hpp>

// Preservation
#include <ase/storage/systems/keycard/storage_kycd_exp_sys.hpp>
#include <ase/storage/systems/keycard/storage_kycd_rev_sys.hpp>
#include <ase/storage/systems/audit/storage_audt_writ_sys.hpp>
#include <ase/storage/systems/lattice/storage_latc_sync_sys.hpp>

// Curator
#include <ase/storage/systems/curator/storage_cur_prc_sys.hpp>

// Observation
#include <ase/storage/systems/quota/storage_quot_chk_sys.hpp>
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

        // Configuration: die Service-Keycards der Tier-Dienste (KCD_SERVICE).
        //
        // WARUM NICHT Initialization, obwohl es ein Start-Vorgang ist: dort wird der
        // Verwalter erst ANGELEGT (StorageIniSystem), und dieses System liest seine Wanduhr.
        // Configuration ist die nachfolgende Lifecycle-Schedule — "Cross-module
        // initialization" (schedule.hpp) — und laeuft ebenfalls genau einmal je Prozess.
        // Ein run_after waere hier wirkungslos: es ordnet nur INNERHALB einer Schedule;
        // zwischen zwei Schedules entscheidet die Nummer, und Configuration ist die 1.
        //
        // KEIN run_after AUF StorageKycdReqDrnSystem, obwohl der die Antraege abholt: der
        // Drain sitzt in Ingestion und laeuft in jedem Frame. Die Antragszeilen liegen, bis
        // er sie nimmt — eine Schedule frueher zu praegen ist die Ordnung selbst.
        app.add_system<StorageKycdTierSeedSystem>(ecs::Schedule::Configuration);

        // Ingestion (60Hz): Developer Keycard pipeline (drain → validate → link)
        // The identity index is built FIRST in the frame's ingestion stage: the notify
        // drain and the keycard link both resolve by key, and a key they cannot look up
        // is a scan. Realms come from Reception, so they are already there.
        app.add_system<StorageIdnIdxSystem>(ecs::Schedule::Ingestion);
        app.add_system_with<StorageKycdDrnSystem>(ecs::Schedule::Ingestion)
            .run_after("StorageIdnIdxSystem");
        // SDK Hub-bridge drain: convert SES_KYCD_NTF_* Hub keys into
        // StorageReqKycdComponent on the request entity before the main
        // keycard-req drain sees it (same tick).
        //
        // NO run_after ON HubRcvDrnSystem, AND THE ABSENCE IS THE POINT — without this note the
        // next reader adds it back here and at StorageWflwDrnSystem below. Both edges existed
        // and both were inert: run_after orders only WITHIN one schedule (dependency_sorter),
        // and HubRcvDrnSystem is registered in Reception (hub_module.hpp) while these drains run
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
        app.add_system<StorageKycdNtfyDrnSystem>(ecs::Schedule::Ingestion);
        // HTTP-posted keycard issuance drain runs after the notify bridge so it
        // sees StorageReqKycdComponent + HubStgKycdPendTag together.
        app.add_system_with<StorageKycdReqDrnSystem>(ecs::Schedule::Ingestion)
            .run_after("StorageKycdNtfyDrnSystem");
        app.add_system_with<StorageKycdVldSystem>(ecs::Schedule::Ingestion)
            .run_after("StorageKycdDrnSystem");
        app.add_system_with<StorageKycdLnkSystem>(ecs::Schedule::Ingestion)
            .run_after("StorageKycdVldSystem");
        // Workflow-promote Hub-bridge drain: converts HubStgWflwReqComponent bridge
        // entities (sdk::emplace_workflow_promote_request) into module-local
        // StorageReqWflwTranComponent requests (+ released-gate tag).
        //
        // NO run_after ON HubRcvDrnSystem — same case as StorageKycdNtfyDrnSystem above, and
        // the reasoning is written out there once: Reception and Ingestion share the Frame tier
        // and run in array order, so the ordering holds without an edge.
        app.add_system<StorageWflwDrnSystem>(ecs::Schedule::Ingestion);

        // Integration (60Hz): index → ACL → file ops → workflow → concealment
        // The index is built FIRST and read by the ladder in the same tick. Ordering it
        // after StorageAcssChkSystem would leave the ladder reading last tick's relations,
        // which is exactly the stale-grant failure the full rebuild exists to prevent.
        app.add_system<StorageAcssIdxSystem>(ecs::Schedule::Integration);
        // TRENNUNG 2026-08-29: die Leiter stellt zwei Arten von Frage. Die Aufloesung schlaegt
        // nach (welches Revier, wem gehoert es, welche ACL-Regel regiert diesen Pfad), die
        // Torleiter entscheidet (darf DIESER Anrufer). Der Kanal zwischen beiden ist
        // StorageStaAcssRslvComponent samt Marken - im selben Tick, also unveraendert schnell.
        app.add_system_with<StorageAcssRslvSystem>(ecs::Schedule::Integration)
            .run_after("StorageAcssIdxSystem");
        app.add_system_with<StorageAcssChkSystem>(ecs::Schedule::Integration)
            .run_after("StorageAcssRslvSystem");
        // TRENNUNG 2026-08-31: die Torleiter wog ZWEI Fragen. Die Schluessel-Tore wiegen den
        // ANRUFER (Ausweis, Revier, Gitter, Schutzstufe, Codewort, Recht), die Politik-Tore den
        // GEGENSTAND (Label, laufende Aufgabe, Kontingent). Der Kanal zwischen beiden ist
        // StorageAcssPassTag - im selben Tick, also unveraendert schnell.
        //
        // DIE GEWAEHRUNG FAELLT SEITHER HIER, NICHT IN StorageAcssChkSystem. Jede Kante, die auf
        // das Ergebnis der Leiter wartet, zeigt deshalb auf DIESES System; eine, die auf der
        // halben Leiter stehen bliebe, saehe null Gewaehrungen und taete still gar nichts.
        app.add_system_with<StorageAcssPolSystem>(ecs::Schedule::Integration)
            .run_after("StorageAcssChkSystem");
        app.add_system_with<StorageFileWritSystem>(ecs::Schedule::Integration)
            .run_after("StorageAcssPolSystem");
        // released-gate artifact precondition runs BEFORE the transition system:
        // requests targeting "released" keep StorageWflwGateTag until the
        // .sig/.sha256/.spdx.json/.smoke companions are verified on disk.
        app.add_system_with<StorageWflwGateSystem>(ecs::Schedule::Integration)
            .run_after("StorageFileWritSystem");
        // TRENNUNG 2026-08-29: der Beförderungs-Pfad trug drei Vorgaenge in einer tick().
        // BERECHTIGEN (A/ACS: haelt der Anrufer PERM_PROMOTE) laeuft zuerst und schliesst seine
        // Ablehnungen selbst ab; ADOPTIEREN legt fuer ein unverwaltetes, auf der Platte
        // liegendes Gut die Entwurfs-Regel an; erst dann BEWEGT der Uebergang das Gut entlang
        // einer Kante des Graphen. Drei Fragen, drei Fehlerbilder, drei Dateien.
        app.add_system_with<StorageWflwPermSystem>(ecs::Schedule::Integration)
            .run_after("StorageWflwGateSystem");
        app.add_system_with<StorageWflwIniSystem>(ecs::Schedule::Integration)
            .run_after("StorageWflwPermSystem");
        app.add_system_with<StorageWflwTranSystem>(ecs::Schedule::Integration)
            .run_after("StorageWflwIniSystem");
        // NACH DER GANZEN LEITER, nicht nach ihrer ersten Haelfte: die Verschleierung filtert auf
        // das ENTSCHIEDENE Ergebnis (2026-08-31 mit dem Schnitt nachgezogen).
        app.add_system_with<StorageCncmFltSystem>(ecs::Schedule::Integration)
            .run_after("StorageAcssPolSystem");
        // The codeword projection is what the L4 edge gate reads: SES_KYCD_HOLDS_*,
        // SES_CLEARANCE and SES_KYCD_PERM at owner = hashed_string(user_id). It existed,
        // it was referenced as the producer by edge_binary_routes.cpp and by the hub
        // metrics contract - and it was registered NOWHERE, so those axes stood at
        // NOT_FOUND and the codeword step of the A/ACS ladder had no effect at that edge.
        // It runs in Integration because the session set it must ask is built there.
        app.add_system_with<StorageKycdCwrdPubSystem>(ecs::Schedule::Integration)
            .run_after("StorageAcssIdxSystem");

        // Integration (60Hz): Curator request processing (after ACL)
        // NACH DER GANZEN LEITER: der Kurator arbeitet auf gewaehrten Anfragen, und die
        // Gewaehrung faellt seit dem 2026-08-31 in StorageAcssPolSystem.
        app.add_system_with<StorageCurPrcSystem>(ecs::Schedule::Integration)
            .run_after("StorageAcssPolSystem");

        // Preservation (1Hz): expiry, revocation, audit write, lattice sync
        app.add_system<StorageKycdExpSystem>(ecs::Schedule::Preservation);
        app.add_system_with<StorageKycdRevSystem>(ecs::Schedule::Preservation)
            .run_after("StorageKycdExpSystem");
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
        // owner's value count to reach zero, never releases HubSessLifeTag.
        app.add_system<StorageKycdSessClnSystem>(ecs::Schedule::Preservation);
        // Workflow-label durable persist: drains the applied-transition buffers
        // (StorageBufWflwComponent + StorageWflwPstPendTag) into frame-112
        // REPLACE-upserts (storage_workflow_labels, keyed {realm,path}).
        app.add_system<StorageWflwPstSystem>(ecs::Schedule::Preservation);
        // Keycard durable-persist is no longer a storage system: minting a keycard now
        // ships the full document straight to the Replica from the dist L4 plugin mint
        // sites (BIN_MSG_EDGE_KYCD_PERSIST, the document rides the wire as DATA). The
        // former Hub-signal emit (SES_KYCD_PERSIST_*) could not carry the recipient
        // user_id string across to the Replica — that bridge is removed.

        // Observation (1Hz): quota monitoring, vote evaluation
        // (anomaly detection moved to Preservation — see StorageSrvlLogSystem above)
        app.add_system<StorageQuotChkSystem>(ecs::Schedule::Observation);
        // Retired-build retention sweep (Tag-filtered, 90 days): the cleanup —
        // not the quota ceiling — is what keeps the edge_binaries realm small.
        app.add_system<StorageWflwClnSystem>(ecs::Schedule::Observation);
        app.add_system<StorageVotePrcSystem>(ecs::Schedule::Observation);
        // Drain edge A/ACS gate audit-signals (SES_EDGE_AUDIT_*) emitted by the
        // L4 edge-distribution gate into the storage audit buffer so the
        // Preservation-stage StorageAudtWritSystem persists every gate decision.
        app.add_system<StorageEdgeAudtDrnSystem>(ecs::Schedule::Observation);

        // Edge keycard-authz REQUEST is no longer a storage system: the dist edge
        // routes (edge_keycard_routes/edge_binary_routes trigger_keycard_fetch)
        // build the BIN_MSG_EDGE_KYCD_REQ frame from the held user_id and push it
        // onto the L1 transport outbound queue directly — the user_id is string
        // DATA on the binary wire, never the numeric Hub. The RESPONSE drain
        // (StorageEdgeKycdResDrnSystem) stays: it publishes the SES_CLEARANCE
        // session the gate reads.

        // The matching receiver: pops BIN_MSG_EDGE_KYCD_RES off the keycard lane,
        // parses the keycard document the Replica found, and publishes the
        // SES_CLEARANCE + SES_KYCD_PERM + SES_KYCD_HOLDS_* session the gate reads —
        // owner = hashed_string(user_id), byte-for-byte the StorageKycdCwrdPubSystem
        // projection. Reception, alongside the kernel WS inbound demux (uniform with
        // RsnMemResDrnSystem). A NOT_FOUND / revoked keycard publishes nothing.
        app.add_system<StorageEdgeKycdResDrnSystem>(ecs::Schedule::Reception);
        // Operator release-workflow WS console receiver (Phase 12): pops BIN_MSG_EDGE_WFLW_FWD off
        // LANE_WFLW (the Replica already verified the operator YK-JWT), deposits the hub workflow-bridge
        // request for a promote (StorageWflwDrn/Gate/Tran/Pst drive it) or reads STG_WFLW_STAGE/RES for a
        // status, and ships BIN_MSG_EDGE_WFLW_RES back for the Replica to relay to the ase-cli. Reception,
        // alongside the kernel WS inbound demux, mirror of StorageEdgeKycdResDrnSystem.
        app.add_system<StorageEdgeWflwFwdRcvSystem>(ecs::Schedule::Reception);
        // Credential A/ACS access-check: drain CACSS_WIRE_REQ (LANE_CACSS) from the Replica, resolve
        // keycard + project realm, emplace StorageReqAcssComponent so the Integration ladder decides.
        app.add_system<StorageCredAcssRcvSystem>(ecs::Schedule::Reception);
        // Ship the ladder verdict (Grant/DenyTag) back to the Replica as CACSS_WIRE_RES, after the receiver.
        app.add_system_with<StorageCredAcssRspSystem>(ecs::Schedule::Reception)
            .run_after("StorageCredAcssRcvSystem");

        // Operator bootstrap moved to the dist L4 plugin (EdgeOperSeedSystem):
        // it mints a DURABLE operator keycard via the SDK pipeline instead of an
        // in-memory per-launch session — so it survives restarts and the existing
        // /edge/keycard/session → Replica-fetch chain publishes SES_CLEARANCE.
    }
};

}  // namespace ase::storage
