#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        storage_lnk_exp_tag.hpp
 * @brief       StorageLnkExpTag - Lattice link has expired
 * @description Marker for lattice link entities whose lifetime (expires_at) has elapsed
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    tag
 * @parity      shared
 * @usecase     usecase:101533 - withdraw a lapsed lattice bond
 * @actor       Horizontal Layer Steward (HLSTEWARD)
 * @created     2026-10-29
 * @modified    2026-10-29
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
 */

namespace ase::storage {

/**
 * @brief StorageLnkExpTag - Lattice link has expired (wall clock past expires_at)
 *
 * State: The bridge between two realms has ended; it grants no cross-realm access any more
 * Filter: registry.view<StorageLatLnkComponent, StorageLnkIdnComponent>(
 *         entt::exclude<StorageLnkExpTag>) - the links that still stand
 * Added: StorageLatcSyncSystem when the wall clock exceeds StorageLatLnkComponent::expires_at
 * Removed: never - the ended link stays as the record of the realms it once bridged
 *
 * The link carries exactly one producer of this tag (StorageLatcSyncSystem). The ACL check of
 * StorageAcssChkSystem refuses the same link by comparing expires_at against the same clock,
 * so both answer alike; the tag turns that per-request answer into a state the client sees
 * and ends the link exactly once.
 */
struct StorageLnkExpTag {};

}  // namespace ase::storage
