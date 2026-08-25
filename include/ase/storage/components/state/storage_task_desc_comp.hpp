#pragma once

/**
 * ASE ECS COMPONENT
 *
 * @file        storage_task_desc_comp.hpp
 * @brief       StorageTaskDescComponent - Human-readable description of one task
 * @description WHO a need-to-know task is assigned to and what it covers, as the
 *              portal shows and persists it: the assignee user id, the task name
 *              and the asset path glob. Split out of StorageStaTaskComponent
 *              2026-08-19 - the God-Component gate allows five fields and the
 *              combined row carried six. The ladder never compares these strings:
 *              it matches the hashes in the existing StorageTaskIdnComponent
 *              sibling; the active window lives in the sta_task sibling. Every
 *              creator emplaces all three rows (see the sibling's creator
 *              invariant).
 *
 *              THE PARITY BELOW IS A PRECAUTION, NOT A FINDING. Measured 2026-08-20:
 *              no system under src/ reads or writes this row, so the creator
 *              invariant above describes an intent nothing carries out yet. The
 *              need-to-know scoping feature is unfinished - no producer, no consumer -
 *              and the decision is to be retaken WITH the feature. server_only is the
 *              direction in which being wrong costs nothing: it declares nothing to
 *              the client, whereas shared would export a component the becsy tree
 *              could never fill.
 *
 *              THE ROW STAYS regardless: types.hpp carries the full vocabulary -
 *              TASK_ACTIVE/TASK_DONE/TASK_LAPSED, MAX_TASK_NAME, the ACL section on
 *              path_pattern semantics, and SERIAL_TYP_STG_TSK = 386. A serialization
 *              id means the type was meant for the wire; that is a commitment, not a
 *              leftover.
 *
 * desc = description (structure/datatype/textual)
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

namespace ase::storage {

/**
 * StorageTaskDescComponent - the strings people read; the ladder reads hashes
 */
struct StorageTaskDescComponent {
    char assignee[64] = {};                   // User ID assigned to this task
    char task_name[128] = {};                 // Human-readable task name
    char path_pattern[256] = {};              // Asset path glob (e.g. "assets/story/chapter-03/*")
};

}  // namespace ase::storage
