#pragma once

/**
 * =============================================================================
 * ASE ECS COMPONENT (TAG)
 * =============================================================================
 *
 * @file        storage_tag_kycd_snt.hpp
 * @brief       StorageKycdSntTag - Keycard sync frame has been sent to Replica
 * @description Marker for client session entities for which Engine has already
 *              dispatched a BIN_MSG_SESSION_KYCD_SYNC (status=validate) frame
 *              to Replica via the binary WebSocket :9003 bridge. Used by
 *              StorageKycdSyncSndSystem for idempotence: sessions carrying
 *              this tag are not resent every tick, sessions that lose
 *              StorageKycdVldTag trigger a status=invalidate frame and the
 *              tag is removed so a subsequent re-validation will resync.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    tag
 * @created     2026-04-16
 * @modified    2026-04-16
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
 * @brief StorageKycdSntTag - Session already synchronised to Replica
 *
 * State: Engine has pushed a BIN_MSG_SESSION_KYCD_SYNC (validate) frame for
 *        this session entity. Replica owns the matching Session* components.
 * Filter: StorageKycdSyncSndSystem uses this tag both as an EXCLUDE filter
 *         on the validate-path (skip already-sent sessions) and as an
 *         INCLUDE filter on the invalidate-path (find sessions whose
 *         keycard has been revoked).
 * Added: StorageKycdSyncSndSystem after the validate frame is enqueued.
 * Removed: StorageKycdSyncSndSystem after the invalidate frame is enqueued.
 */
struct StorageKycdSntTag {};

}  // namespace ase::storage
