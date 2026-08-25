#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        storage_acss_grnt_tag.hpp
 * @brief       StorageAcssGrntTag - Die Zugriffspruefung ist bestanden
 * @description Marke auf einer Zugriffsanfrage, deren Pruefung durch A/ACS positiv
 *              ausgegangen ist.
 *
 *              ZWEI DINGE AM ALTEN NAMEN WAREN FALSCH. Die Datei hiess
 *              `storage_tag_acss_grant.hpp`: das Typwort `tag` stand in der MITTE, wo nach
 *              INST_ASE_ECS_SER Taxonomie steht.
 *
 *              UND `grant` WAR DIE WORTFORM, NICHT DAS KUERZEL. Der Katalog fuehrt "grant"
 *              als `grnt` (03_act_action.json, act/cmm/expr). Das ist der umgekehrte Fall
 *              zu `vote`, das der Katalog UNGEKUERZT fuehrt: **welche Form gilt, sagt der
 *              Katalogeintrag, nicht die Laenge des Wortes.** Ein vierbuchstabiges Wort
 *              sieht schon aus wie ein Kuerzel und ist trotzdem keines - `acss` daneben ist
 *              eines (aus "access").
 *
 * storage = module, acss = access, grnt = grant
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    tag/access
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
 * @brief StorageAcssGrntTag - A/ACS access check passed
 *
 * State: The request may proceed
 * Filter: registry.view<StorageAcssReqComponent, StorageAcssGrntTag>()
 * Added: StorageAcssChkSystem when the rules allow the request
 * Removed: With the request entity
 */
struct StorageAcssGrntTag {};

}  // namespace ase::storage
