#pragma once

/**
 * =============================================================================
 * ASE ECS COMPONENT (TAG)
 * =============================================================================
 *
 * @file        storage_tag_kycd_rev_pst.hpp
 * @brief       StorageKycdRevPstTag - Keycard revocation already durably emitted
 * @description Idempotency marker on a revoked keycard whose durable revoke
 *              record has been emitted to the Replica, so the revocation is
 *              shipped exactly once.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    tag
 * @created     2026-06-24
 * @modified    2026-06-24
 * @version     1.0.0
 *
 * ECS TAG COMPLIANCE
 *
 * [ ] DATA fields ONLY - No methods (empty struct for tags)
 * [ ] NO .cpp file - Header-only
 * [ ] ONLY zero-initialization - N/A (no fields)
 * [ ] No magic numbers in defaults - N/A (no fields)
 * [ ] Entity references - N/A (no fields)
 * [ ] Single responsibility - N/A (marker only)
 * [ ] No God-Component - N/A (no fields)
 * [ ] Large data in registry.ctx() - N/A (Tags have no data)
 * [ ] Tag structs end with Tag suffix
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [ ] Struct name: Remove tag_ from middle, add Tag suffix
 * [ ] 1 File = 1 Component
 * [ ] File in tag/ subfolder (with optional deeper hierarchy)
 * [ ] Per-entity runtime values use state/ components (NOT types.hpp!)
 * [ ] SHARED components listed in codegen.json components.shared
 * [ ] Pointer components in codegen.json components.server_only
 * [ ] Tag replaces `bool is_*` or `bool has_*` field in Component
 * [ ] Tag replaces `uint8_t *_type` field with if-chain dispatch
 * [ ] Systems use View filter instead of if-else inside loop
 * [ ] INCLUDE: registry.view<Component, ThisTag>()
 * [ ] EXCLUDE: registry.view<Component>(entt::exclude<ThisTag>)
 * [ ] NO if (entity.has<Tag>) inside loop - use filtered View!
 * [ ] NO switch/case on type - use separate View per Tag!
 * [ ] Each state = separate Tag + separate View in System
 * [ ] N-item support via Entity-per-Item + Tags
 *
 * =============================================================================
 */

namespace ase::storage {

/**
 * @brief StorageKycdRevPstTag - Keycard revocation already durably emitted
 *
 * State: Durable revoke record (op=KYCD_PST_OP_REVOKE) emitted to the Replica
 * Filter: StorageKycdRevSystem excludes already-emitted revocations
 * Added: StorageKycdRevSystem after emitting the durable revoke signal
 * Removed: never (terminal idempotency marker for the revocation)
 */
struct StorageKycdRevPstTag {};

}  // namespace ase::storage
