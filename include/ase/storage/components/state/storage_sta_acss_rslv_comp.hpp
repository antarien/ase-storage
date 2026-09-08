#pragma once

/**
 * ASE ECS COMPONENT
 *
 * @file        storage_sta_acss_rslv_comp.hpp
 * @brief       StorageStaAcssRslvComponent - What the ACL resolution found for one request
 * @description Das Ergebnis der AUFLOESUNG, auf der Anfrage-Entity: welche Schutzstufe der Ort
 *              verlangt, welche ACL-Regel ihn regiert, und welches Arbeitsstatus-Etikett diese
 *              Regel traegt. Mehr braucht die Torleiter nicht - alles Uebrige leitet sie aus
 *              Dingen ab, die ohnehin auf der Anfrage stehen (Keycard, Pfad, Revier-Verweis).
 *
 *              WARUM NUR DREI FELDER. Die wirksame Freigabestufe und die wirksamen Rechte sind
 *              KEINE Felder hier: sie sind `owner_preset ? Vorgabe : Keycard`, und beide Haelften
 *              stehen dem Leser bereits zur Verfuegung - die Marke StorageAcssOwnrTag und die
 *              Keycard-Komponente. Sie mitzufuehren hiesse, denselben Wert an zwei Orten wahr zu
 *              halten; ein zweiter Ort ist ein zweiter Ort, an dem er falsch sein kann.
 *
 *              WARUM DIE AUFLOESUNG UEBERHAUPT GETRENNT IST (2026-08-29): "welches Revier und
 *              welche Regel gelten hier" ist eine Nachschlagefrage - Pfad-Vorfahren, Revier-Index,
 *              Regelabgleich. "Darf dieser Anrufer" ist eine Entscheidungsfrage - Gitter,
 *              Freigabestufe, Codewort, Recht, Etikett, Kenntnisnahme, Kontingent. Beide standen
 *              in einer Funktion, und jede Aenderung an der einen war eine Aenderung an der
 *              Datei der anderen.
 *
 * storage = module, sta = state, acss = access, rslv = resolve
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @parity      server_only
 *
 * PARITAET GEMESSEN AM 2026-08-30. ACHTUNG: die uebliche Bestandsableitung traegt hier NICHT —
 * ase-storage fuehrt 44 shared Components und client_relevance true, das Modul HAT einen
 * Client-Kanal. Diese Component haelt, was die ACL-Aufloesung gefunden hat; sie ist die
 * Ableitung selbst, nicht deren Ergebnis. Getragen wird die Zeile von null Lesern ausserhalb
 * des Moduls, null Treffern im Web-Client bei 88 dortigen Storage*-Dateien (starke
 * Positivkontrolle), und der Regel, dass eine Autorisierungsentscheidung den Browser nicht
 * erreicht. Volle Begruendung in storage_acss_ownr_tag.hpp.
 * @created     2026-08-29
 * @modified    2026-08-29
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

#include <ase/storage/types.hpp>

namespace ase::storage {

/**
 * StorageStaAcssRslvComponent - the resolved location facts of one pending access request
 */
struct StorageStaAcssRslvComponent {
    uint32_t matched_rule = INVALID_ENTITY;             // ACL rule entity governing this path
    uint32_t rule_label_hash = 0;                       // workflow label of that rule (0 = none)
    uint8_t  required_protection = PROTECTION_PUBLIC;   // clearance the location demands
};

}  // namespace ase::storage
