#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        storage_rvw_acpt_tag.hpp
 * @brief       StorageRvwAcptTag - Der Pruefer hat das Bild angenommen
 * @description Marke auf einem Pruefeintrag: das Bild darf im Spiel verwendet werden.
 *              `StorageRvwPrcSystem` setzt sie, wenn der Pruefer den Status auf angenommen
 *              stellt, und nimmt dafuer die Wartemarke weg.
 *
 *              DREI DINGE AM ALTEN NAMEN WAREN FALSCH. Die Datei hiess
 *              `storage_tag_cur_approved.hpp`: das Typwort `tag` stand in der MITTE, wo nach
 *              INST_ASE_ECS_SER Taxonomie steht, und `approved` war ausgeschrieben.
 *
 *              DRITTENS - UND DAS IST DER TEIL, DEN EIN TOR NICHT SIEHT - WAR AUCH DER
 *              STAMM `cur` UNGEDECKT. Der Katalog fuehrt weder "curation" noch "curate" noch
 *              "curator"; `cur` kam trotzdem durch, weil das Namenstor ein FLACHES Set ueber
 *              alle Domaenen baut und Existenz prueft, nicht Zustaendigkeit fuer DIESES
 *              Wort. Die Sache ist eine PRUEFUNG - "Curator has approved this image" -, und
 *              die fuehrt der Katalog als "review" (`rvw`, 03_act_action.json, act/lrn/prc).
 *
 *              `acpt` KOMMT AUS DEMSELBEN KNOTEN WIE SEIN GEGENSTUECK: "accepted" und
 *              "rejected" stehen in `09_com_communication.json` unter `req/stat` in
 *              benachbarten Zeilen (168 und 169). Zwei Urteilsmarken aus einem Knoten -
 *              das belegt, dass beide dieselbe Frage beantworten.
 *
 *              Fuer "approved" selbst fuehrt der Katalog KEINEN Eintrag. Er war auch keiner
 *              noetig: ein Pruefurteil IST eine Annahme.
 *
 * storage = module, rvw = review, acpt = accepted
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
 * @brief StorageRvwAcptTag - Image accepted for game use
 *
 * State: Reviewer looked at the image and accepted it
 * Filter: Used by StorageCurPrcSystem when the status changes
 * Added: On CUR_ACT_STATUS with CUR_ST_APPROVED
 * Removed: When the entry is re-opened for review
 */
struct StorageRvwAcptTag {};

}  // namespace ase::storage
