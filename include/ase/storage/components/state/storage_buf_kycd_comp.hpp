#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_buf_kycd_comp.hpp
 * @brief       StorageBufKycdComponent - Durable keycard-issuance identity record
 * @description One entity per minted (or revoked) keycard, queued for durable
 *              round-trip to the Replica MongoDB authority. Mirrors
 *              StorageBufAudtComponent: the dist host NEVER links a data client,
 *              so the record is shipped owner-keyed over the Hub WS lane and the
 *              Replica drains then persists it (ARCH_ASE_STORAGE.md line 91 —
 *              "ase-mongodb → Persistierung (ACL, Keycards, Audit, Realms)").
 *              The accreditation axes (clearance, permission, expiry, realm)
 *              ride on a sibling StorageBufKycdAccrComponent on the same entity;
 *              codewords ride as Entity-per-Item StorageBufKycdCwrdComponent.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @created     2026-06-24
 * @modified    2026-06-24
 * @version     1.0.0
 *
 * ECS COMPONENT COMPLIANCE
 *
 * [ ] DATA fields ONLY - No methods
 * [ ] NO .cpp file - Header-only
 * [ ] ONLY zero-initialization (= 0, = 0.0f, = false, = {})
 * [ ] No magic numbers in defaults (use types.hpp constants)
 * [ ] Entity references initialized to = 0 (systems set values)
 * [ ] Single responsibility (one data category)
 * [ ] No God-Component (unrelated fields)
 * [ ] Large data in registry.ctx()? (component has only lookup ID!)
 * [ ] Tag structs end with Tag suffix - N/A (not a tag)
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [ ] Struct name derived from filename (snake_case to PascalCase)
 * [ ] 1 File = 1 Component
 * [ ] File in correct category subfolder
 * [ ] SHARED components listed in codegen.json components.shared
 * [ ] Pointer components in codegen.json components.server_only
 * [ ] Strings < 64 bytes use char[N] fixed arrays
 * [ ] Strings 64-256 bytes use appropriately sized char[N]
 * [ ] Strings > 256 bytes use registry.ctx() mit Lookup-ID?
 * [ ] NO Entity-per-Character (strings are single attributes, not N-Items!)
 * [ ] Lookup-only strings use uint32_t hash (entt::hashed_string)
 * [ ] NO std::shared_ptr in components (use Flyweight Pattern via ctx!)
 * [ ] NO void* in components (use Flyweight Pattern via ctx!)
 * [ ] NO uint64_t as pointer concept (use uint32_t ID + ResourceManager via ctx!)
 * [ ] External library objects (shared_ptr, handles) in ResourceManager via ctx()
 * [ ] Component stores ONLY primitive ID (uint32_t) referencing external resource
 */

#include <ase/storage/types.hpp>
#include <cstdint>

namespace ase::storage {

/**
 * @brief StorageBufKycdComponent - Keycard durable-persist identity record
 *
 * Created by StorageKycdPstEmitSystem from a minted StorageStaKycdComponent
 * (or from a revocation). Tagged StorageKycdPstPendTag until the durable
 * round-trip emits the owner-keyed SES_KYCD_PERSIST_* signals. The `op` field
 * distinguishes upsert (KYCD_PST_OP_UPSERT) from revoke (KYCD_PST_OP_REVOKE)
 * so issuance and revoke write the SAME durable store keyed by kycd_hash.
 *
 * @hub_reads  none
 * @hub_writes SES_KYCD_PERSIST_* (owner-keyed, drained by the Replica)
 */
struct StorageBufKycdComponent {
    char kycd_hash[MAX_KEYCARD_HASH] = {};    // SHA-256 hex digest — durable primary key
    char issued_to[MAX_OWNER_ID] = {};        // Recipient user ID (MongoDB ObjectId hex)
    char issued_by[MAX_OWNER_ID] = {};        // Issuer user ID (operator ObjectId hex)
    uint8_t op = 0;                           // KYCD_PST_OP_UPSERT or KYCD_PST_OP_REVOKE
    uint32_t cwrd_count = 0;                  // Number of StorageBufKycdCwrdComponent children
};

}  // namespace ase::storage
