#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_lat_lnk_comp.hpp
 * @brief       StorageLatLnkComponent - What a lattice link grants
 * @description WHAT a cross-realm bridge hands over: the shared path prefix, the
 *              highest protection level reachable through it, the permission bits,
 *              an optional codeword and the expiry. WHO the two realms are and
 *              whether both consented lives in the sibling StorageLnkCnstComponent
 *              on the same entity - split 2026-08-19, the God-Component gate allows
 *              five fields and this row carried nine. StorageAcssChkSystem reads
 *              both rows together: consent first, then these terms.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @created     2026-04-04
 * @modified    2026-08-19
 * @version     1.1.0
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
 * @brief StorageLatLnkComponent - the terms of a cross-realm sharing bridge
 *
 * The link is directed: the source realm shares path_prefix with the target realm.
 * These fields say how far that sharing reaches - never whether it is allowed to
 * happen at all. That answer is StorageLnkCnstComponent (bilateral consent) and is
 * checked before any of these terms are looked at.
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageLatLnkComponent {
    char path_prefix[256] = {};               // Shared path glob (e.g. "shared/shaders/*")
    uint8_t max_clearance = 0;                // Max Schutzstufe accessible via this link (0-9)
    uint16_t permissions = 0;                 // Bitflags: PERM_READ or PERM_READ | PERM_WRITE
    char codeword[32] = {};                   // Optional codeword required for this link
    uint64_t expires_at = 0;                  // Unix timestamp (0 = no expiry)
};

}  // namespace ase::storage
