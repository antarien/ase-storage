#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_rule_idn_comp.hpp
 * @brief       StorageRuleIdnComponent - ACL rule identity as comparable hashes
 * @description The rule's path pattern and workflow label in the ONLY form the
 *              ladder and the transition system compare.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @parity      shared
 *
 * PARITAETS-ENTSCHEIDUNG (WS-0.4, ausdruecklich getroffen)
 *   StorageAcssRuleComponent - der Datensatz mit path_pattern und label - steht in
 *   modules/ase-storage/codegen.json unter components.shared. Die Regel erreicht den
 *   Client als Text; dieser Zwilling traegt den Hash genau dieser beiden Zeichenketten
 *   plus die Spezifitaet (match_hash, match_len), nach der der Client konkurrierende
 *   Regeln ordnet. Bliebe er server_only, saehe der Client die Regel, koennte sie aber
 *   weder identifizieren noch gegen eine zweite abwaegen - er zeigte eine Rechteliste,
 *   deren Rangfolge fehlt. Identitaet folgt ihrem Datensatz.
 * @created     2026-08-16
 * @modified    2026-08-16
 * @version     1.0.0
 *
 * ECS COMPONENT COMPLIANCE
 *
 * [ ] DATA fields ONLY - No methods
 * [ ] NO .cpp file - Header-only
 * [ ] ONLY zero-initialization (= 0, = 0.0f, = false, = {})
 * [ ] No magic numbers in defaults (use types.hpp constants)
 * [ ] Entity references initialized to = 0 (systems set values)
 * [ ] Single responsibility (one data category)
 * [ ] No God-Component (unrelated fields)
 * [ ] Large data in registry.ctx()? (component has only lookup ID!)
 * [ ] Tag structs end with Tag suffix - N/A (not a tag)
 * [ ] Filename: prefix/suffix NOT abbreviated, words between = 3-4 chars
 * [ ] Struct name derived from filename (snake_case to PascalCase)
 * [ ] 1 File = 1 Component
 * [ ] File in correct category subfolder
 * [ ] SHARED components listed in codegen.json components.shared
 * [ ] Pointer components in codegen.json components.server_only
 * [ ] Strings < 64 bytes use char[N] fixed arrays
 * [ ] Strings 64-256 bytes use appropriately sized char[N]
 * [ ] Strings > 256 bytes use registry.ctx() mit Lookup-ID?
 * [ ] NO Entity-per-Character (strings are single attributes, not N-Items!)
 * [ ] Lookup-only strings use uint32_t hash (entt::hashed_string)
 * [ ] NO std::shared_ptr in components (use Flyweight Pattern via ctx!)
 * [ ] NO void* in components (use Flyweight Pattern via ctx!)
 * [ ] NO uint64_t as pointer concept (use uint32_t ID + ResourceManager via ctx!)
 * [ ] External library objects (shared_ptr, handles) in ResourceManager via ctx()
 * [ ] Component stores ONLY primitive ID (uint32_t) referencing external resource
 */

#include <cstdint>

namespace ase::storage {

/**
 * @brief StorageRuleIdnComponent - ACL rule identity, comparable in one 32-bit test
 *
 * The label decides the workflow gate and the pattern decides which rule an exact
 * asset path adopts - both are lookups, so both are compared as hashes
 * (WRFL_ASE_STRING_HANDLING Section 3).
 *
 * pattern_hash carries the pattern VERBATIM, wildcard included: it answers "is this
 * the same rule", never "does this path match". Wildcard matching reads the literal
 * segment and extension hashes and lives in the systems, not here.
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageRuleIdnComponent {
    uint32_t pattern_hash = 0;                // entt::hashed_string of StorageAcssRuleComponent::path_pattern
    uint32_t label_hash = 0;                  // entt::hashed_string of StorageAcssRuleComponent::label
    uint32_t match_hash = 0;                  // entt::hashed_string of the pattern's literal part, wildcard stripped
    uint32_t match_len = 0;                   // Length of that literal part - the rule's specificity score
};

}  // namespace ase::storage
