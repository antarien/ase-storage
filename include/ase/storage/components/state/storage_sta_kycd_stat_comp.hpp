#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_sta_kycd_stat_comp.hpp
 * @brief       StorageStaKycdStatComponent - wie viele Keycards dieses Modul ausgestellt hat
 * @description Die Zaehlzeile des Moduls, gefuehrt auf der Verwalterentity. Sie ist die QUELLE
 *              der Hub-Zeile STG_KYCD_ISSUED_COUNT, nicht ihr Spiegel.
 *
 *              WARUM DIE ZAHL HIER LIEGT UND NICHT IM STERN. Bis 2026-09-21 hielt der
 *              Ausstellungsschritt den Zaehler ausschliesslich im Hub und zaehlte ihn dort je
 *              Karte hoch: lesen, eins addieren, zurueckschreiben - und das INNERHALB der
 *              Schleife ueber alle Anfragen eines Takts. Zwei Dinge daran waren falsch. Erstens
 *              mischt es Hub-I/O mit Rechnung in einem System (HUB_IO_MIXED_WITH_MATH). Zweitens
 *              ist ein Hub-Wert EIN stehender Platz: zehn Karten in einem Takt hiessen zehn
 *              Schreibvorgaenge auf denselben Slot, von denen nur der letzte etwas bedeutet.
 *
 *              Der Zaehler gehoert dem Modul, der Stern bekommt ihn ANGESAGT - einmal je Takt,
 *              von StorageKycdStatPubSystem.
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

#include <cstdint>

namespace ase::storage {

/**
 * @brief StorageStaKycdStatComponent - the module's own issue counter
 *
 * Lives on the StorageMgrTag entity, raised by StorageKycdReqDrnSystem, published by
 * StorageKycdStatPubSystem. Nobody reads it back from the star - the star is the OUTPUT.
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageStaKycdStatComponent {
    uint32_t issued = 0;                      // keycards minted since process start
};

}  // namespace ase::storage
