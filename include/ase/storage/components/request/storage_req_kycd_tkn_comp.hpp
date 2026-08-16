#pragma once

/**
 * ASE ECS COMPONENT (REQUEST)
 *
 * @file        storage_req_kycd_tkn_comp.hpp
 * @brief       StorageReqKycdTknComponent - The keycard a drained request has minted
 * @description Written onto the request the moment its keycard entity exists, so the
 *              request's codeword children can reach that keycard in O(1) instead of
 *              being searched for once per request.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    communication/request
 * @parity      server_only
 *
 * PARITAETS-ENTSCHEIDUNG (WS-0.4, ausdruecklich getroffen)
 *   Diese Komponente lebt nur INNERHALB eines Ticks des Keycard-Drains: sie haelt den
 *   Elternbezug, ueber den die Codewort-Kinder ihre frisch gepraegte Keycard finden, und
 *   verschwindet mit der Anfrage im selben Frame. Ein becsy-Zwilling waere ein Zustand,
 *   den niemand schreibt und niemand liest. Die Keycard selbst erreicht den Client ueber
 *   ihre eigene Komponente - hier geht nichts verloren.
 * @created     2026-08-16
 * @modified    2026-08-16
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
 * @brief StorageReqKycdTknComponent - the keycard this request minted
 *
 * The drain used to walk EVERY requested codeword for EVERY request just to find the
 * ones belonging to it (WS-K.2c): the parent was known and the children were searched
 * for. This component turns the search around. The request records its keycard the
 * moment it exists, so the SECOND pass walks the codeword children ONCE and reaches
 * the parent by entity reference - a try_get, not a scan.
 *
 * Its presence is also the drain's completion marker: a request carrying it has minted
 * and is destroyed after the codeword pass, never during it.
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageReqKycdTknComponent {
    uint32_t kycd_ref = 0;                    // Entity ref to the keycard entity this request minted
};

}  // namespace ase::storage
