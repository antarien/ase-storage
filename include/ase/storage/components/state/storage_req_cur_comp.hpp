#pragma once

/**
 * ASE ECS COMPONENT
 *
 * @file        storage_req_cur_comp.hpp
 * @brief       StorageReqCurComponent - Curation command request core
 * @description WHAT a curation request asks: the curation key to operate on, the
 *              project the key belongs to, the CUR_ACT_* action and the requesting
 *              user. The action parameters (rating, target tag, notes - exactly
 *              one applies per action) live in the sibling StorageReqCurPrmComponent
 *              on the same entity - split 2026-08-19, the God-Component gate allows
 *              five fields and this row carried seven.
 *
 *              ERZEUGER-INVARIANTE: jede Anfrage-Entity traegt Stamm UND Prm-Zeile.
 *              Der fruehere Erzeuger curator_routes.cpp (L5) ist geloescht; der
 *              dokumentierte kuenftige Weg ist die Hub-Migration nach
 *              ase-pl-webserver (MIG_ASE_KERNEL_DLOPEN.md Phase 4b/4c,
 *              STG_CUR_REQ_*-Schluessel). Wer ihn baut, emplaced BEIDE Zeilen -
 *              StorageCurPrcSystem filtert view<Stamm, Prm, CurReqTag> und sieht
 *              halbe Zeilen schlicht nicht.
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
 * StorageReqCurComponent - which key, which project, which action, who asks
 */
struct StorageReqCurComponent {
    char key[160] = {};                       // Curation key to operate on
    uint32_t project_ref = 0;                 // Entity ref to EngineStaProjComponent (systems set value)
    uint8_t action = 0;                       // CUR_ACT_* from types.hpp
    char user_id[64] = {};                    // Requesting user ID
};

}  // namespace ase::storage
