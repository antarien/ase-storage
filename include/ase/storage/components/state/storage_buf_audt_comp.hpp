#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_buf_audt_comp.hpp
 * @brief       StorageBufAudtComponent - Audit entry buffer for batch persistence
 * @description One entity per access decision (grant or deny) with full context.
 *              Batch-written to MongoDB by StgAudtWrtSystem at 1Hz.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @created     2026-04-04
 * @modified    2026-04-05
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
 * @brief StorageBufAudtComponent - Audit entry for every access decision
 *
 * Created by StgAcssChkSystem after every grant or deny decision.
 * Tagged with StorageAudtPendTag until StgAudtWrtSystem persists to MongoDB.
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageBufAudtComponent {
    uint32_t relm_ref = 0;                    // Entity ref to realm where access was attempted
    uint32_t proj_ref = 0;                    // Entity ref to project (0 = realm-level access)
    char user_id[64] = {};                    // User who attempted access (MongoDB ObjectId hex)
    uint8_t action = 0;                       // What was attempted (AUD_READ, AUD_WRITE, etc.)
    char path[256] = {};                      // Asset path that was accessed
    uint64_t timestamp = 0;                   // Unix timestamp of the access attempt
    uint8_t result = 0;                       // Outcome (AUD_GRANTED, AUD_DENIED, AUD_ESCALATED)
    char reason[64] = {};                     // Deny reason (e.g. "missing_cwrd(SHADER)")
};

}  // namespace ase::storage
