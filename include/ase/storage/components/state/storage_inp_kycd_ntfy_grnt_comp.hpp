#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_inp_kycd_ntfy_grnt_comp.hpp
 * @brief       StorageInpKycdNtfyGrntComponent - was die angefragte Keycard oeffnet, aus dem Stern
 * @description Die Bedingungshaelfte der Bruecke einer Ausstellungsanfrage: Freigabestufe und
 *              Ablauf, gespiegelt von StorageKycdNtfSynSystem. Sie liegt auf derselben
 *              ANFRAGE-Entity wie die Identitaetshaelfte.
 *
 *              WARUM DIE FUGE HIER LIEGT UND WARUM SIE `grnt` HEISST. Das Modul trennt dieselbe
 *              Sache schon einmal so: StorageStaKycdComponent traegt, WEM die Karte gilt,
 *              StorageKycdGrntComponent, WAS sie oeffnet und BIS WANN. Die Bruecke folgt dieser
 *              Fuge und ihrem Wort, statt eine eigene zu erfinden - ein `trm` fuer "terms" wurde
 *              vom Tor zu Recht abgewiesen: es steht in keinem Katalog, und ein erfundenes
 *              Kuerzel ist schlechter als gar keins.
 *
 *              WARUM ES DEN SCHNITT UEBERHAUPT GIBT. Gemessen 2026-09-21 meldete der Validator an
 *              storage_kycd_ntfy_drn_sys.cpp HUB_IO_MIXED_WITH_MATH: sechs hub::get und die
 *              Rekonstruktion der Hashes standen in EINEM tick(). Eine server-only Seite liest
 *              den Stern in eine *_inp_*-Zeile, die rechnende Seite nimmt sie von dort
 *              (WRFL_ASE_SYN_PATTERN).
 *
 *              DIE ABWESENHEIT DER FREIGABESTUFE IST KEIN FEHLER. Fehlt SES_KYCD_NTF_CLRN im
 *              Stern, bleibt das Feld auf null - so stand es vor dem Schnitt im Drain und so
 *              steht es jetzt im Spiegel. Nur die vier Pflichtzeilen fuehren bei Abwesenheit zu
 *              einer Meldung und zum Ueberspringen.
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
 * @brief StorageInpKycdNtfyGrntComponent - the star side of WHAT a requested keycard opens
 *
 * Written by StorageKycdNtfSynSystem, read by StorageKycdNtfyDrnSystem. Both fields are a
 * MIRROR of a hub line under this request entity; nothing here is computed.
 *
 * @hub_reads  SES_KYCD_NTF_CLRN, SES_KYCD_NTF_EXP_AT (by the sync system)
 * @hub_writes none
 */
struct StorageInpKycdNtfyGrntComponent {
    float clearance = 0.0f;                   // requested clearance level, 0 when the star is silent
    float expires_at = 0.0f;                  // requested expiry, seconds since epoch
};

}  // namespace ase::storage
