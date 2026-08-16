#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_task_idn_comp.hpp
 * @brief       StorageTaskIdnComponent - Need-to-know assignee as a comparable hash
 * @description The user a need-to-know task is assigned to, in the ONLY form the
 *              ladder compares.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @parity      shared
 *
 * PARITAETS-ENTSCHEIDUNG (WS-0.4, ausdruecklich getroffen)
 *   StorageStaTaskComponent - die Need-to-know-Aufgabe, deren assignee hier gehasht ist -
 *   steht in modules/ase-storage/codegen.json unter components.shared. Die Aufgabe
 *   erreicht den Client; dieser Zwilling traegt den Hash ihres Empfaengers und den
 *   Pfad-Scope, nach dem der Client sie einem Asset zuordnet. Waere er server_only,
 *   saehe der Client jede Aufgabe, aber keine als seine - "ist das meine Aufgabe" ist
 *   ein Hash-Vergleich, und ohne den bliebe die Liste stumm statt leer.
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
 * @brief StorageTaskIdnComponent - Task assignee, comparable in one 32-bit test
 *
 * "Is this task mine" is a lookup, so it compares hashes, never characters
 * (WRFL_ASE_STRING_HANDLING Section 3). The task name and path scope stay strings
 * in StorageStaTaskComponent: one is shown to people, the other is matched by path
 * segment, and neither is an identity.
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageTaskIdnComponent {
    uint32_t assignee_hash = 0;               // entt::hashed_string of StorageStaTaskComponent::assignee
    uint32_t scope_hash = 0;                  // entt::hashed_string of the task path scope, wildcard stripped
    uint32_t scope_len = 0;                   // Length of that scope (0 = the task scopes nothing)
};

}  // namespace ase::storage
