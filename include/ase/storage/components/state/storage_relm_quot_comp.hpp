#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_relm_quot_comp.hpp
 * @brief       StorageRelmQuotComponent - A realm's storage ceiling and measured usage
 * @description The storage accounting of one realm: the ceiling in bytes, the last
 *              measured usage, and when that measurement was taken. All three are
 *              rewritten by StorageQuotChkSystem on its own cadence - the scan
 *              paces itself off usage_scanned_at, which is why the pacing state is
 *              DATA here and the system stays stateless. WHAT the realm is - id,
 *              name, owner, protection default, tier - stays in
 *              StorageStaRelmComponent on the same entity and does not move when a
 *              file is written. Split 2026-08-19 because the God-Component gate
 *              allows five fields and the combined row carried eight.
 *
 * relm = realm, quot = quota
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
 * @brief StorageRelmQuotComponent - the byte accounting of one realm
 *
 * quota_bytes of 0 means no ceiling is configured and StorageQuotChkSystem skips
 * the realm entirely - it does not scan a realm it could never report over.
 * used_bytes is the filesystem's answer, never a running total kept by hand.
 *
 * @hub_reads  none
 * @hub_writes STG_RELM_USED_HI, STG_RELM_USED_LO, STG_RELM_QUOTA_HI, STG_RELM_QUOTA_LO
 */
struct StorageRelmQuotComponent {
    uint64_t quota_bytes = 0;                 // Realm storage ceiling in bytes (seeded from types.hpp, 0 = unset)
    uint64_t used_bytes = 0;                  // Measured realm usage in bytes (StorageQuotChkSystem FS scan)
    uint64_t usage_scanned_at = 0;            // Unix time of the last usage scan (paces the Observation rescan)
};

}  // namespace ase::storage
