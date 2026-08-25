#pragma once

/**
 * ASE ECS COMPONENT
 *
 * @file        storage_sta_task_comp.hpp
 * @brief       StorageStaTaskComponent - Active window of one need-to-know task
 * @description WHEN a task grants access and under which project bucket: the
 *              project ref the access index buckets tasks by, and the Unix window
 *              (starts_at/expires_at, 0 = unbounded) the A/ACS ladder checks in
 *              Step 8. The human-readable description (assignee, task name, path
 *              glob) lives in the sibling StorageTaskDescComponent - split
 *              2026-08-19, the God-Component gate allows five fields and this row
 *              carried six. The comparison values the ladder matches against
 *              (assignee_hash, scope_hash/len) already live in the existing
 *              StorageTaskIdnComponent sibling.
 *
 *              ERZEUGER-INVARIANTE: jede Task-Entity traegt Fenster-, Desc- UND
 *              Idn-Zeile. Der fruehere Erzeuger storage_routes.cpp (L5) ist
 *              geloescht; der dokumentierte kuenftige Weg ist die Hub-Migration
 *              nach ase-pl-webserver (MIG_ASE_KERNEL_DLOPEN.md Phase 4b). Wer ihn
 *              baut, emplaced ALLE Geschwister.
 *
 * sta = state, task selbst ist katalogisiert (entity/abstract/task)
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
 * StorageStaTaskComponent - which project bucket, and when the task is live
 */
struct StorageStaTaskComponent {
    uint32_t proj_ref = 0;                    // Entity ref to project this task belongs to
    uint64_t starts_at = 0;                   // Unix timestamp when access begins
    uint64_t expires_at = 0;                  // Unix timestamp when access ends
};

}  // namespace ase::storage
