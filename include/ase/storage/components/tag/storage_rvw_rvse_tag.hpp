#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        storage_rvw_rvse_tag.hpp
 * @brief       StorageRvwRvseTag - Der Pruefer verlangt Nacharbeit
 * @description Marke auf einem Pruefeintrag: das Bild braucht Aenderungen, bevor es
 *              angenommen werden kann. Der Unterschied zur Ablehnung ist, dass hier etwas zu
 *              tun bleibt - der Eintrag kommt zurueck.
 *
 *              DREI DINGE AM ALTEN NAMEN WAREN FALSCH. Die Datei hiess
 *              `storage_tag_cur_rework.hpp`: `tag` in der Mitte, `rework` ausgeschrieben,
 *              und der Stamm `cur` ungedeckt. Fuer "rework" selbst fuehrt der Katalog
 *              KEINEN Eintrag - die Sache ist "revise".
 *
 *              `revise` TRAEGT ZWEI KUERZEL, UND DER KNOTEN ENTSCHEIDET, NICHT DAS WORT:
 *
 *                  "revise": "rvse"   03_act_action.json:715   act/lrn/prc
 *                  "revise": "revs"   16_bdi_bdi.json:67       BDI-Emotionsast
 *
 *              Beide sind gueltig, beide kaemen am flachen Namenstor durch - ein
 *              `StorageRvwRevsTag` waere ebenso konform gewesen wie dieser hier. Gewaehlt
 *              ist `rvse`, WEIL ES IN DER ZEILE DIREKT UNTER `"review": "rvw"` STEHT
 *              (714/715). Stamm und Nacharbeitsmarke kommen damit aus demselben Ast; `revs`
 *              haette dieselbe Sache aus einer fremden Domaene benannt.
 *
 *              Das ist die Umkehrung des `cncl`-Falls im selben Umbau: dort trug EIN Kuerzel
 *              ZWEI Woerter, hier traegt EIN Wort ZWEI Kuerzel. Kein Tor sieht eine der
 *              beiden Lagen.
 *
 * storage = module, rvw = review, rvse = revise
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
 * @brief StorageRvwRvseTag - Image needs changes before it can be accepted
 *
 * State: Reviewed, but modifications are required
 * Filter: Used by StorageCurPrcSystem when the status changes
 * Added: On CUR_ACT_STATUS with CUR_ST_NEEDS_REWORK
 * Removed: When the entry is re-opened for review
 */
struct StorageRvwRvseTag {};

}  // namespace ase::storage
