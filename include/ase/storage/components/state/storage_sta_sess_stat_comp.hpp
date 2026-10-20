#pragma once

/**
 * ASE ECS COMPONENT (STATE)
 *
 * @file        storage_sta_sess_stat_comp.hpp
 * @brief       StorageStaSessStatComponent - die Zaehlzeile der Sitzungsstufen dieses Moduls
 * @description Wie viele Keycards an eine Sitzung gebunden wurden und wie viele abgelaufen sind,
 *              gefuehrt auf der Verwalterentity. Die Zeile ist die QUELLE der Hub-Zeilen
 *              STG_KYCD_LNK_COUNT und STG_KYCD_EXP_COUNT, nicht ihr Spiegel: ein Zaehler, der im
 *              Hub hochgezaehlt wird, mischt Hub-I/O mit Rechnung (HUB_IO_MIXED_WITH_MATH), und
 *              ein Hub-Wert ist EIN stehender Platz, auf dem bei zehn Ereignissen eines Takts nur
 *              der letzte Schrieb etwas bedeutet.
 *
 * @usecase     usecase:100265 - check access clearance
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    state
 * @parity      server_only
 * @created     2026-10-13
 * @modified    2026-10-13
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
 * @brief StorageStaSessStatComponent - the module's own session stage counters
 *
 * Lives on the StorageMgrTag entity; the stage system that counts first places it there.
 * Raised by StorageKycdLnkCntSystem and StorageKycdExpSystem, published by
 * StorageKycdStatPubSystem. Nobody reads it back from the star - the star is the OUTPUT.
 *
 * @hub_reads  none
 * @hub_writes none
 */
struct StorageStaSessStatComponent {
    uint32_t linked = 0;                      // keycards linked to a session since process start
    uint32_t expired = 0;                     // keycards that expired since process start
};

}  // namespace ase::storage
