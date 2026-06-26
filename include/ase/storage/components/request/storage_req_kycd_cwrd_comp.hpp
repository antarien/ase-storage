#pragma once

/**
 * ASE ECS COMPONENT
 *
 * @file        storage_req_kycd_cwrd_comp.hpp
 * @brief       StorageReqKycdCwrdComponent - Requested codeword grant on a keycard request
 * @description Entity-per-Item: one entity per codeword to grant on the issued keycard.
 *              Emplaced by the operator edge-keycard mint route
 *              (plugins/ase-pl-edge-webserver, L4, SDK/Hub only); req_ref points at the
 *              keycard request entity (StorageReqKycdComponent owner).
 *              StorageKycdReqDrnSystem matches req_ref against the request entity it is
 *              draining and emplaces a StorageKycdCwrdComponent on the new token entity
 *              for each match. ABSENCE of any such entity = no codewords granted
 *              (legacy auth-gate keycard flow, unchanged).
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
 * @brief StorageReqKycdCwrdComponent - One requested codeword for a keycard request
 *
 * Entity-per-Item: one entity per codeword to grant on a single keycard request.
 * Example: minting a keycard with "ART" + "AUDIO" = 2 entities sharing the same req_ref.
 * Drained by StorageKycdReqDrnSystem into StorageKycdCwrdComponent on the token entity.
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageReqKycdCwrdComponent {
    uint32_t req_ref = 0;                     // Entity ref to parent keycard request entity
    char cwrd[32] = {};                       // Codeword string to grant (e.g. "ART", "BINARY")
};

}  // namespace ase::storage
