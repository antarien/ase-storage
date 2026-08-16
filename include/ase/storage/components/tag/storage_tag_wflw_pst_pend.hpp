#pragma once

/**
 * =============================================================================
 * ASE ECS COMPONENT (TAG)
 * =============================================================================
 *
 * @file        storage_tag_wflw_pst_pend.hpp
 * @brief       StorageWflwPstPendTag - applied transition not yet durably persisted
 * @description Marker for StorageBufWflwComponent entities awaiting the frame-112
 *              REPLACE-upsert to the Replica workflow-label store
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    tag
 * @parity      server_only
 *
 * PARITAETS-ENTSCHEIDUNG (WS-0.4, ausdruecklich getroffen)
 *   Die Marke haengt am Persist-Puffer einer angewandten Label-Transition und
 *   sagt, dass der Frame-112-Upsert in den Replica-Speicher
 *   storage_workflow_labels noch aussteht. Gesetzt von StorageWflwTranSystem beim
 *   Anwenden (storage_wflw_tran_sys.cpp, Zeile 433), gelesen allein von
 *   StorageWflwPstSystem in der Schedule Preservation (storage_module.hpp,
 *   Zeile 205), das den Puffer in die Ausgangsschlange leert und das Entity
 *   danach zerstoert (storage_wflw_pst_sys.cpp, Zeile 224); ausserhalb dieses
 *   Persist-Pfades trifft die Suche nach dem Tag-Namen nur Kommentare und einen
 *   Test. Entscheidend ist der Traeger: StorageBufWflwComponent steht in
 *   codegen.json unter components.server_only. Das Entity, an dem die Marke
 *   haengt, existiert clientseitig also gar nicht — ein Zwilling haette nichts,
 *   woran er haengen koennte, und der Browser besitzt weder die Ausgangsschlange
 *   noch die MongoDB dahinter.
 * @created     2026-07-11
 * @modified    2026-07-11
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
 * @brief StorageWflwPstPendTag - applied transition not yet durably persisted
 *
 * State: Label transition applied locally, frame-112 upsert not yet sent
 * Filter: Used by StorageWflwPstSystem (Preservation) to drain persist buffers
 * Added: StorageWflwTranSystem together with StorageBufWflwComponent on apply
 * Removed: Entity destroyed by StorageWflwPstSystem after the frame is pushed
 */
struct StorageWflwPstPendTag {};

}  // namespace ase::storage
