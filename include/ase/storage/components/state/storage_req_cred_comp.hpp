#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_req_cred_comp.hpp
 * @brief       StorageReqCredComponent - The caller's credentials on an access request
 * @description WHO is asking, with what standing: the user id, the clearance and
 *              the permission bits, all three resolved from the auth header before
 *              the request entity exists. They are what the caller BRINGS - never
 *              what the caller wants, which is StorageReqAcssComponent on the same
 *              entity (realm, project, path, action). Split 2026-08-19 because the
 *              God-Component gate allows five fields and the combined row carried
 *              seven. StorageAcssChkSystem weighs these three against the realm's
 *              rules; an owner preset overrides clrn and perm without touching them.
 *
 * req = request, cred = credential
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
 * @brief StorageReqCredComponent - what the caller brings to an access request
 *
 * An empty user_id is the unauthenticated case and is denied before clearance or
 * permissions are looked at - the two numbers are meaningless without a name to
 * attach them to.
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageReqCredComponent {
    char user_id[64] = {};                    // User ID from auth header (pre-resolved by route)
    uint8_t clrn = 0;                         // Clearance from auth header (pre-resolved by route)
    uint16_t perm = 0;                        // Permissions from auth header (pre-resolved by route)
};

}  // namespace ase::storage
