#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_wflw_retr_comp.hpp
 * @brief       StorageWflwRetrComponent - one retired build awaiting retention cleanup
 * @description One entity per asset whose workflow label transitioned to "retired"
 *              (Entity-per-Item), created by StorageWflwTranSystem on the apply.
 *              StorageWflwClnSystem (Observation) deletes the asset + its companion
 *              artifacts once the 90-day retention (WFLW_RETIRED_RETENTION_S)
 *              elapses — the cleanup, not the quota ceiling, keeps the realm small.
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
 * @brief StorageWflwRetrComponent - one retired build awaiting retention cleanup
 *
 * One entity per retired asset (Entity-per-Item). Carries the realm-relative
 * path (the on-disk delete target plus its companion suffixes), the retiring
 * rule ref (destroyed together with the files) and the retire timestamp the
 * retention window counts from.
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageWflwRetrComponent {
    uint32_t rule_ref = 0;                    // Entity ref to the retired StorageAcssRuleComponent
    char path[256] = {};                      // Realm-relative asset path (delete target)
    uint64_t retired_at = 0;                  // Unix time the retired label was applied
};

}  // namespace ase::storage
