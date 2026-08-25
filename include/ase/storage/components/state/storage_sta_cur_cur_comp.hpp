#pragma once

/**
 * ASE ECS COMPONENT
 *
 * @file        storage_sta_cur_cur_comp.hpp
 * @brief       StorageStaCurCurComponent - Address of one curation row
 * @description WHICH asset a curation row is about: the curation key string, its
 *              hash (the ONLY form ever compared - the access index composes its
 *              bucket key from project_ref + key_hash) and the owning project.
 *              The curator's verdict (rating, notes, who, when) lives in the
 *              sibling StorageCurAsmtComponent on the same entity - split
 *              2026-08-19, the God-Component gate allows five fields and this row
 *              carried seven. StorageCurPrcSystem creates both rows idempotently
 *              and registers the address in the index immediately.
 *
 * sta = state, cur = curation, cur = current
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @created     2026-04-06
 * @modified    2026-08-19
 * @version     1.1.0
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
 * StorageStaCurCurComponent - which asset the curation row addresses
 */
struct StorageStaCurCurComponent {
    char key[160] = {};                       // Curation key ("IMG_ID" or "IMG_ID::v3")
    uint32_t key_hash = 0;                    // entt::hashed_string of key - the ONLY form compared
    uint32_t project_ref = 0;                 // Entity ref to EngineStaProjComponent (systems set value)
};

}  // namespace ase::storage
