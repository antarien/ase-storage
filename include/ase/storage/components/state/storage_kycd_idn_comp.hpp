#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_kycd_idn_comp.hpp
 * @brief       StorageKycdIdnComponent - Keycard holder identity as a comparable hash
 * @description The user a keycard was issued to, in the ONLY form the ladder compares.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @parity      shared
 *
 * PARITAETS-ENTSCHEIDUNG (WS-0.4, ausdruecklich getroffen)
 *   StorageStaKycdComponent - die Keycard, deren issued_to hier gehasht ist - steht in
 *   modules/ase-storage/codegen.json unter components.shared. Die Keycard erreicht den
 *   Client, und derselbe Hash ist der Owner, unter dem sie im Hub veroeffentlicht wird.
 *   Waere dieser Zwilling server_only, haette der Client die Keycard, aber nicht den
 *   Schluessel, mit dem er sie seinem Inhaber zuordnet - "gehoert diese Keycard mir"
 *   waere drueben unbeantwortbar, und die Antwort fehlte lautlos statt zu scheitern.
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
 * @brief StorageKycdIdnComponent - Keycard holder, comparable in one 32-bit test
 *
 * "Does this keycard belong to the requester" is a lookup, so it compares hashes,
 * never characters (WRFL_ASE_STRING_HANDLING Section 3). The same hash is already
 * the hub owner id a keycard publishes under, so identity stays one number across
 * the module instead of one number and one string that must agree.
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageKycdIdnComponent {
    uint32_t issued_to_hash = 0;              // entt::hashed_string of StorageStaKycdComponent::issued_to
};

}  // namespace ase::storage
