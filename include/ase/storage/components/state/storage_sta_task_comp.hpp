#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_sta_task_comp.hpp
 * @brief       StorageStaTaskComponent - Need-to-Know task scoping (Enterprise)
 * @description Task-based access scoping: developer sees only assets within task scope.
 *              Task lifecycle via Tags: StorageTaskActiveTag, StorageTaskDoneTag, etc.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @created     2026-04-05
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
 * @brief StorageStaTaskComponent - Need-to-Know task with scoped asset access
 *
 * Enterprise tier only. Limits developer access to a specific path pattern.
 * Task lifecycle via Tags (NOT uint8_t field):
 *   StorageTaskActiveTag = developer has scoped access
 *   StorageTaskDoneTag   = task finished, access revoked
 *   StorageTaskLapseTag  = deadline passed, access revoked
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageStaTaskComponent {
    uint32_t proj_ref = 0;                    // Entity ref to project this task belongs to
    char assignee[64] = {};                   // User ID assigned to this task
    char task_name[128] = {};                 // Human-readable task name
    char path_pattern[256] = {};              // Asset path glob (e.g. "assets/story/chapter-03/*")
    uint64_t starts_at = 0;                   // Unix timestamp when access begins
    uint64_t expires_at = 0;                  // Unix timestamp when access ends
};

}  // namespace ase::storage
