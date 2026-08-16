#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_lnk_idn_comp.hpp
 * @brief       StorageLnkIdnComponent - Lattice link endpoints as comparable hashes
 * @description The realms a lattice link joins, in the ONLY form the ladder compares.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @parity      shared
 *
 * PARITAETS-ENTSCHEIDUNG (WS-0.4, ausdruecklich getroffen)
 *   StorageLatLnkComponent - der Datensatz mit source_realm und target_realm - steht in
 *   modules/ase-storage/codegen.json unter components.shared. Die Kante erreicht den
 *   Client als Text; ihre Endpunkte sind Realm-Identitaeten, und der Client verbindet
 *   sie ueber denselben Hash, unter dem die Realms bei ihm liegen. Bliebe der Zwilling
 *   server_only, bekaeme der Client die Kante ohne beide Enden: ein Gitter aus Linien,
 *   die an keinem Knoten ankommen - der Verlust waere ein leeres Bild, keine Fehlzeile.
 *   Identitaet folgt ihrem Datensatz.
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
 * @brief StorageLnkIdnComponent - Lattice link endpoints, comparable in one 32-bit test
 *
 * Both endpoints are realm identities, and identity is a lookup: hashes, never
 * characters (WRFL_ASE_STRING_HANDLING Section 3). The same hash indexes the link,
 * so the bucket key and the stored identity are one number - a link cannot end up
 * filed under one realm while claiming another.
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageLnkIdnComponent {
    uint32_t source_realm_hash = 0;           // entt::hashed_string of StorageLatLnkComponent::source_realm
    uint32_t target_realm_hash = 0;           // entt::hashed_string of StorageLatLnkComponent::target_realm
    uint32_t prefix_hash = 0;                 // entt::hashed_string of the shared path prefix, wildcard stripped
    uint32_t prefix_len = 0;                  // Length of that prefix (0 = the link shares nothing)
};

}  // namespace ase::storage
