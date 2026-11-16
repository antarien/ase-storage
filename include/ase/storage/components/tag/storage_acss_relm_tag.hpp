#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        storage_acss_relm_tag.hpp
 * @brief       StorageAcssRelmTag - Keycard und Revier stehen, der Regelabgleich darf ansetzen
 * @description Marke auf einer Zugriffsanfrage, deren Keycard StorageAcssRslvSystem als gueltig
 *              und deren Revier es als sichtbar festgestellt hat (Sprossen 1 und 2 der Leiter,
 *              ARCH_ASE_STORAGE Section 14.1). Erst mit ihr sucht StorageAcssRuleSystem die
 *              ACL-Regel, die den Pfad regiert.
 *
 *              SIE IST DAS TOR ZWISCHEN DEN BEIDEN NACHSCHLAG-SYSTEMEN. Revier-Sichtbarkeit und
 *              Regelabgleich sind zwei Fragen mit zwei verschiedenen Rechnungen - Pfad-Vorfahren
 *              gegen den Revier-Index, Pfad-Hashes gegen die Regel-Eimer - und die Marke traegt
 *              das Ergebnis der ersten in die View der zweiten, statt es dort aus dem Fehlen
 *              einer Abweisung abzulesen.
 *
 *              ABGEWIESENE ANFRAGEN BEKOMMEN SIE NICHT. Wer bei Keycard, Revier oder
 *              Verschleierung scheitert, traegt StorageAcssDenyTag und ist damit aus jeder
 *              weiteren View heraus.
 *
 * storage = module, acss = access, relm = realm
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    tag/access
 * @parity      server_only
 * @design      DSGN_021
 * @usecase     usecase:100265 - check access clearance
 * @actor       Provider Credential Keeper (PROVCREKEE)
 *
 * PARITAET: dieselbe Lage wie bei StorageAcssRslvTag und StorageAcssOwnrTag - eine Stufe der
 * Autorisierungsableitung; der Client erfaehrt das ERGEBNIS einer Zugriffsanfrage, nie ihre
 * Ableitung. Volle Begruendung in storage_acss_ownr_tag.hpp.
 * @created     2026-11-16
 * @modified    2026-11-16
 * @version     00.00.39.00039 [core]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
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
 */

namespace ase::storage {

/**
 * StorageAcssRelmTag - keycard valid and realm visible; the ACL rule match may run
 * State: StorageAcssRslvSystem passed rungs 1 and 2 for this request without denying it
 * Filter: StorageAcssRuleSystem views request entities carrying it; StorageAcssRslvSystem
 *         excludes them, so the realm check runs exactly once per request
 * Added: StorageAcssRslvSystem, after its walk
 * Removed: never - the request is destroyed once granted or denied downstream
 */
struct StorageAcssRelmTag {};

}  // namespace ase::storage
