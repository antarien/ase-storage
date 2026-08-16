#pragma once

/**
 * =============================================================================
 * ASE ECS COMPONENT (TAG)
 * =============================================================================
 *
 * @file        storage_relm_edge_tag.hpp
 * @brief       StorageRelmEdgeTag - Realm distributes edge-daemon binaries
 * @description Identity tag for the single realm whose id is EDGE_REALM_ID
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    tag
 * @parity      shared
 *
 * PARITAETS-ENTSCHEIDUNG (WS-0.4, ausdruecklich getroffen)
 *   Ein Realm wird auf beiden Seiten gleich klassifiziert. Saemtliche uebrigen
 *   Realm-Klassifikations-Tags stehen in codegen.json unter components.shared -
 *   StorageRelmPublicTag, StorageRelmPersonalTag, StorageRelmOrgTag,
 *   StorageRelmActiveTag, StorageRelmSuspendedTag, StorageRelmArchivedTag,
 *   StorageRelmConcealTag - und die Realm-Entity, an der sie haengen
 *   (StorageStaRelmComponent, StorageRelmIdnComponent), ebenfalls. Dieses Tag
 *   beantwortet dieselbe Art Frage wie jene: WELCHER Realm ist das. Faende der
 *   Client die sieben anderen Antworten vor und diese eine nicht, koennte er den
 *   Edge-Realm nicht von einem beliebigen oeffentlichen unterscheiden und muesste
 *   die Realm-Id wieder als String vergleichen - genau die Suche, die dieses Tag
 *   auf der Serverseite abgeloest hat.
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
 * @brief StorageRelmEdgeTag - Realm holds the edge-daemon binary distribution
 *
 * State: Identity, not lifecycle - orthogonal to Active/Suspended/Archived
 * Filter: registry.view<StorageStaRelmComponent, StorageRelmEdgeTag>()
 * Added: StorageEdgeIniSystem::on_start, on the realm it creates
 * Removed: Never - the realm identity does not change while the entity lives
 *
 * The workflow systems need the entity id of the edge realm for their audit
 * records. Before this tag they searched every realm and compared relm.id
 * against EDGE_REALM_ID with a string compare - inside their per-request loop,
 * so the cost was realms x requests every tick (WS-K.2c). The realm carries
 * exactly one producer (StorageEdgeIniSystem), which makes the tag the SSOT for
 * the same question and turns the search into a one-element View.
 */
struct StorageRelmEdgeTag {};

}  // namespace ase::storage
