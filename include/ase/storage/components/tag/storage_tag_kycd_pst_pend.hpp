#pragma once

/**
 * =============================================================================
 * ASE ECS COMPONENT (TAG)
 * =============================================================================
 *
 * @file        storage_tag_kycd_pst_pend.hpp
 * @brief       StorageKycdPstPendTag - Minted keycard not yet durably persisted
 * @description Marker for newly minted StorageStaKycdComponent entities whose
 *              durable record has not yet been emitted to the Replica.
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
 * @brief StorageKycdPstPendTag - Keycard needs durable persist round-trip
 *
 * State: Keycard minted in-memory but its durable record not yet emitted
 * Filter: Used by StorageKycdPstEmitSystem to build the Replica persist record
 * Added: StorageKycdReqDrnSystem when an operator/customer keycard is minted
 * Removed: StorageKycdPstEmitSystem after the durable record is emitted
 */
struct StorageKycdPstPendTag {};

}  // namespace ase::storage
