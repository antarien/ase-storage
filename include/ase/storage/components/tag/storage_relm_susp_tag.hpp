#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        storage_relm_susp_tag.hpp
 * @brief       StorageRelmSuspTag - Der Bereich ist von der Verwaltung stillgelegt
 * @description Marke auf einer Bereichs-Entity: weder Lesen noch Schreiben. Anders als beim
 *              archivierten Bereich ist der Zustand rueckholbar - die Marke faellt, wenn die
 *              Verwaltung ihn wieder freigibt.
 *
 *              ZWEI DINGE AM ALTEN NAMEN WAREN FALSCH. Die Datei hiess
 *              `storage_tag_relm_suspended.hpp`: das Typwort `tag` stand in der MITTE, wo
 *              nach INST_ASE_ECS_SER Taxonomie steht. Und `suspended` war ausgeschrieben;
 *              der Katalog fuehrt dafuer `susp` (02_sta_state.json, sta/lfc/actv).
 *
 * storage = module, relm = realm, susp = suspended
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    tag/realm
 * @parity      shared
 * @created     2026-04-04
 * @modified    2026-08-20
 * @version     2.0.0
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
 * @brief StorageRelmSuspTag - Realm disabled (no read or write)
 *
 * State: All access denied until an admin lifts the suspension
 * Filter: registry.view<StorageStaRelmComponent, StorageRelmSuspTag>()
 * Added: Admin suspends realm (replaces StorageRelmActvTag)
 * Removed: Admin restores realm
 */
struct StorageRelmSuspTag {};

}  // namespace ase::storage
