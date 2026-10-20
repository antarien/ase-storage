#pragma once

/**
 * ASE ECS COMPONENT (REQUEST)
 *
 * @file        storage_req_wflw_tran_comp.hpp
 * @brief       StorageReqWflwTranComponent - Workflow label-transition request
 * @description One entity per requested promote (Entity-per-Item). Filled by
 *              StorageEdgeWflwFwdRcvSystem (frame 113) or by StorageWflwDrnSystem
 *              (hub row HUB_STG_WFLW_REQ) and staged by StorageWflwStgeSystem;
 *              validated data-driven against the seeded StorageWflwEdgeComponent
 *              entities by StorageWflwTranSystem, which writes the new label into
 *              the matching StorageAcssRuleComponent + the audit attribution.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    communication/request
 * @parity      server_only
 * @usecase     usecase:100329 - stage workflow promotion
 *
 * PARITAETS-ENTSCHEIDUNG (WS-0.4, ausdruecklich getroffen)
 *   Der Antrag auf eine Label-Transition, nicht das Label. StorageEdgeWflwFwdRcvSystem oder
 *   StorageWflwDrnSystem erzeugt die Zeile, StorageWflwStgeSystem stellt sie ein;
 *   StorageWflwGateSystem und StorageWflwTranSystem lesen
 *   sie, faellen das Urteil und zerstoeren die Entity (deferred delete) - kein Verbraucher
 *   reicht path/target_label/requested_by weiter. Das ERGEBNIS verlaesst diesen Rumpf auf
 *   zwei eigenen Wegen: der neue Zustand als label auf StorageAcssRuleComponent, das
 *   Urteil als Hub-Wert STG_WFLW_RES unter dem Pfad-Owner, dazu der Audit-Eintrag. Wer
 *   drueben wissen will, wie eine Promotion ausging, liest diese, nie den Antrag.
 *   Zusaetzlich traegt die Zeile mit requested_by eine Operator-Identitaet, die auf der
 *   Serverseite zur Attribution gebraucht wird und im Browser nichts zu suchen hat.
 * @created     2026-07-11
 * @modified    2026-10-16
 * @version     1.0.2
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

// MAX_PATH_LEN, MAX_LABEL_LEN, MAX_OWNER_ID - the module's slot widths, NUL included
#include <ase/storage/types.hpp>

namespace ase::storage {

/**
 * @brief StorageReqWflwTranComponent - one workflow label-transition request
 *
 * One entity per promote request (Entity-per-Item). Carries the realm-relative
 * asset path (the ACL-rule pattern key), the requested target label and the
 * requester identity for keycard attribution. Lifecycle tags on the same entity:
 * StorageWflwPendTag (awaiting transition), StorageWflwGateTag (released-gate
 * artifact check outstanding). Destroyed by StorageWflwTranSystem after the
 * verdict is published (deferred delete).
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageReqWflwTranComponent {
    char path[MAX_PATH_LEN] = {};             // Realm-relative asset path (matches StorageAcssRuleComponent.path_pattern)
    char target_label[MAX_LABEL_LEN] = {};    // Requested workflow label ("review", "released", ...)
    char requested_by[MAX_OWNER_ID] = {};     // Operator user_id string (keycard attribution for the audit entry)
};

}  // namespace ase::storage
