#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        storage_rvw_rjct_tag.hpp
 * @brief       StorageRvwRjctTag - Der Pruefer hat das Bild abgelehnt
 * @description Marke auf einem Pruefeintrag: das Bild ist fuer seinen Zweck nicht geeignet.
 *              Anders als bei der Nacharbeitsmarke ist damit nichts zu tun - der Eintrag ist
 *              erledigt.
 *
 *              DREI DINGE AM ALTEN NAMEN WAREN FALSCH. Die Datei hiess
 *              `storage_tag_cur_rejected.hpp`: `tag` in der Mitte, `rejected`
 *              ausgeschrieben, und der Stamm `cur` ungedeckt (der Katalog fuehrt weder
 *              "curation" noch "curate" noch "curator"). Die Sache ist eine PRUEFUNG, und
 *              die heisst `rvw` (03_act_action.json, act/lrn/prc).
 *
 *              `rjct` UND SEIN GEGENSTUECK `acpt` STEHEN IM SELBEN KNOTEN, in benachbarten
 *              Zeilen: `09_com_communication.json`, `req/stat`, Zeilen 168 und 169. Der
 *              Knoten fuehrt die ganze Zustandsreihe einer Anfrage - pending, processing,
 *              completed, failed, timeout, cancelled, rejected, accepted, queued, deferred,
 *              retrying, aborted -, und ein Pruefurteil ist genau so ein Zustand.
 *
 * storage = module, rvw = review, rjct = rejected
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    tag/review
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
 * @brief StorageRvwRjctTag - Image rejected by the reviewer
 *
 * State: Not suitable for the intended purpose
 * Filter: Used by StorageCurPrcSystem when the status changes
 * Added: On CUR_ACT_STATUS with CUR_ST_REJECTED
 * Removed: When the entry is re-opened for review
 */
struct StorageRvwRjctTag {};

}  // namespace ase::storage
