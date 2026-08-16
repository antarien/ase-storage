#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_relm_idn_comp.hpp
 * @brief       StorageRelmIdnComponent - Realm identity as comparable hashes
 * @description The realm's id and owner in the ONLY form the ladder compares.
 *              StorageStaRelmComponent keeps the readable strings for audit
 *              records and filesystem paths; nothing on a hot path reads them.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @parity      shared
 *
 * PARITAETS-ENTSCHEIDUNG (WS-0.4, ausdruecklich getroffen)
 *   StorageStaRelmComponent - der Datensatz, dessen Zeichenketten hier gehasht sind -
 *   steht in modules/ase-storage/codegen.json unter components.shared. Das Realm
 *   erreicht den Client also samt id und owner als Text. Dieser Zwilling traegt den
 *   Vergleichsschluessel derselben Identitaet. Waere er server_only, bekaeme der Client
 *   das Realm ohne seinen Schluessel: Link-Endpunkt, Keycard-Inhaber und Hub-Owner
 *   werden drueben ueber den Hash zugeordnet, nicht ueber den Text, und jede dieser
 *   Zuordnungen liefe ins Leere - ohne Fehlermeldung, weil eine fehlende Component
 *   drueben schlicht keine Query trifft. Identitaet folgt ihrem Datensatz.
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
 * @brief StorageRelmIdnComponent - Realm identity, comparable in one 32-bit test
 *
 * Identity is a LOOKUP question, and a lookup compares hashes, never characters
 * (WRFL_ASE_STRING_HANDLING Section 3). Splitting the hashes out of
 * StorageStaRelmComponent keeps that component from becoming a God-Component and
 * makes the split honest: the strings are the record, these are the identity.
 *
 * Written by every system that writes the matching string in StorageStaRelmComponent,
 * in the same statement. A realm carrying the string component but not this one is a
 * bug the ladder reports rather than absorbs.
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageRelmIdnComponent {
    uint32_t id_hash = 0;                     // entt::hashed_string of StorageStaRelmComponent::id
    uint32_t owner_hash = 0;                  // entt::hashed_string of StorageStaRelmComponent::owner
};

}  // namespace ase::storage
