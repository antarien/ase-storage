#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_lat_lnk_comp.hpp
 * @brief       StorageLatLnkComponent - Lattice link between two realms
 * @description Bilateral cross-realm bridge for sharing assets between studios.
 *              Both source and target must approve the link (bilateral consent).
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

#include <cstdint>

namespace ase::storage {

/**
 * @brief StorageLatLnkComponent - Cross-realm sharing bridge (bilateral)
 *
 * Lattice link is directed: source realm shares path_prefix with target realm.
 * Both sides must approve (approved_by_source + approved_by_target).
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageLatLnkComponent {
    char source_realm[64] = {};               // Realm ID that shares assets
    char target_realm[64] = {};               // Realm ID that receives access
    char path_prefix[256] = {};               // Shared path glob (e.g. "shared/shaders/*")
    uint8_t max_clearance = 0;                // Max Schutzstufe accessible via this link (0-9)
    uint16_t permissions = 0;                 // Bitflags: PERM_READ or PERM_READ | PERM_WRITE
    char codeword[32] = {};                   // Optional codeword required for this link
    uint8_t approved_by_source = 0;           // 1 = source realm owner approved
    uint8_t approved_by_target = 0;           // 1 = target realm owner approved
    uint64_t expires_at = 0;                  // Unix timestamp (0 = no expiry)
};

}  // namespace ase::storage
