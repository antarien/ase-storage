#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_audt_outc_comp.hpp
 * @brief       StorageAudtOutcComponent - What an audited access attempt tried and how it ended
 * @description The three AUD_* fields of one audit row: the attempted operation
 *              (AUD_READ, AUD_WRITE, AUD_PROMOTE, AUD_DELETE), the outcome
 *              (AUD_GRANTED, AUD_DENIED, AUD_ESCALATED) and the machine-readable
 *              deny reason. They share one vocabulary from types.hpp and one
 *              writer moment: the deciding system fills all three at once. The
 *              context of the attempt - realm, project, user, path, time - lives
 *              in StorageBufAudtComponent on the same entity, split 2026-08-19
 *              because the God-Component gate allows five fields and the combined
 *              row carried eight. Every producer emplaces both rows together and
 *              tags the entity StorageAudtPendTag; StorageAudtWritSystem drains
 *              them to MongoDB at 1Hz.
 *
 * audt = audit, outc = outcome
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @parity      server_only
 * @created     2026-08-19
 * @modified    2026-08-20
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
 * @brief StorageAudtOutcComponent - the attempted operation and its verdict
 *
 * The reason string is only meaningful for AUD_DENIED and AUD_ESCALATED; a
 * granted row leaves it empty. Consumers read this row beside
 * StorageBufAudtComponent in one view - never by looking the entity up again.
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageAudtOutcComponent {
    uint8_t action = 0;                       // What was attempted (AUD_READ, AUD_WRITE, etc.)
    uint8_t result = 0;                       // Outcome (AUD_GRANTED, AUD_DENIED, AUD_ESCALATED)
    char reason[64] = {};                     // Deny reason (e.g. "missing_cwrd(SHADER)")
};

}  // namespace ase::storage
