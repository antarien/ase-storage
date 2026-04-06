#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_sta_kycd_comp.hpp
 * @brief       StorageStaKycdComponent - Keycard entity core data
 * @description Issued keycard with clearance, permissions, and expiry.
 *              Extended JWT carrying realm accreditations for A/ACS enforcement.
 *              Keycard type via Tags. Codewords via Entity-per-Item (StorageKycdCwrdComponent).
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @created     2026-04-04
 * @modified    2026-04-05
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

#include <cstdint>

namespace ase::storage {

/**
 * @brief StorageStaKycdComponent - Issued keycard with A/ACS accreditations
 *
 * Keycard type via Tags (StorageKycdLoginTag, StorageKycdInviteTag, etc.)
 * Transferability via Tag (StorageKycdTransferTag)
 * Revocation via Tag (StorageKycdRevTag)
 * Codewords via Entity-per-Item (StorageKycdCwrdComponent)
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageStaKycdComponent {
    char kycd_hash[64] = {};                  // SHA-256 hex digest of this keycard
    uint32_t relm_ref = 0;                    // Entity ref to realm this keycard grants access to
    uint32_t proj_ref = 0;                    // Entity ref to project (0 = realm-wide access)
    char issued_to[64] = {};                  // Recipient user ID (MongoDB ObjectId hex)
    char issued_by[64] = {};                  // Issuer user ID (MongoDB ObjectId hex)
    uint8_t clrn = 0;                        // Max Schutzstufe this keycard can access (0-9)
    uint16_t perm = 0;                        // Bitflags: PERM_READ | PERM_WRITE | PERM_DELETE | etc.
    uint64_t expires_at = 0;                  // Unix timestamp when keycard becomes invalid (0 = no expiry)
};

}  // namespace ase::storage
