# ase-storage

[![Layer](https://img.shields.io/badge/Layer-3%20Modules-green.svg)]()
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)]()
[![ECS](https://img.shields.io/badge/Pattern-ECS-orange.svg)]()

> Platform asset storage with Mandatory Access Control — Realms, Keycards, Schutzstufen, Codewörter, Lattice, Audit. Absorbs ase-auth.

Part of [ASE - Antares Simulation Engine](../../..)

## Overview

The **ase-storage** module manages all platform-level asset storage and access control for the ASE Engine. It implements the A/ACS (Antares Access Control System) from DSGN_109 applied to developer workflows: game studios, teams, freelancers, and publishers collaborating on projects built with the ASE Engine.

This module **absorbs ase-auth** — the Keycard IS the authentication token. Identity, authorization, and storage are one domain: **Who are you** (Keycard) → **What can you do** (ACL) → **Where are the files** (Storage).

### Core Concepts

- **Realms**: Isolated security sandboxes (personal, organization, public). No cross-realm visibility without explicit Lattice links.
- **Keycards**: Extended JWTs carrying identity + realm accreditations + clearance + codewords + permissions. Transferable, revocable, expirable.
- **Schutzstufen (0-9)**: Vertical protection levels on assets. A developer needs clearance ≥ asset level to access.
- **Codewörter**: Horizontal access keys for asset areas (e.g., "ART", "AUDIO", "SHADER"). Both clearance AND codeword must match.
- **Lattice**: Bilateral cross-realm bridges for sharing assets between studios/organizations.
- **Need-to-Know**: Task-based access scoping (Enterprise tier). Developers see only what their current task requires.
- **Vote of Confidence**: Team voting before granting high-clearance keycards to new members.
- **Audit Trail**: Every access logged — granted, denied, escalated. Full accountability.
- **Concealment**: Realms are invisible to non-members by default. Even existence is hidden.
- **Tier Quotas**: Indie (0-3 clearance, 1GB), Pro (0-6, 50GB), Enterprise (0-9, 500GB).

## Architecture

### A/ACS Subsystem Mapping (DSGN_109 → Platform)

| DSGN_109 | Platform System | ECS System |
|----------|----------------|------------|
| 83/ACCESS | Mandatory Access Control enforcement | StgAclChkSystem |
| 83/SSHARE | Shared build services via Realm | StgLatSyncSystem |
| 83/RSHARE | Shared resources (fonts, shaders) | StgLatSyncSystem |
| 83/ASHARE | Cross-project asset sharing | StgLatSyncSystem |
| 83/PSHARE | Member invitation with permissions | StgRlmMgrSystem |
| 83/APC | Bitflag permissions (READ\|WRITE\|DELETE\|...) | StgAclChkSystem |
| 83/ACL | Entity-per-Item ACL entries | StgAclChkSystem |
| 83/POLICY | Realm-wide default rules | StgRlmMgrSystem |
| 83/CODES | Project-area codewords | StgAclChkSystem |
| 83/LABELS | Asset workflow labels (draft/approved/...) | StgWflTransSystem |
| 83/SCTGRS | Asset categories (art/code/content/...) | StgAclChkSystem |
| 83/CLASS | Access classes per project area | StgAclChkSystem |
| 83/MSLM | Metadata Security Lattice | StgLatSyncSystem |
| 83/SRVLNC | Surveillance / audit logging | StgSrvLogSystem |
| 83/QUOTAS | Tier-based storage/member limits | StgQuoChkSystem |
| 83/LEVEL | Vertical protection levels (0-9) | StgAclChkSystem |
| 83/LATTICE | Horizontal cross-realm bridges | StgLatSyncSystem |
| 83/REALM | Isolated sandbox environments | StgRlmMgrSystem |
| 83/CNCLMNT | Realm concealment (hide existence) | StgCncFltSystem |
| 83/VOTE | Vote of Confidence for keycards | StgVotPrcSystem |
| 83/KEYCARD | Virtual Keycard (extended JWT) | StgKcdVldSystem |
| 83/CLEARANCE | Authority clearance levels | StgKcdVldSystem |
| 83/NTKPF | Need-to-Know task scoping | StgAclChkSystem |
| 83/WRKFLW | Asset workflow rules | StgWflTransSystem |

### System Pipeline

#### Keycard Pipeline (absorbed from ase-auth)

| # | System | Schedule | Purpose |
|---|--------|----------|---------|
| 1 | StgKcdDrnSystem | Ingestion (60Hz) | Drain keycard tokens from WebRTC buffer |
| 2 | StgKcdVldSystem | Ingestion (60Hz) | HMAC-SHA256 validation, parse claims |
| 3 | StgKcdLnkSystem | Ingestion (60Hz) | Link identity to client entity |
| 4 | StgKcdExpSystem | Preservation (1Hz) | Tag expired keycards |
| 5 | StgKcdRevSystem | Preservation (1Hz) | Tag revoked keycards |

#### ACL + Storage

| # | System | Schedule | Purpose |
|---|--------|----------|---------|
| 6 | StgAclChkSystem | Integration (60Hz) | 10-step A/ACS enforcement |
| 7 | StgFsWrtSystem | Integration (60Hz) | Filesystem CRUD operations |
| 8 | StgWflTransSystem | Integration (60Hz) | Workflow label transitions |
| 9 | StgCncFltSystem | Integration (60Hz) | Concealment filtering |

#### Async + Persistence

| # | System | Schedule | Purpose |
|---|--------|----------|---------|
| 10 | StgAudWrtSystem | Preservation (1Hz) | Batch-write audit entries to MongoDB |
| 11 | StgLatSyncSystem | Preservation (1Hz) | Sync lattice connections |
| 12 | StgQuoChkSystem | Observation (1Hz) | Quota monitoring |
| 13 | StgVotPrcSystem | Observation (1Hz) | Vote evaluation |
| 14 | StgSrvLogSystem | Observation (1Hz) | Anomaly detection |

### Storage Topology

```
{ASE_DATA_DIR}/
├── realms/
│   ├── realm_public/                    # Engine defaults, templates, shared shaders
│   ├── realm_o_{org_id}/               # Organization realm
│   │   ├── projects/{project_id}/
│   │   │   ├── assets/{domain}/        # Curator images, prompts
│   │   │   ├── content/                # Quests, lore, YAML
│   │   │   ├── builds/                 # Build artifacts
│   │   │   └── depot/                  # Git repo, manifest hash
│   │   └── shared/                     # Org-wide shared assets
│   └── realm_u_{user_hash}/            # Personal workspace
├── keycards/                           # Signed keycard store
└── audit/                              # Access logs
```

## Dependencies

### Layer 1 (Core)
- **ase-ecs**: Entity Component System
- **ase-log**: Logging
- **ase-mongodb**: Persistence (ACL, Keycards, Audit, Realms)

### Layer 0 (Foundation)
- **ase-types**: Constants (protection levels, permission flags)
- **ase-utils**: SHA-256, HMAC, crypto primitives

### Hub Channels
- Reads: User identity from Keycard validation
- Writes: Storage events (upload, delete, promote) for downstream modules

## Tier Quotas

| Resource | Indie | Pro | Enterprise |
|----------|-------|-----|-----------|
| Projects per Realm | 3 | 20 | unlimited |
| Storage per Project | 1 GB | 50 GB | 500 GB |
| Realm Members | 5 | 50 | unlimited |
| Keycards per Project | 10 | 100 | unlimited |
| Codewords per Project | 20 | 200 | unlimited |
| Protection Levels | 0-3 | 0-6 | 0-9 |
| Lattice Links | 1 (public only) | 5 | unlimited |
| Audit Retention | 30 days | 1 year | unlimited |

## Related Documents

- `ARCH_ASE_STORAGE.md` — Full architecture specification
- `DSGN_109` — A/ACS game design (31 subsystems, inspiration source)
- `DSGN_080` — Asset Management (lifecycle, categories)
- `ARCH_ASE_AUTH.md` — JWT pipeline (absorbed into this module)
- `DPLY_ASE_GAME_PROJECT.md` — Manifest + project structure
- `PJV Praeambel` — Complexity is a FEATURE

## License

MIT
