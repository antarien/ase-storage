#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        storage_relm_visb_tag.hpp
 * @brief       StorageRelmVisbTag - Realm is discoverable without a viewer
 * @description Derived classification: the realm may appear in an unauthenticated
 *              listing. Set and cleared by StorageCncmFltSystem, never by hand.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    tag
 * @parity      shared
 *
 * PARITAETS-ENTSCHEIDUNG (WS-0.4, ausdruecklich getroffen)
 *   Jede Eingangsgroesse dieser Ableitung ist bereits shared - StorageStaRelm,
 *   RelmPublic, RelmActive, RelmConceal, RelmSuspended, RelmArchived. Der Client
 *   haelt dieselben Realm-Entities und muss dieselbe Liste zeigen; rechnete er
 *   sie nicht selbst, muesste die Regel ein zweites Mal in TypeScript stehen und
 *   koennte auseinanderlaufen. Genau das verhindert die Paritaet.
 *
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
 * @brief StorageRelmVisbTag - realm may be listed to someone who named no keycard
 *
 * State:   Public AND active AND not concealed, suspended or archived.
 * Filter:  A listing route or client view walks realms carrying THIS tag instead
 *          of re-deriving the rule; the derivation lives in one system.
 * Added:   StorageCncmFltSystem when all conditions hold.
 * Removed: StorageCncmFltSystem the moment one of them stops holding.
 *
 * WHAT THIS TAG IS NOT: it is not "visible to user X". Discoverability is
 * viewer-INDEPENDENT and can therefore live on the realm entity. Whether a
 * concealed realm is visible to one particular requester depends on that
 * requester's ownership and keycard, is answered inside StorageAcssChkSystem
 * where the requester is known, and must never be folded in here - a per-viewer
 * answer cached on a shared entity would show one user another user's realms.
 */
struct StorageRelmVisbTag {};

}  // namespace ase::storage
