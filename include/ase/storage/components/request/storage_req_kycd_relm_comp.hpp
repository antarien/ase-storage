#pragma once

/**
 * ASE ECS COMPONENT
 *
 * @file        storage_req_kycd_relm_comp.hpp
 * @brief       StorageReqKycdRelmComponent - Realm/permission extension on a keycard request
 * @description Optional sibling component co-located on the SAME request entity as
 *              StorageReqKycdComponent. Emplaced by the operator edge-keycard mint
 *              route (plugins/ase-pl-edge-webserver, L4, SDK/Hub only) so the drain
 *              can carry a realm ref + permission bitflags onto the issued keycard.
 *              ABSENCE of this component = legacy auth-gate keycard flow, unchanged
 *              (StorageKycdReqDrnSystem defaults relm_ref/perm to 0 when missing).
 *              Split from StorageReqKycdComponent to respect the 5-field component
 *              limit (God-Component rule) — additive, separate concern.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    communication/request
 * @created     2026-06-24
 * @modified    2026-06-24
 * @version     1.0.0
 *
 * ECS COMPONENT COMPLIANCE (15 Checkpoints)
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
 * @brief StorageReqKycdRelmComponent - Realm + permission extension for a keycard request
 *
 * Co-located with StorageReqKycdComponent on the request entity. The operator mint
 * sets relm_ref + perm here; StorageKycdReqDrnSystem copies them onto
 * StorageStaKycdComponent. Missing component = legacy auth flow (defaults 0).
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageReqKycdRelmComponent {
    uint32_t relm_ref = 0;          // Realm entity ref this keycard grants access to
    uint16_t perm = 0;              // Permission bitflags (PERM_READ | PERM_WRITE | ...)
};

}  // namespace ase::storage
