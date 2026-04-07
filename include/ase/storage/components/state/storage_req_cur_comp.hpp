#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_req_cur_comp.hpp
 * @brief       StorageReqCurComponent - Curator action request from HTTP
 * @description Incoming request from Integration Layer (curator_routes.cpp).
 *              One entity per request (destroyed after processing).
 *              Action type determines which field is relevant:
 *                CUR_ACT_RATE   → rating field
 *                CUR_ACT_STATUS → target_tag field (which Tag to emplace)
 *                CUR_ACT_NOTES  → notes field
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @created     2026-04-06
 * @modified    2026-04-06
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
 * @brief StorageReqCurComponent - Curator request from HTTP handler
 *
 * Lifecycle: Created by curator_routes.cpp → processed by StorageCurPrcSystem
 *            → tagged StorageCurDoneTag → destroyed in cleanup pass
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageReqCurComponent {
    char key[160] = {};                       // Curation key to operate on
    uint32_t project_ref = 0;                 // Entity ref to EngineStaProjComponent (systems set value)
    uint8_t action = 0;                       // CUR_ACT_* from types.hpp
    uint8_t rating = 0;                       // New rating (for CUR_ACT_RATE)
    uint8_t target_tag = 0;                   // CUR_ST_* value → determines which Tag to emplace
    char notes[256] = {};                     // New notes (for CUR_ACT_NOTES)
    char user_id[64] = {};                    // Requesting user ID
};

}  // namespace ase::storage
