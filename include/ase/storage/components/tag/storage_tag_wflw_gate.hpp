#pragma once

/**
 * =============================================================================
 * ASE ECS COMPONENT (TAG)
 * =============================================================================
 *
 * @file        storage_tag_wflw_gate.hpp
 * @brief       StorageWflwGateTag - released-gate artifact check outstanding
 * @description Marker for promote requests targeting "released" whose companion
 *              artifacts (.sig/.sha256/.spdx.json/.smoke) are not yet verified
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    tag
 * @created     2026-07-11
 * @modified    2026-07-11
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
 * @brief StorageWflwGateTag - released-gate artifact check outstanding
 *
 * State: Request targets EDGE_LABEL_RELEASED; .sig/.sha256/.spdx.json/.smoke
 *        presence beside the asset is not yet verified
 * Filter: Used by StorageWflwGateSystem (Tag-filtered artifact check);
 *         StorageWflwTranSystem EXCLUDES entities still carrying it
 * Added: StorageWflwDrnSystem when the drained target label is "released"
 * Removed: StorageWflwGateSystem — removed on pass; entity destroyed on fail
 */
struct StorageWflwGateTag {};

}  // namespace ase::storage
