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

// ── A/ACS LADDER PRESETS (§14.1 in-ladder rules, no pre-ladder shortcuts) ─
// The public realm and the realm-owner power are modelled as proper ladder
// inputs so every GRANT happens only at step 10, never before step 2.

constexpr const char* ACSS_REALM_PUBLIC_ID = "ase";   // Public 'ase' realm: engine defaults readable by all (PUBLIC protection rule)
constexpr const char* ACSS_CWRD_WILDCARD   = "ALL";   // Wildcard codeword: owner-preset keycard satisfies every required codeword
constexpr uint8_t  ACSS_OWNER_CLEARANCE = 9;          // Realm-owner keycard preset clearance (SOVEREIGN, ARCH §:819 "Owner = Clearance 9")
constexpr uint16_t ACSS_OWNER_PERMS     = 0xFFFF;     // Realm-owner keycard preset permissions (all bitflags, ARCH §:819 "alle Codewörter")

// ── ENTITY REFERENCE ────────────────────────────────────────────────────

constexpr uint32_t INVALID_ENTITY = 0xFFFFFFFF;  // No entity reference (UINT32_MAX sentinel)

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

// ── KEYCARD-NOTIFY KEY BUILDING (per-index Hub key construction, Phase 12) ─

constexpr uint32_t NTF_CWRD_PREFIX_LEN = 18;  // length of the literal "SES_KYCD_NTF_CWRD_"
constexpr uint32_t NTF_KEY_BUF_LEN     = 40;  // SES_KYCD_NTF_CWRD_<i> key scratch buffer chars
constexpr uint32_t NTF_NUM_BUF_LEN     = 12;  // decimal index scratch buffer chars
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
// SES_KYCD_CWRD_* session the gate reads. The dist host links NO data client — the
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

// Per-index decode codeword key construction. The dist decode rebuilds the
// identical "SES_KYCD_CWRD_<owner>_<i>" key StorageKycdCwrdPubSystem produces so
// the edge A/ACS gate reads the recovered codeword set unchanged.
constexpr uint32_t KYCD_CWRD_KEY_BUF      = 64;  // "SES_KYCD_CWRD_<owner>_<i>" key scratch chars
constexpr uint32_t KYCD_DECODE_CWRD_MAX   = 64;  // max codewords parsed out of a recovered keycard document

}  // namespace ase::storage
