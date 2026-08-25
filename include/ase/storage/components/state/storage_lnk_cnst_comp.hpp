#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_lnk_cnst_comp.hpp
 * @brief       StorageLnkCnstComponent - The two realms of a lattice link and their consent
 * @description WHO a lattice link joins and whether each side agreed: the sharing
 *              realm, the receiving realm, and one approval flag per side. A link
 *              only carries access once BOTH flags are set - the bilateral consent
 *              the sibling header has named since 2026-04-04. What the link grants
 *              (path, clearance, permissions, codeword, expiry) lives in
 *              StorageLatLnkComponent on the same entity, the compared hash form in
 *              StorageLnkIdnComponent. Split out 2026-08-19 - the God-Component gate
 *              allows five fields and the combined row carried nine.
 *
 * lnk = link, cnst = consent
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @parity      server_only
 * @created     2026-08-19
 * @modified    2026-08-20
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
 * @brief StorageLnkCnstComponent - the two parties of a lattice link and their consent
 *
 * The link is directed: source shares, target receives. Consent is not - both
 * owners must have said yes before StorageAcssChkSystem lets the link carry a
 * single byte. The realm names are the readable form; the form ever compared is
 * StorageLnkIdnComponent::source_realm_hash / target_realm_hash.
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageLnkCnstComponent {
    char source_realm[64] = {};               // Realm ID that shares assets
    char target_realm[64] = {};               // Realm ID that receives access
    uint8_t approved_by_source = 0;           // 1 = source realm owner approved
    uint8_t approved_by_target = 0;           // 1 = target realm owner approved
};

}  // namespace ase::storage
