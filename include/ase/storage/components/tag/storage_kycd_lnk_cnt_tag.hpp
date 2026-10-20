#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        storage_kycd_lnk_cnt_tag.hpp
 * @brief       StorageKycdLnkCntTag - a keycard was linked to this session in this tick
 * @description Marker for the session entity StorageKycdLnkSystem just linked; counted and
 *              removed by StorageKycdLnkCntSystem in the same frame
 *
 * @usecase     usecase:100265 - check access clearance
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    tag
 * @parity      server_only
 *
 * PARITAETS-ENTSCHEIDUNG (WS-0.4, ausdruecklich getroffen)
 *   Zaehlmarke des Servers: StorageKycdLnkSystem setzt sie, StorageKycdLnkCntSystem nimmt sie
 *   im selben Takt wieder ab. Sie lebt nie ueber einen Takt und traegt nichts, was der Client
 *   mitrechnet; die Zahl erreicht ihn als STG_KYCD_LNK_COUNT ueber den Stern.
 * @created     2026-10-13
 * @modified    2026-10-13
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
 * @brief StorageKycdLnkCntTag - one link of this tick, waiting to be counted
 *
 * The link system publishes the session to the star and may not raise a counter itself: a
 * system with hub I/O does no arithmetic (HUB_IO_MIXED_WITH_MATH). It leaves this marker
 * instead, and the counting system turns the markers into StorageStaSessStatComponent.linked.
 *
 * Added:   StorageKycdLnkSystem, on the session entity it linked
 * Removed: StorageKycdLnkCntSystem, after counting
 */
struct StorageKycdLnkCntTag {};

}  // namespace ase::storage
