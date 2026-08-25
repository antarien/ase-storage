#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_buf_audt_comp.hpp
 * @brief       StorageBufAudtComponent - Context of one audited access attempt
 * @description WHO reached for WHAT asset, WHERE and WHEN: realm, project, user,
 *              path and the timestamp of the attempt. What was attempted and how
 *              it was decided lives in StorageAudtOutcComponent on the same
 *              entity - split 2026-08-19, the God-Component gate allows five
 *              fields and the combined row carried eight. Both rows are emplaced
 *              together with StorageAudtPendTag and batch-written to MongoDB by
 *              StorageAudtWritSystem at 1Hz.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @created     2026-04-04
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
 * @brief StorageBufAudtComponent - the context of one audit entry
 *
 * Created by every system that decides an access - StorageAcssChkSystem, the
 * three workflow systems and the edge drain. Tagged with StorageAudtPendTag
 * until StorageAudtWritSystem persists it to MongoDB. Its verdict half is
 * StorageAudtOutcComponent and is read in the same view, never fetched again.
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageBufAudtComponent {
    uint32_t relm_ref = 0;                    // Entity ref to realm where access was attempted
    uint32_t proj_ref = 0;                    // Entity ref to project (0 = realm-level access)
    char user_id[64] = {};                    // User who attempted access (MongoDB ObjectId hex)
    char path[256] = {};                      // Asset path that was accessed
    uint64_t timestamp = 0;                   // Unix timestamp of the access attempt
};

}  // namespace ase::storage
