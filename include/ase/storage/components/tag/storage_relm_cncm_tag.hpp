#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        storage_relm_cncm_tag.hpp
 * @brief       StorageRelmCncmTag - Der Bereich ist fuer Nichtmitglieder unsichtbar
 * @description Marke auf einer Bereichs-Entity: seine blosse EXISTENZ wird Nichtmitgliedern
 *              nicht gezeigt. Das ist mehr als fehlender Zugriff - eine Auflistung darf ihn
 *              nicht einmal erwaehnen.
 *
 *              DER DATEINAME TRUG DAS TYPWORT IN DER MITTE (`storage_tag_relm_conceal.hpp`),
 *              und `conceal` war ausgeschrieben.
 *
 *              DAS KUERZEL IST `cncm`, NICHT `cncl` - UND DAS IST EINE ENTSCHEIDUNG GEGEN
 *              DEN NAECHSTLIEGENDEN KATALOGEINTRAG. Der Katalog fuehrt "conceal" tatsaechlich
 *              als `cncl` (06_str_structure.json), aber dasselbe Kuerzel gehoert dort auch
 *              "cancel" (03_act_action.json, act/ctrl/flow) und "cancelled"
 *              (09_com_communication.json, req/stat). Im Baum steht `cncl` heute
 *              ausschliesslich fuer Abbruch - `replica_llm_cncl_sys`, `rsn_llm_cncl_emit_sys`
 *              und zwei weitere -, und diese Dateien sind KATALOGKONFORM, nicht
 *              uebergetreten.
 *
 *              Ein `StorageRelmCnclTag` waere damit ebenfalls konform gewesen und trotzdem
 *              unlesbar: dasselbe Kuerzel, zwei Bedeutungen, beide erlaubt. `concealment`
 *              loest EINDEUTIG auf `cncm` auf (14_cmb_combat.json, cmb/tac/str - Verbergen
 *              als taktische Sache) und kollidiert mit nichts.
 *
 * storage = module, relm = realm, cncm = concealment
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
 * @brief StorageRelmCncmTag - Realm existence hidden from non-members
 *
 * State: Listings must not mention the realm to non-members
 * Filter: registry.view<StorageStaRelmComponent, StorageRelmCncmTag>()
 * Added: Owner conceals the realm
 * Removed: Owner reveals the realm
 */
struct StorageRelmCncmTag {};

}  // namespace ase::storage
