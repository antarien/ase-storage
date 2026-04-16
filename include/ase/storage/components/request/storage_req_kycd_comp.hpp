#pragma once

/**
 * ASE ECS COMPONENT
 *
 * @file        storage_req_kycd_comp.hpp
 * @brief       StorageReqKycdComponent - Keycard issuance request (HTTP notify payload)
 * @description Emplaced by plugins/ase-pl-webserver auth_routes
 *              POST /api/auth/keycard-issued after HMAC-verifying the notify
 *              body from clients/aut-client-web.  Consumed by
 *              StorageKycdReqDrnSystem which creates the token entity
 *              (StorageStaIdnComponent + StorageStaKycdComponent +
 *              StorageKycdPendTag) and destroys the request entity.
 *              The request entity additionally carries hub::HubStgKycdPendTag
 *              so Hub systems can discover it without importing L3.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    communication/request
 * @created     2026-04-16
 * @modified    2026-04-16
 * @version     1.0.0
 *
 * ECS COMPONENT COMPLIANCE (15 Checkpoints)
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
 * [ ] Lookup-only strings use uint32_t hash (entt::hashed_string)
 * [ ] NO Entity-per-Character (strings are single attributes, not N-Items!)
 * [ ] NO std::shared_ptr in components (use Flyweight Pattern via ctx!)
 * [ ] NO void* in components (use Flyweight Pattern via ctx!)
 * [ ] NO uint64_t as pointer concept (use uint32_t ID + ResourceManager via ctx!)
 * [ ] External library objects (shared_ptr, handles) in ResourceManager via ctx()
 * [ ] Component stores ONLY primitive ID (uint32_t) referencing external resource
 */

#include <cstdint>

namespace ase::storage {

/**
 * StorageReqKycdComponent - Keycard issuance request payload
 *
 * Emplaced by plugins/ase-pl-webserver auth_routes after HMAC-verifying the
 * notify body from clients/aut-client-web /auth/ws-ticket issuance.
 * Consumed by StorageKycdReqDrnSystem which instantiates the token entity
 * and destroys the request entity in the same tick (collect-then-destroy).
 */
struct StorageReqKycdComponent {
    char user_id[64] = {};          // MongoDB ObjectId hex of the authenticating user
    uint32_t user_id_hash = 0;      // FNV-1a32 of user_id (matches entt::hashed_string)
    uint64_t authenticated_at = 0;  // Unix timestamp (seconds) of ticket issuance
    uint64_t expires_at = 0;        // Unix timestamp (seconds) when ticket expires
    uint8_t clearance = 0;          // Clearance level carried by this keycard (0-9)
};

}  // namespace ase::storage
