#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_buf_wflw_comp.hpp
 * @brief       StorageBufWflwComponent - applied workflow transition awaiting durable persist
 * @description One entity per APPLIED label transition (Entity-per-Item), created by
 *              StorageWflwTranSystem together with StorageWflwPstPendTag. Drained by
 *              StorageWflwPstSystem (Preservation), which serializes it as the
 *              workflow-label document and ships BIN_MSG frame 112 to the Replica
 *              (REPLACE-upsert into storage_workflow_labels keyed {realm,path}),
 *              then destroys the buffer entity. Mirrors the StorageBufAudtComponent
 *              buffer-then-persist pattern.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @created     2026-07-11
 * @modified    2026-07-11
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
 * @brief StorageBufWflwComponent - one applied transition awaiting durable persist
 *
 * One entity per applied workflow-label transition. Serialized by
 * StorageWflwPstSystem into the {"realm","path","label","updated_by",
 * "updated_at"} document for the frame-112 REPLACE-upsert, then destroyed.
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageBufWflwComponent {
    char realm[64] = {};                      // Realm id the asset lives in (upsert key part 1)
    char path[256] = {};                      // Realm-relative asset path (upsert key part 2)
    char label[32] = {};                      // Newly applied workflow label
    char updated_by[64] = {};                 // Operator user_id (keycard attribution)
    uint64_t updated_at = 0;                  // Unix time the transition was applied
};

}  // namespace ase::storage
