#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_inp_kycd_ntfy_idn_comp.hpp
 * @brief       StorageInpKycdNtfyIdnComponent - wem die angefragte Keycard gilt, aus dem Stern
 * @description Die Identitaetshaelfte der Bruecke einer Ausstellungsanfrage: StorageKycdNtfSynSystem
 *              spiegelt die vier Sternzeilen HIERHER, und der Drain liest nur noch diese Zeile.
 *              Sie liegt auf der ANFRAGE-Entity, ihr Besitzer ist also die Entity selbst - ein
 *              eigenes Besitzerfeld waere ein zweiter Ort fuer dieselbe Auskunft.
 *
 *              WARUM DIE HAELFTEN ROH STEHEN BLEIBEN. Der Stern fuehrt jede Groesse als float,
 *              und ein uint32-Hash passt nicht in eine 24-Bit-Mantisse; die SDK-Seite zerlegt ihn
 *              deshalb in zwei exakte 16-Bit-Haelften. Diese Zeile SPIEGELT, sie rechnet nicht -
 *              das Zusammensetzen ist Sache des lesenden Systems. Wer hier zusammensetzte, holte
 *              die Rechnung zurueck an die Hub-Seite und hoebe den Schnitt auf, fuer den es die
 *              Zeile gibt.
 *
 *              WARUM ES DEN SCHNITT GIBT. Gemessen 2026-09-21 meldete der Validator an
 *              storage_kycd_ntfy_drn_sys.cpp HUB_IO_MIXED_WITH_MATH: sechs hub::get und die
 *              Rekonstruktion standen in EINEM tick(). Die Regel nennt ihren Weg selbst - eine
 *              server-only Seite liest den Stern in eine *_inp_*-Zeile, die rechnende Seite nimmt
 *              sie von dort (WRFL_ASE_SYN_PATTERN).
 *
 *              WARUM ZWEI ZEILEN UND NICHT EINE. Sechs Felder in einem Component sind eine
 *              God-Component; das Modul trennt dieselbe Sache schon einmal so -
 *              StorageStaKycdComponent traegt, WEM die Karte gilt, StorageKycdGrntComponent, WAS
 *              sie oeffnet. Die Bruecke folgt derselben Fuge: hier die Identitaet, in
 *              storage_inp_kycd_ntfy_trm_comp.hpp die Bedingungen.
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @parity      server_only
 * @created     2026-09-21
 * @modified    2026-09-21
 * @version     00.00.00.00000 [seed]
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
 * @brief StorageInpKycdNtfyIdnComponent - the star side of WHO a requested keycard is for
 *
 * Written by StorageKycdNtfSynSystem, read by StorageKycdNtfyDrnSystem. Every field is a
 * MIRROR of a hub line under this request entity; nothing here is computed.
 *
 * @hub_reads  SES_KYCD_NTF_USER_ID_HI/_LO, SES_KYCD_NTF_REALM_ID_HI/_LO (by the sync system)
 * @hub_writes none
 */
struct StorageInpKycdNtfyIdnComponent {
    float user_hash_hi = 0.0f;                // upper 16 bits of the FNV user hash, exact
    float user_hash_lo = 0.0f;                // lower 16 bits of the FNV user hash, exact
    float realm_hash_hi = 0.0f;               // upper 16 bits of the FNV realm hash, exact
    float realm_hash_lo = 0.0f;               // lower 16 bits of the FNV realm hash, exact
};

}  // namespace ase::storage
