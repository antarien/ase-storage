#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        storage_wflw_rcv_tag.hpp
 * @brief       StorageWflwRcvTag - promote request received, not yet staged
 * @description Marker on a StorageReqWflwTranComponent request that one of the two doors of
 *              the workflow promote has filled: StorageEdgeWflwFwdRcvSystem from the
 *              Replica-forwarded operator command (frame 113) or StorageWflwDrnSystem from a
 *              hub row HUB_STG_WFLW_REQ. StorageWflwStgeSystem stages every request that
 *              carries it, so the staging stands in one place for both doors. Server only:
 *              the request it marks is an operator command on the dist tier, and the system
 *              that consumes it publishes the hub verdict STG_WFLW_RES.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    tag
 * @parity      server_only
 * @usecase     usecase:100329 - stage workflow promotion
 * @created     2026-10-12
 * @modified    2026-10-12
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
 * @brief StorageWflwRcvTag - promote request received, not yet staged
 *
 * State: StorageReqWflwTranComponent filled with path, target label and requester;
 *        no StorageWflwPendTag yet, no STG_WFLW_RES verdict published
 * Filter: registry.view<StorageReqWflwTranComponent, StorageWflwRcvTag>() in StorageWflwStgeSystem
 * Added: StorageEdgeWflwFwdRcvSystem (frame 113 promote) and StorageWflwDrnSystem (hub row)
 * Removed: StorageWflwStgeSystem when it stages the request
 */
struct StorageWflwRcvTag {};

}  // namespace ase::storage
