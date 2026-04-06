#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_sta_relm_comp.hpp
 * @brief       StorageStaRelmComponent - Realm entity core data
 * @description Core data for an isolated security realm on the platform.
 *              Realm type via Tags: StorageRelmPersonalTag / StorageRelmOrgTag / StorageRelmPublicTag
 *              Realm status via Tags: StorageRelmActiveTag / StorageRelmSuspendedTag / StorageRelmArchivedTag
 *              Concealment via Tag: StorageRelmConcealTag (present = hidden from non-members)
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @created     2026-04-04
 * @modified    2026-04-04
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
 * @brief StorageStaRelmComponent - Isolated security realm core data
 *
 * Type discrimination via Tags (NOT uint8_t field):
 *   StorageRelmPersonalTag  = per-user workspace
 *   StorageRelmOrgTag       = studio/team workspace
 *   StorageRelmPublicTag    = engine defaults
 *
 * Status discrimination via Tags:
 *   StorageRelmActiveTag    = normal operation
 *   StorageRelmSuspendedTag = disabled by admin
 *   StorageRelmArchivedTag  = read-only before deletion
 *
 * Concealment via Tag:
 *   StorageRelmConcealTag   = hidden from non-members (default for org/personal)
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageStaRelmComponent {
    char id[64] = {};                         // Realm path identifier (e.g. "org/adg" or "public")
    char name[128] = {};                      // Display name (e.g. "Antarien Dev Group")
    char owner[64] = {};                      // Creator user ID (MongoDB ObjectId hex)
    uint8_t default_protection = 0;           // Default Schutzstufe for new assets (0-9)
    uint8_t tier = 0;                         // License tier (0=indie, 1=pro, 2=enterprise)
};

}  // namespace ase::storage
