#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_cred_acss_pnd_comp.hpp
 * @brief       StorageCredAcssPndComponent - Wire correlation for a credential A/ACS access check
 * @description Emplaced alongside StorageReqAcssComponent by StorageCredAcssRcvSystem when a
 *              CACSS_WIRE_REQ frame arrives Replica→Engine. Carries the wire req_id + the Replica
 *              connection id so StorageCredAcssRspSystem can ship the CACSS_WIRE_RES verdict back to
 *              the originating connection after StorageAcssChkSystem renders Grant/DenyTag.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @parity      server_only
 *
 * PARITAETS-ENTSCHEIDUNG (WS-0.4, ausdruecklich getroffen)
 *   Beide Felder sind Adressen der Server-zu-Server-Naht, keine Spielwerte: req_id ist
 *   die Korrelation eines CACSS_WIRE_REQ/RES-Rahmens, conn_id die Replica-WS-Verbindung
 *   im Engine. StorageCredAcssRspSystem baut daraus den 11-Byte-Rahmen fuer die
 *   OutboundQueue und zerstoert die Entity danach - der Empfaenger ist die Replica, nie
 *   ein Browser. Eine Verbindungs-Id ist drueben nicht nur nutzlos, sondern benennt
 *   fremde Sitzungen; im becsy-Weltbild existiert diese Naht gar nicht. Das Urteil selbst
 *   erreicht seinen Adressaten ueber den Wire-Rahmen, nicht ueber eine Kanalliste.
 * @created     2026-07-04
 * @modified    2026-07-04
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
 * @brief StorageCredAcssPndComponent - correlates an in-flight credential A/ACS check to its wire response.
 *
 * On the same entity as StorageReqAcssComponent. StorageCredAcssRcvSystem sets it from the inbound
 * CACSS_WIRE_REQ; StorageCredAcssRspSystem reads it (with the ladder's Grant/DenyTag) to emit
 * CACSS_WIRE_RES back to conn_id, then destroys the entity. req_id/conn_id are wire values, never pointers.
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageCredAcssPndComponent {
    uint64_t req_id = 0;                       // Wire correlation id echoed back in CACSS_WIRE_RES
    uint32_t conn_id = 0;                      // Replica WS connection the verdict is shipped to
};

}  // namespace ase::storage
