#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        storage_relm_glob_tag.hpp
 * @brief       StorageRelmGlobTag - Der plattformweite Bereich mit den Engine-Vorgaben
 * @description Marke auf GENAU EINER Bereichs-Entity: dem Bereich, der die geteilten
 *              Vorlagen, Shader und Engine-Bestaende traegt. Er entsteht beim Hochfahren und
 *              vergeht nie.
 *
 *              DER DATEINAME TRUG DAS TYPWORT IN DER MITTE (`storage_tag_relm_public.hpp`),
 *              und `public` war ausgeschrieben.
 *
 *              DAS WORT IST NICHT "public", SONDERN "global". Fuer "public" fuehrt der
 *              Katalog keinen Eintrag - `publ` gehoert dort "publication"
 *              (03_act_action.json), also einem anderen Wort. Ein `StorageRelmPublTag` waere
 *              am flachen Namenstor gruen durchgegangen und haette eine fremde Sache
 *              benannt.
 *
 *              Die Sache ist ein GELTUNGSBEREICH, und den fuehrt `34_cfg_config.json` unter
 *              `set/scope`: *global, user, session, local, default*. Der plattformweite
 *              Bereich ist `global` (`glob`), das Gegenstueck (`StorageRelmUsrTag`) kommt
 *              aus DEMSELBEN Knoten. Zwei Namen aus einem Knoten belegen, dass beide Marken
 *              dieselbe Frage beantworten - fuer wen gilt dieser Bereich.
 *
 * storage = module, relm = realm, glob = global (Geltungsbereich)
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
 * @brief StorageRelmGlobTag - Public platform realm (engine defaults)
 *
 * State: Single realm with shared templates, shaders, and engine assets
 * Filter: registry.view<StorageStaRelmComponent, StorageRelmGlobTag>()
 * Added: StgIniSystem during bootstrap (exactly one)
 * Removed: Never (the platform realm is permanent)
 */
struct StorageRelmGlobTag {};

}  // namespace ase::storage
