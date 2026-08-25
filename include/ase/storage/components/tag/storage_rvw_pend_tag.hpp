#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        storage_rvw_pend_tag.hpp
 * @brief       StorageRvwPendTag - Der Eintrag wartet auf die Pruefung
 * @description Marke auf einem Pruefeintrag, solange kein Urteil vorliegt. Sie wird beim
 *              Anlegen gesetzt und faellt, sobald der Pruefer entscheidet.
 *
 *              DREI DINGE AM ALTEN NAMEN WAREN FALSCH. Die Datei hiess
 *              `storage_tag_cur_unrated.hpp`: `tag` in der Mitte, `unrated` ausgeschrieben,
 *              und der Stamm `cur` ungedeckt. Fuer "unrated" fuehrt der Katalog KEINEN
 *              Eintrag; "rating" gibt es (`rtng`, 13_skl), aber die Sache ist keine
 *              Bewertung, sondern eine WARTELAGE: "awaiting curator review".
 *
 *              SIE IST EINE ECHTE MARKE UND KEINE ABWESENHEIT - und das ist gemessen, nicht
 *              angenommen. Die naheliegende Lesart waere: wer keine der drei Urteilsmarken
 *              traegt, wartet noch, und ein Tag fuer "nichts" muesste mit seiner eigenen
 *              Abwesenheit synchron gehalten werden. `storage_cur_prc_sys.cpp` zeigt das
 *              Gegenteil - `remove` in Zeile 182, `emplace` in 201 und 251: ein gepflegter
 *              Lebenszyklus mit Anfang und Ende.
 *
 *              `pend` ist im Modul die Hausform fuer genau diese Lage: `audt_pend`,
 *              `kycd_pend`, `wflw_pend`, `vote_pend`. Der Katalog fuehrt "pending" dreifach
 *              (02_sta, 09_com, 10_pst), im selben `req/stat`-Knoten wie die beiden
 *              Urteilsmarken.
 *
 * storage = module, rvw = review, pend = pending
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
 * @brief StorageRvwPendTag - Entry has not been reviewed yet
 *
 * State: Default for new review entries
 * Filter: Used by StorageCurPrcSystem to find unreviewed images
 * Added: When the review entity is first created
 * Removed: On the first verdict (accepted, rejected or revise)
 */
struct StorageRvwPendTag {};

}  // namespace ase::storage
