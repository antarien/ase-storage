#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_inp_kycd_ntfy_grnt_comp.hpp
 * @brief       StorageInpKycdNtfyGrntComponent - was die angefragte Keycard oeffnet, aus dem Stern
 * @description Die Bedingungshaelfte der Bruecke einer Ausstellungsanfrage: Freigabestufe, Rechte
 *              und Ablauf, gespiegelt von StorageKycdNtfySynSystem aus der Hub-Zeile
 *              HUB_STG_KYCD_REQ. Sie liegt auf derselben ANFRAGE-Entity wie die Identitaetshaelfte.
 *
 *              WARUM DIE FUGE HIER LIEGT UND WARUM SIE `grnt` HEISST. Das Modul trennt dieselbe
 *              Sache schon einmal so: StorageStaKycdComponent traegt, WEM die Karte gilt,
 *              StorageKycdGrntComponent, WAS sie oeffnet und BIS WANN. Die Bruecke folgt dieser
 *              Fuge und ihrem Wort, statt eine eigene zu erfinden.
 *
 *              DER ABLAUF STEHT ALS u64. Die Zeile fuehrt ihn bitgenau; Unix-Sekunden passen nicht
 *              in die 24-Bit-Mantisse eines float.
 *
 *              DIE ABWESENHEIT VON FREIGABESTUFE UND RECHTEN IST KEIN FEHLER. Die Ausstellung des
 *              Auth-Gates traegt keine Rechte; fehlt ein Feld, bleibt es auf null.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @parity      server_only
 * @usecase     usecase:100264 - issue customer keycard
 * @created     2026-09-21
 * @modified    2026-10-12
 * @version     00.00.00.00001 [seed]
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
 * @brief StorageInpKycdNtfyGrntComponent - the star side of WHAT a requested keycard opens
 *
 * Written by StorageKycdNtfySynSystem, read by StorageKycdNtfyDrnSystem. Every field is a
 * MIRROR of a field of the hub row on this request entity; nothing here is computed.
 *
 * @hub_reads  HUB_STG_KYCD_REQ_CLEARANCE, _PERM, _EXP_AT (by the sync system)
 * @hub_writes none
 */
struct StorageInpKycdNtfyGrntComponent {
    uint32_t clearance = 0;                   // requested clearance level, 0 when the request names none
    uint32_t perm = 0;                        // requested permission bits, 0 when the request names none
    uint64_t expires_at = 0;                  // requested expiry, seconds since epoch, exact
};

}  // namespace ase::storage
