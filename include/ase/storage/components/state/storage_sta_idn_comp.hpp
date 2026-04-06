#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_sta_idn_comp.hpp
 * @brief       StorageStaIdnComponent - Authenticated user identity
 * @description Validated identity extracted from keycard JWT claims.
 *              Placed on client entity after successful keycard verification.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @created     2026-04-04
 * @modified    2026-04-04
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
 * @brief StorageStaIdnComponent - Validated user identity from keycard
 *
 * Developer identity extracted from platform keycard (JWT).
 * Placed on client entity by StgKcdLnkSystem after successful verification.
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageStaIdnComponent {
    char user_id[MAX_OWNER_ID] = {};          // MongoDB ObjectId from JWT "sub" claim
    char email[MAX_EMAIL_LEN] = {};           // Email address from JWT claims
    char display_name[MAX_DISPLAY_NAME] = {}; // User display name from JWT claims
    uint32_t client_id = 0;                   // Network client ID from ase-network WebRTC
    uint32_t active_keycard = 0;              // Entity ref to active keycard entity
    uint64_t authenticated_at = 0;            // Unix timestamp when keycard was validated
};

}  // namespace ase::storage
