#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        storage_wflw_rjct_tag.hpp
 * @brief       StorageWflwRjctTag - forwarded operator command was refused
 * @description Marker on a StorageReqWflwFwdComponent whose forward frame could not be served:
 *              a malformed frame, a promote without target or attribution, or an operation
 *              byte that names neither promote nor status. The reason stands in the
 *              component; StorageEdgeWflwRplySystem answers with the bad-request status and
 *              that reason. Server only: it marks a dist-tier transport command.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    tag
 * @parity      server_only
 * @usecase     usecase:100317 - relay operator command
 * @created     2026-11-05
 * @modified    2026-11-05
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
 * @brief StorageWflwRjctTag - forwarded operator command was refused
 *
 * State: StorageReqWflwFwdComponent with the refusal reason
 * Filter: registry.view<StorageReqWflwFwdComponent, StorageWflwRjctTag>() in StorageEdgeWflwRplySystem
 * Added: StorageEdgeWflwFwdRcvSystem (malformed frame, missing field, unknown operation)
 * Removed: StorageEdgeWflwRplySystem destroys the entity after the reply
 */
struct StorageWflwRjctTag {};

}  // namespace ase::storage
