#pragma once

/**
 * =============================================================================
 * ASE ECS COMPONENT (TAG)
 * =============================================================================
 *
 * @file        storage_acss_rule_sufx_tag.hpp
 * @brief       StorageAcssRuleSufxTag - ACL rule matches by path suffix
 * @description Marks a rule whose pattern began with '*' ("*.sig"), so it governs
 *              assets by their trailing extension instead of by their location
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    tag
 * @parity      shared
 *
 * PARITAETS-ENTSCHEIDUNG (WS-0.4, ausdruecklich getroffen)
 *   Dieses Tag klassifiziert eine Regel, und die Regel selbst - StorageAcssRuleComponent -
 *   steht in codegen.json unter components.shared. Die Komponente traegt nur Realm- und
 *   Projektbezug; WIE eine Regel trifft, steht ausschliesslich im Tag. Ohne das Tag saehe
 *   der Client eine Regel, deren Wirkungsbereich er nicht bestimmen kann: er wuerde eine
 *   Endungsregel ("*.sig") als Ortsregel lesen und ihr einen Pfadpraefix zuschreiben, den
 *   sie nie hatte. Beide Seiten muessen dieselbe Trefferart kennen, sonst weichen die
 *   Zugriffsentscheidungen auseinander, ohne dass irgendwo ein Fehler auftaucht.
 * @created     2026-08-16
 * @modified    2026-08-16
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
 *
 * =============================================================================
 */

namespace ase::storage {

/**
 * @brief StorageAcssRuleSufxTag - The rule matches assets by trailing extension
 *
 * State: A pattern kind, fixed when the rule is seeded and never changing after
 * Filter: registry.view<StorageAcssRuleComponent, StorageAcssRuleSufxTag>()
 * Added: The system that seeds the rule, when the pattern begins with '*'
 * Removed: Never - a rule does not change the way it matches
 *
 * Location rules (a path followed by a wildcard) and extension rules ("*.sig") match by two
 * different computations, so they are two Tag-filtered Views and two loops - never
 * one loop that inspects a discriminator and branches. The suffix form exists
 * because companion artifacts sit BESIDE the binary they belong to, where no
 * location prefix can reach them.
 */
struct StorageAcssRuleSufxTag {};

}  // namespace ase::storage
