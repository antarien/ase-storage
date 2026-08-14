#pragma once

/**
 * ASE MODULE TYPES (SSOT)
 *
 * @file        types.hpp
 * @brief       Single Source of Truth for ase-storage constants and types
 * @description All compile-time constants, type aliases, and enumeration values.
 *              A/ACS patterns applied to real-world engine asset management.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @created     2026-04-04
 * @modified    2026-04-04
 * @version     1.0.0
 *
 * ECS TYPES COMPLIANCE
 *
 * [ ] All constants defined (no magic numbers in code)
 * [ ] Every constant has inline comment (English, explains purpose)
 * [ ] NO enum class (only constexpr uint8_t for enumeration values)
 * [ ] Type aliases defined
 * [ ] InvalidEntityId = UINT32_MAX defined (if needed)
 * [ ] Abbreviations documented
 * [ ] NO structs (structs belong in Components)
 */

#include <cstdint>

namespace ase::storage {

// ── SERIAL TYPE IDs (Layer 3: 380-399 — network serialization) ──────────

constexpr uint32_t SERIAL_TYP_STG_RLM  = 380;  // Realm state serialization identifier
constexpr uint32_t SERIAL_TYP_STG_KCD  = 381;  // Keycard state serialization identifier
constexpr uint32_t SERIAL_TYP_STG_ACL  = 382;  // ACL rule serialization identifier
constexpr uint32_t SERIAL_TYP_STG_AUD  = 383;  // Audit entry serialization identifier
constexpr uint32_t SERIAL_TYP_STG_LAT  = 384;  // Lattice link serialization identifier
constexpr uint32_t SERIAL_TYP_STG_VOT  = 385;  // Vote data serialization identifier
constexpr uint32_t SERIAL_TYP_STG_TSK  = 386;  // Need-to-Know task serialization identifier
constexpr uint32_t SERIAL_TYP_STG_IDN  = 387;  // Identity data serialization identifier
constexpr uint32_t SERIAL_TYP_STG_TKN  = 388;  // Token reference serialization identifier

// ── BUFFER SIZES (char[N] array lengths) ────────────────────────────────

constexpr uint32_t MAX_REALM_ID       = 64;    // Realm path identifier max chars
constexpr uint32_t MAX_REALM_NAME     = 128;   // Realm display name max chars
constexpr uint32_t MAX_OWNER_ID       = 64;    // User ID max chars (MongoDB ObjectId hex)
constexpr uint32_t MAX_PATH_LEN       = 256;   // Filesystem path or glob pattern max chars
constexpr uint32_t MAX_CODEWORD_LEN   = 32;    // Single codeword max chars
constexpr uint32_t MAX_LABEL_LEN      = 32;    // Workflow label max chars
constexpr uint32_t MAX_REASON_LEN     = 64;    // Audit deny-reason max chars
constexpr uint32_t CRED_ACSS_RSP_BATCH = 64;   // Max credential A/ACS verdicts responded per tick (deferred-destroy batch bound)
constexpr uint32_t MAX_TASK_NAME      = 128;   // Need-to-Know task name max chars
constexpr uint32_t MAX_KEYCARD_HASH   = 64;    // SHA-256 hex digest max chars
constexpr uint32_t MAX_EMAIL_LEN      = 64;    // Email address max chars
constexpr uint32_t MAX_DISPLAY_NAME   = 64;    // User display name max chars

// ── JWT / KEYCARD LIMITS (developer platform tokens) ────────────────────

constexpr uint32_t JWT_MAX_LENGTH           = 2048;  // Max JWT string length in base64url encoding
constexpr uint32_t AUTH_CHECK_INTERVAL_MS   = 1000;  // Expiry check runs every 1000ms at 1Hz
constexpr uint16_t USER_ID_MAX_LENGTH       = 64;    // Max user_id field size in bytes

// ── REALM TYPES ─────────────────────────────────────────────────────────

constexpr uint8_t REALM_PERSONAL      = 0;    // Auto-created workspace per registered user
constexpr uint8_t REALM_ORGANIZATION  = 1;    // Studio or team workspace created by owner
constexpr uint8_t REALM_PUBLIC        = 2;    // Platform-wide engine defaults and shared templates

// ── REALM STATUS ────────────────────────────────────────────────────────

constexpr uint8_t REALM_ACTIVE        = 0;    // Normal read-write operation mode
constexpr uint8_t REALM_SUSPENDED     = 1;    // Disabled by admin (no read or write)
constexpr uint8_t REALM_ARCHIVED      = 2;    // Read-only before deletion

// ── TIER LEVELS (license tiers control quotas and max protection) ───────

constexpr uint8_t TIER_INDIE          = 0;    // Small team with limited features and storage
constexpr uint8_t TIER_PRO            = 1;    // Professional studio with expanded limits
constexpr uint8_t TIER_ENTERPRISE     = 2;    // Full control with unlimited resources

// ── PROTECTION LEVELS — Schutzstufen (vertical content classification) ─

constexpr uint8_t PROTECTION_PUBLIC       = 0;  // Showcase and marketing material visible to all
constexpr uint8_t PROTECTION_VISIBLE      = 1;  // Released assets and docs for realm members
constexpr uint8_t PROTECTION_INTERNAL     = 2;  // Work-in-progress assets for project team
constexpr uint8_t PROTECTION_TEAM         = 3;  // Team-internal planning and discussion materials
constexpr uint8_t PROTECTION_RESTRICTED   = 4;  // Unreleased features requiring explicit codeword
constexpr uint8_t PROTECTION_PROTECTED    = 5;  // API keys and service credentials for tech leads
constexpr uint8_t PROTECTION_CONFIDENTIAL = 6;  // Story spoilers and unpublished narrative content
constexpr uint8_t PROTECTION_SECRET       = 7;  // Pre-announcement roadmap and business plans
constexpr uint8_t PROTECTION_TOP_SECRET   = 8;  // Signing keys and master encryption credentials
constexpr uint8_t PROTECTION_SOVEREIGN    = 9;  // Realm config and billing for owner access

// ── MAX PROTECTION PER TIER ─────────────────────────────────────────────

constexpr uint8_t TIER_INDIE_MAX_PROTECTION      = 3;  // Indie teams use levels 0 through 3
constexpr uint8_t TIER_PRO_MAX_PROTECTION        = 6;  // Pro studios use levels 0 through 6
constexpr uint8_t TIER_ENTERPRISE_MAX_PROTECTION  = 9;  // Enterprise has access to all 10 levels

// ── PERMISSION BITFLAGS (developer action authorization) ────────────────

constexpr uint16_t PERM_READ     = 0x0001;   // Read and download assets from storage
constexpr uint16_t PERM_WRITE    = 0x0002;   // Upload and modify assets in storage
constexpr uint16_t PERM_DELETE   = 0x0004;   // Remove assets from storage
constexpr uint16_t PERM_PROMOTE  = 0x0008;   // Promote asset versions through workflow
constexpr uint16_t PERM_MANAGE   = 0x0010;   // Manage ACL rules and workflow labels
constexpr uint16_t PERM_INVITE   = 0x0020;   // Issue keycards to invite new members
constexpr uint16_t PERM_ALL      = 0xFFFF;   // Full access wildcard for realm owners

// ── KEYCARD TYPES (developer access grant method) ──────────────────────

constexpr uint8_t KCD_LOGIN      = 0;    // Auto-issued session token at user login
constexpr uint8_t KCD_INVITE     = 1;    // Realm owner invites a team member
constexpr uint8_t KCD_GUEST      = 2;    // Limited access for publishers or freelancers
constexpr uint8_t KCD_DELEGATE   = 3;    // Transferred keycard from an existing member
constexpr uint8_t KCD_SERVICE    = 4;    // Automated access for CI/CD build servers

// ── AUDIT ACTIONS (recorded platform operations) ────────────────────────

constexpr uint8_t AUD_READ       = 0;    // Asset was read or downloaded by user
constexpr uint8_t AUD_WRITE      = 1;    // Asset was uploaded or modified by user
constexpr uint8_t AUD_DELETE     = 2;    // Asset was removed by user
constexpr uint8_t AUD_PROMOTE    = 3;    // Asset version was promoted in workflow
constexpr uint8_t AUD_MANAGE     = 4;    // ACL rule or setting was changed by admin
constexpr uint8_t AUD_INVITE     = 5;    // New keycard was issued to a member
constexpr uint8_t AUD_LOGIN      = 6;    // User authenticated via keycard validation
constexpr uint8_t AUD_REVOKE     = 7;    // Keycard was revoked by issuer or admin

// ── AUDIT RESULTS ───────────────────────────────────────────────────────

constexpr uint8_t AUD_GRANTED    = 0;    // Access was allowed after A/ACS check passed
constexpr uint8_t AUD_DENIED     = 1;    // Access was refused with logged reason string
constexpr uint8_t AUD_ESCALATED  = 2;    // Decision was forwarded to admin or vote system

// ── ASSET CATEGORIES (engine content classification) ────────────────────

constexpr uint8_t AST_ART        = 0;    // Images, prompts, 3D models and textures
constexpr uint8_t AST_AUDIO      = 1;    // Music, sound effects and voice recordings
constexpr uint8_t AST_CONTENT    = 2;    // Quest YAML, lore documents and dialog data
constexpr uint8_t AST_CODE       = 3;    // Plugin source code, shaders and ECS systems
constexpr uint8_t AST_CONFIG     = 4;    // Project manifest, environment and settings files
constexpr uint8_t AST_BUILD      = 5;    // Compiled artifacts and distribution packages
constexpr uint8_t AST_SIGNING    = 6;    // Cryptographic keys and signing certificates
constexpr uint8_t AST_DOC        = 7;    // Documentation, guides and README files

// ── VOTE STATUS (content review approval outcome) ───────────────────────

constexpr uint8_t VOT_OPEN       = 0;    // Active vote collecting ballots from members
constexpr uint8_t VOT_ACCEPTED   = 1;    // Majority voted in favor of the motion
constexpr uint8_t VOT_REJECTED   = 2;    // Majority voted against the motion
constexpr uint8_t VOT_EXPIRED    = 3;    // Deadline passed without reaching quorum

// ── BALLOT DECISIONS ────────────────────────────────────────────────────

constexpr uint8_t BALLOT_FOR     = 0;    // Cast vote in favor of the motion
constexpr uint8_t BALLOT_AGAINST = 1;    // Cast vote against the motion
constexpr uint8_t BALLOT_ABSTAIN = 2;    // Abstain but count toward quorum threshold

// ── TASK STATUS (Need-to-Know scoping, Enterprise tier) ────────────────

constexpr uint8_t TASK_ACTIVE    = 0;    // Developer has scoped access within this task
constexpr uint8_t TASK_DONE      = 1;    // Task finished, scoped access revoked
constexpr uint8_t TASK_LAPSED    = 2;    // Deadline passed, scoped access revoked

// ── QUOTA LIMITS — INDIE TIER ───────────────────────────────────────────

constexpr uint32_t QUOTA_INDIE_PROJECTS    = 3;              // Max projects in one realm
constexpr uint64_t QUOTA_INDIE_STORAGE     = 1073741824;     // Max bytes per project (1 GB)
constexpr uint32_t QUOTA_INDIE_MEMBERS     = 5;              // Max members per realm
constexpr uint32_t QUOTA_INDIE_KEYCARDS    = 10;             // Max active keycards per project
constexpr uint32_t QUOTA_INDIE_CODEWORDS   = 20;             // Max codewords per project
constexpr uint32_t QUOTA_INDIE_LATTICE     = 1;              // Max lattice links (public only)
constexpr uint32_t QUOTA_INDIE_AUDIT_DAYS  = 30;             // Audit log retention in days

// ── QUOTA LIMITS — PRO TIER ─────────────────────────────────────────────

constexpr uint32_t QUOTA_PRO_PROJECTS      = 20;             // Max projects in one realm
constexpr uint64_t QUOTA_PRO_STORAGE       = 53687091200;    // Max bytes per project (50 GB)
constexpr uint32_t QUOTA_PRO_MEMBERS       = 50;             // Max members per realm
constexpr uint32_t QUOTA_PRO_KEYCARDS      = 100;            // Max active keycards per project
constexpr uint32_t QUOTA_PRO_CODEWORDS     = 200;            // Max codewords per project
constexpr uint32_t QUOTA_PRO_LATTICE       = 5;              // Max lattice links to other realms
constexpr uint32_t QUOTA_PRO_AUDIT_DAYS    = 365;            // Audit log retention in days

// ── QUOTA LIMITS — ENTERPRISE TIER ──────────────────────────────────────

constexpr uint32_t QUOTA_ENT_PROJECTS      = 0xFFFFFFFF;     // Unlimited projects per realm
constexpr uint64_t QUOTA_ENT_STORAGE       = 536870912000;   // Max bytes per project (500 GB)
constexpr uint32_t QUOTA_ENT_MEMBERS       = 0xFFFFFFFF;     // Unlimited members per realm
constexpr uint32_t QUOTA_ENT_KEYCARDS      = 0xFFFFFFFF;     // Unlimited active keycards
constexpr uint32_t QUOTA_ENT_CODEWORDS     = 0xFFFFFFFF;     // Unlimited codewords per project
constexpr uint32_t QUOTA_ENT_LATTICE       = 0xFFFFFFFF;     // Unlimited lattice links
constexpr uint32_t QUOTA_ENT_AUDIT_DAYS    = 0xFFFFFFFF;     // Unlimited audit log retention

// ── A/ACS LADDER PRESETS (Section 14.1 in-ladder rules, no pre-ladder shortcuts) ─
// The public realm and the realm-owner power are modelled as proper ladder
// inputs so every GRANT happens only at step 10, never before step 2.

constexpr const char* ACSS_REALM_PUBLIC_ID = "ase";   // Public 'ase' realm: engine defaults readable by all (PUBLIC protection rule)
constexpr const char* ACSS_CWRD_WILDCARD   = "ALL";   // Wildcard codeword: owner-preset keycard satisfies every required codeword
constexpr uint8_t  ACSS_OWNER_CLEARANCE = 9;          // Realm-owner keycard preset clearance (SOVEREIGN, ARCH :819 "Owner = Clearance 9")
constexpr uint16_t ACSS_OWNER_PERMS     = 0xFFFF;     // Realm-owner keycard preset permissions (all bitflags, ARCH :819 "alle Codewörter")

// ── ENTITY REFERENCE ────────────────────────────────────────────────────

constexpr uint32_t INVALID_ENTITY = 0xFFFFFFFF;  // No entity reference (UINT32_MAX sentinel)

// ── ACL RULE MATCHING (path_pattern semantics) ──────────────────────────
// A LEADING '*' makes a rule a SUFFIX rule ("*.sig" governs companion artifacts
// that sit BESIDE binaries — prefix patterns cannot express them); any other
// pattern is the established prefix rule. Specificity ranking: suffix rules
// outrank every prefix rule; among equals the LONGER pattern wins.
constexpr uint32_t ACSS_MATCH_SUFFIX_BONUS = 1000;  // score bonus lifting suffix rules above all prefix rules

// ── FAHNE FLAGS (SharedHeader UI zone identifiers) ──────────────────────

constexpr uint8_t FLAG_ORG  = 0;    // Organization and realm context zone
constexpr uint8_t FLAG_PRJ  = 1;    // Active project context zone
constexpr uint8_t FLAG_KEY  = 2;    // Keycard clearance and permissions zone
constexpr uint8_t FLAG_SHR  = 3;    // Shared resources and lattice links zone

constexpr uint8_t MAX_FLAG_ITEMS = 4;  // Maximum key-value items per Fahne zone

// ── CURATOR — Curation metadata for game art assets ────────────────────

constexpr uint32_t CUR_MAX_KEY    = 160;   // Curation key max chars ("IMG_ID" or "IMG_ID::v3")
constexpr uint32_t CUR_MAX_NOTES  = 256;   // Free-text curation notes max chars
constexpr uint32_t CUR_MAX_USERID = 64;    // User ID who last edited max chars

constexpr uint8_t CUR_ST_UNRATED      = 0;   // Image not yet reviewed by curator
constexpr uint8_t CUR_ST_APPROVED     = 1;   // Approved for use in game
constexpr uint8_t CUR_ST_REJECTED     = 2;   // Not suitable for intended purpose
constexpr uint8_t CUR_ST_NEEDS_REWORK = 3;   // Requires changes before approval

constexpr uint8_t CUR_ACT_RATE    = 0;   // Set rating on an image or version
constexpr uint8_t CUR_ACT_STATUS  = 1;   // Set approval status on image or version
constexpr uint8_t CUR_ACT_NOTES   = 2;   // Set free-text notes on image or version

// ── EDGE-BINARY DISTRIBUTION (Phase 12 — A/ACS edge-daemon distribution) ─

constexpr const char* EDGE_REALM_ID = "edge_binaries";  // Dedicated realm for edge-daemon binary distribution

// Workflow labels — release pipeline driven by StorageWflwTranSystem transitions
constexpr const char* EDGE_LABEL_DRAFT    = "draft";     // Uploaded build, not yet reviewed
constexpr const char* EDGE_LABEL_REVIEW   = "review";    // Under release-manager review
constexpr const char* EDGE_LABEL_APPROVED = "approved";  // Reviewed, awaiting publish to released
constexpr const char* EDGE_LABEL_RELEASED = "released";  // Public download (clearance 0 + codeword BINARY)
constexpr const char* EDGE_LABEL_RETIRED  = "retired";   // Withdrawn build, no longer downloadable

// Codewords — horizontal access keys for edge-binary asset areas
constexpr const char* EDGE_CWRD_BINARY   = "BINARY";     // Native binary + companion latest manifest
constexpr const char* EDGE_CWRD_SBOM     = "SBOM";       // SPDX software bill of materials
constexpr const char* EDGE_CWRD_SIG      = "SIG";        // ES256 YubiKey-PIV signature over the SHA-256
constexpr const char* EDGE_CWRD_METADATA = "METADATA";   // compatibility.json + version manifests

constexpr uint8_t EDGE_CLEARANCE_CUSTOMER = 0;   // Customer download-only clearance for released binaries
constexpr uint8_t EDGE_CLEARANCE_OPERATOR = 5;   // Release-manager full release-workflow clearance

// ── WORKFLOW TRANSITION ENGINE (Phase 12 Task 12.3) ─────────────────────
// The label chain draft→review→approved→released→retired is DATA: each allowed
// edge is a seeded StorageWflwEdgeComponent entity (StorageEdgeIniSystem), and
// StorageWflwTranSystem validates a requested transition purely by matching the
// request against those edge entities — no switch/if-chain dispatch.

// Result codes — published owner-scoped (owner = hashed_string(path)) as the Hub
// value STG_WFLW_RES once a promote request is processed; the dist plugin route
// reads them back via sdk::get without any ase-storage include.
constexpr uint8_t WFLW_RES_PENDING     = 0;   // Request staged, not yet processed
constexpr uint8_t WFLW_RES_APPLIED     = 1;   // Edge allowed: label written + audited + persisted
constexpr uint8_t WFLW_RES_DENIED_EDGE = 2;   // Requested edge not in the seeded transition graph
constexpr uint8_t WFLW_RES_DENIED_GATE = 3;   // released-gate companion artifact missing
constexpr uint8_t WFLW_RES_DENIED_PERM = 4;   // Requester keycard lacks PERM_PROMOTE
constexpr uint8_t WFLW_RES_NOT_FOUND   = 5;   // No ACL rule for the path and no on-disk asset to bootstrap draft

// Stage ordinals — published owner-scoped as STG_WFLW_STAGE for display widgets
// (float-safe small integers; the label STRING never rides the numeric Hub).
constexpr uint8_t WFLW_STAGE_DRAFT    = 0;   // EDGE_LABEL_DRAFT ordinal
constexpr uint8_t WFLW_STAGE_REVIEW   = 1;   // EDGE_LABEL_REVIEW ordinal
constexpr uint8_t WFLW_STAGE_APPROVED = 2;   // EDGE_LABEL_APPROVED ordinal
constexpr uint8_t WFLW_STAGE_RELEASED = 3;   // EDGE_LABEL_RELEASED ordinal
constexpr uint8_t WFLW_STAGE_RETIRED  = 4;   // EDGE_LABEL_RETIRED ordinal

// released-gate companion artifacts — promotion to released is refused unless ALL
// FOUR exist beside the asset in the realm. Checked by the Tag-filtered
// StorageWflwGateSystem (StorageWflwGateTag), never inside the transition system.
constexpr const char* WFLW_ART_SIG   = ".sig";        // ES256 signature over the binary
constexpr const char* WFLW_ART_SHA   = ".sha256";     // SHA-256 checksum file
constexpr const char* WFLW_ART_SBOM  = ".spdx.json";  // SPDX SBOM document
constexpr const char* WFLW_ART_SMOKE = ".smoke";      // Operator smoke-test marker (health-check passed)

// Durable persist — every APPLIED transition ships as a workflow-label document to
// the Replica (REPLACE-upsert into storage_workflow_labels keyed {realm,path}). The
// BIN_MSG id + frame layout are mirrored from modules/ase-replication/replica_types.hpp
// (SSOT registration: ase-network types.hpp); changing either side requires changing
// both (single contract, same envelope as the keycard persist frame 35).
constexpr uint8_t  EDGE_WFLW_BIN_MSG_PERSIST = 112;   // dist → Replica: [112][req_id:u64][doc_len:u32][doc]
constexpr uint32_t WFLW_PST_DOC_BUF          = 1024;  // workflow-label persist document scratch bytes (mirror EDGE_WFLW_PERSIST_DOC_MAX)
constexpr uint32_t WFLW_REQ_BATCH            = 16;    // max workflow entities drained/denied/persisted per tick (deferred-destroy batch bound)

// ── EDGE-REALM QUOTA + RETIRED RETENTION (Phase 12 Task 12.14 / R14) ────────
// SIZING (R14 verification, MEASURED 2026-07-11 against the LIVE realm): one
// linux-x86_64 release = daemon binary 2,028,760 B (~1.9 MB) + bundled
// libdatachannel 3,916,432 B (~3.7 MB) + .sig (72 B) + SPDX SBOM (~1.6 KB)
// + .sha256/.smoke (<1 KB) ≈ 6 MB; whole live realm with one release =
// 5,968,931 B. 10 GB therefore holds ~1700 full releases — years of runway even
// at daily releases across 5 platforms, because the 90-day retired-cleanup
// (StorageWflwClnSystem), not the ceiling, is what keeps the realm small. The
// earlier "Enterprise (500GB)" figure (PLAN PHASE_12:70 / RISKS R14) was an
// UNVERIFIED tier template, not a sizing decision — corrected in both
// documents; never copy it back here.
constexpr uint64_t EDGE_REALM_QUOTA_BYTES   = 10737418240ULL;  // 10 GB edge_binaries realm ceiling
constexpr uint64_t QUOTA_SCAN_INTERVAL_S    = 60;              // seconds between realm-usage FS scans (Observation pacing)
constexpr uint64_t WFLW_RETIRED_RETENTION_S = 7776000;         // 90 days: retired builds older than this are deleted

// ── DIGIT EXTRACTION (base-10 helpers) ──────────────────────────────────

constexpr uint32_t DECIMAL_RADIX       = 10;  // base-10 digit extraction divisor

// ── KEYCARD DURABLE-PERSIST ROUND-TRIP (Phase 12 H-3 — Replica MongoDB) ────
// A minted keycard is shipped owner-keyed over the Hub WS lane to the Replica,
// which drains the SES_KYCD_PERSIST_* signals and upserts the keycard document.
// op distinguishes a fresh/updated keycard from a revocation so issuance and
// revoke write the SAME durable store keyed by kycd_hash.

constexpr uint8_t KYCD_PST_OP_UPSERT = 0;   // Insert or update the durable keycard document
constexpr uint8_t KYCD_PST_OP_REVOKE = 1;   // Mark the durable keycard document revoked

// Per-index Hub key construction for durable-persist codeword projection.
// "SES_KYCD_PERSIST_CWRD_<owner>_<index>" debug-labels carry each codeword
// string; the owner is hashed_string(issued_to) so records never collide
// across users (mirrors the SES_KYCD_CWRD_ owner-scoped key scheme).
constexpr uint32_t PST_KEY_BUF_LEN = 64;    // SES_KYCD_PERSIST_CWRD_<owner>_<i> key scratch chars

// ── EDGE KEYCARD-AUTHZ FETCH LANE (Phase 12 — customer-auth edge download) ──
// The dist tier (ase-server-dist) validates NOTHING itself: when a customer
// download arrives with no live local gate session, the dist edge ROUTES
// (edge_keycard_routes/edge_binary_routes trigger_keycard_fetch) build the
// BIN_MSG_EDGE_KYCD_REQ frame from the held user_id and push it onto the L1
// transport outbound queue directly — the user_id is string DATA on the binary
// wire, never the numeric Hub. The Replica (ReplicaEdgeKycdSystem) FINDs the
// keycard in MongoDB and ships BIN_MSG_EDGE_KYCD_RES back; StorageEdgeKycdResDrnSystem
// parses the document and publishes the SES_CLEARANCE + SES_KYCD_PERM +
// SES_KYCD_HOLDS_* session the gate reads. The dist host links NO data client — the
// SAME split as the edge-daemon connection-token check (ReplicaEdgeRegSystem). The
// RES BIN_MSG id + frame layout are mirrored from
// modules/ase-replication/replica_types.hpp; changing either side requires
// changing both (single contract).

constexpr uint8_t EDGE_KYCD_BIN_MSG_RES = 34;  // Replica → dist: [34][req_id:u64][status:u8][payload_len:u32][payload]

// RES frame layout (mirrors replica_types.hpp RSN_MEM_READ_RES_HDR/PAYLOAD_MAX).
constexpr uint32_t EDGE_KYCD_RES_HDR     = 14;    // 1 id + 8 req_id + 1 status + 4 payload_len
constexpr uint32_t EDGE_KYCD_PAYLOAD_MAX = 8192;  // Max keycard-document payload returned per fetch

// RES status enum (mirrors replica_types.hpp RSN_MEM_STATUS_*).
constexpr uint8_t EDGE_KYCD_STATUS_OK        = 0;  // Keycard found, document payload follows
constexpr uint8_t EDGE_KYCD_STATUS_NOT_FOUND = 1;  // No matching keycard, payload empty
constexpr uint8_t EDGE_KYCD_STATUS_ERROR     = 2;  // Backend/serialization error

// Max codewords parsed out of a recovered keycard document (bounds the dist decode loop).
// Each parsed codeword is compared EXACTLY, server-internal, against the fixed edge codewords
// and projected only as an owner-scoped SES_KYCD_HOLDS_<cw> boolean verdict — never a string
// or per-index key over the Hub.
constexpr uint32_t KYCD_DECODE_CWRD_MAX   = 64;  // max codewords parsed out of a recovered keycard document

// ── Operator release-workflow WS console forward (Phase 12) ──────────────────
// The operator drives promote/status from `ase edge` over the ase-cli WS console to
// the Replica; the Replica verifies the operator YK-JWT and forwards the command here
// as BIN_MSG_EDGE_WFLW_FWD(113). StorageEdgeWflwFwdRcvSystem pops LANE_WFLW, and for a
// promote deposits the SAME hub workflow-bridge request the /admin/workflow/promote
// route uses (StorageWflwDrn/Gate/Tran/Pst then drive it), or for a status reads the
// live STG_WFLW_STAGE/RES, then replies BIN_MSG_EDGE_WFLW_RES(114) to the Replica which
// relays it to the CLI. Mirror of ase-network / ase-replication types.hpp (SSOT there);
// changing either side requires changing both (single wire contract).
constexpr uint8_t  EDGE_WFLW_BIN_MSG_FWD = 113;  // Replica → dist: [113][cli_conn:u32][op:u8][path_len:u16][path][label_len:u16][label][by_len:u16][by]
constexpr uint8_t  EDGE_WFLW_BIN_MSG_RES = 114;  // dist → Replica → CLI: [114][cli_conn:u32][status:u8][json_len:u32][json]
constexpr uint32_t EDGE_WFLW_FWD_HDR = 5u;   // [113] + cli_conn(4)
constexpr uint32_t EDGE_WFLW_RES_HDR = 10u;  // [114] + cli_conn(4) + status(1) + json_len(4)
constexpr uint8_t  EDGE_WFLW_OP_PROMOTE = 0u;  // op byte: drive the release-workflow promote
constexpr uint8_t  EDGE_WFLW_OP_STATUS  = 1u;  // op byte: read the live workflow stage/result
constexpr uint8_t  EDGE_WFLW_STATUS_OK          = 0u;  // verdict shipped / promote staged / status found
constexpr uint8_t  EDGE_WFLW_STATUS_NOT_AUTHED  = 1u;  // (Replica-side only; the dist never emits it)
constexpr uint8_t  EDGE_WFLW_STATUS_NO_DIST     = 2u;  // (Replica-side only)
constexpr uint8_t  EDGE_WFLW_STATUS_BAD_REQUEST = 3u;  // malformed forward frame (missing path/target)
constexpr uint8_t  EDGE_WFLW_STATUS_NOT_FOUND   = 4u;  // status: no workflow state staged for this path
constexpr uint32_t EDGE_WFLW_JSON_MAX  = 512u;  // verdict JSON payload bytes

}  // namespace ase::storage
