#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        storage_relm_usr_tag.hpp
 * @brief       StorageRelmUsrTag - Der Bereich gehoert genau einem Nutzer
 * @description Marke auf einer Bereichs-Entity: sie entstand bei der Registrierung eines
 *              Nutzers und ist sein persoenlicher Arbeitsbereich. Die Marke faellt nie -
 *              die Art eines Bereichs steht mit seiner Erzeugung fest.
 *
 *              DER DATEINAME TRUG DAS TYPWORT IN DER MITTE (`storage_tag_relm_personal.hpp`),
 *              und `personal` war ausgeschrieben.
 *
 *              DAS WORT IST NICHT "personal", SONDERN "user" - UND DAS IST DER GRUND, WARUM
 *              DER KATALOG KEINE LUECKE HAT. Fuer "personal" fuehrt er keinen Eintrag, und
 *              die Suche danach endet in einer scheinbaren Luecke. Die Sache ist aber ein
 *              GELTUNGSBEREICH, und den fuehrt `34_cfg_config.json` unter `set/scope`:
 *              *global, user, session, local, default*. Ein Bereich je registriertem Nutzer
 *              ist `user`, dort als `usr` gefuehrt - und das Gegenstueck
 *              (`StorageRelmGlobTag`) kommt aus DEMSELBEN Knoten.
 *
 *              Zwei Namen aus einem Knoten sind der staerkste Beleg, den eine Benennung
 *              haben kann: er zeigt, dass die beiden Marken dieselbe FRAGE beantworten -
 *              fuer wen gilt dieser Bereich - und nicht zufaellig nebeneinanderstehen.
 *
 * storage = module, relm = realm, usr = user (Geltungsbereich)
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
 * @brief StorageRelmUsrTag - Realm is a personal workspace
 *
 * State: Realm was auto-created when user registered
 * Filter: registry.view<StorageStaRelmComponent, StorageRelmUsrTag>()
 * Added: StgIniSystem or realm creation route
 * Removed: Never (realm type is immutable)
 */
struct StorageRelmUsrTag {};

}  // namespace ase::storage
