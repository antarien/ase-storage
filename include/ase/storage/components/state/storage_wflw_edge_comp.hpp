#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_wflw_edge_comp.hpp
 * @brief       StorageWflwEdgeComponent - one allowed workflow-label transition edge
 * @description The release-pipeline graph as DATA (Entity-per-Item): one entity per
 *              allowed edge (draft→review, review→approved, approved→released,
 *              released→retired), seeded by StorageEdgeIniSystem. This "edge" is a
 *              graph Kante of the label chain — NOT the Edge-Daemon domain prefix.
 *              StorageWflwTranSystem validates a requested transition purely by
 *              matching (from_label == rule.label && to_label == request.target)
 *              against these entities — no switch/if-chain dispatch anywhere.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @created     2026-07-11
 * @modified    2026-07-11
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

namespace ase::storage {

/**
 * @brief StorageWflwEdgeComponent - one allowed label-transition edge (graph Kante)
 *
 * One entity per allowed edge of the release pipeline. Seeded once by
 * StorageEdgeIniSystem from the EDGE_LABEL_* chain; adding a new allowed
 * transition = seeding one more entity, never touching system code.
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageWflwEdgeComponent {
    char from_label[32] = {};                 // Current label the rule must hold ("draft", ...)
    char to_label[32] = {};                   // Label this edge permits transitioning to
};

}  // namespace ase::storage
