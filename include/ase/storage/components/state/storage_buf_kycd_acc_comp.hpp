#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_buf_kycd_acc_comp.hpp
 * @brief       StorageBufKycdAccComponent - Keycard durable-persist accreditation axes
 * @description Sibling of StorageBufKycdComponent on the same persist-record
 *              entity, carrying the A/ACS accreditation axes (clearance,
 *              permission, expiry, realm/project binding) of a minted keycard
 *              for the durable Replica round-trip. Split from the identity
 *              record to keep each component focused (≤5 fields) per
 *              WRFL_ASE_COMPONENT_DESIGN.md — Component composition, not a
 *              God-Component.
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

#include <cstdint>

namespace ase::storage {

/**
 * @brief StorageBufKycdAccComponent - Keycard durable-persist accreditation axes
 *
 * Emplaced beside StorageBufKycdComponent by StorageKycdPstEmitSystem. The
 * Replica persists these axes onto the same MongoDB keycard document keyed by
 * StorageBufKycdComponent.kycd_hash.
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageBufKycdAccComponent {
    uint32_t relm_ref = 0;                    // Entity ref to realm this keycard grants access to
    uint32_t proj_ref = 0;                    // Entity ref to project (0 = realm-wide access)
    uint8_t clrn = 0;                         // Max Schutzstufe this keycard can access (0-9)
    uint16_t perm = 0;                        // Permission bitflags (PERM_READ|PERM_WRITE|...)
    uint64_t expires_at = 0;                  // Unix timestamp of expiry (0 = no expiry)
};

}  // namespace ase::storage
