#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_sta_tkn_comp.hpp
 * @brief       StorageStaTknComponent - Flyweight reference to raw keycard token
 * @description Stores only the uint32_t lookup ID into StorageResourceManager.
 *              Actual JWT string lives in the ResourceManager (OOP bridge).
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
 * @brief StorageStaTknComponent - Flyweight ID referencing JWT in ResourceManager
 *
 * The actual JWT string (up to 2048 bytes) is stored in StorageResourceManager.
 * This component holds only the uint32_t lookup ID (Flyweight Pattern).
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageStaTknComponent {
    uint32_t token_id = 0;                    // Flyweight ID into StorageResourceManager token store
    uint32_t client_id = 0;                   // Network client ID from WebRTC callback
};

}  // namespace ase::storage
