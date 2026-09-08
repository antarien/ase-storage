#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        storage_acss_rslv_tag.hpp
 * @brief       StorageAcssRslvTag - Die Auflösung ist gelaufen, die Torleiter darf ansetzen
 * @description Marke auf einer Zugriffsanfrage, für die StorageAcssRslvSystem Revier und
 *              ACL-Regel aufgelöst hat, ohne sie dabei abzuweisen. Erst mit ihr betritt die
 *              Anfrage die Torleiter in StorageAcssChkSystem.
 *
 *              SIE IST DAS TOR, NICHT EINE NOTIZ. Ohne sie müsste die Torleiter am Vorhandensein
 *              der Auflösungs-Komponente ablesen, ob die Auflösung gelaufen ist - und eine
 *              Komponente, die nur durch ihre Abwesenheit spricht, ist genau die Laufzeitprüfung,
 *              die WRFL_ASE_TAGGED_VIEWS in den View-Filter verweist.
 *
 *              ABGEWIESENE ANFRAGEN BEKOMMEN SIE NICHT. Wer schon bei Keycard, Revier oder
 *              Verschleierung scheitert, trägt StorageAcssDenyTag und ist damit ohnehin aus
 *              jeder weiteren View heraus - die Leiter sieht ihn nie.
 *
 * storage = module, acss = access, rslv = resolve
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    tag/access
 * @parity      server_only
 *
 * PARITAET GEMESSEN AM 2026-08-30. ACHTUNG: die uebliche Bestandsableitung traegt hier NICHT —
 * ase-storage fuehrt 44 shared Components und client_relevance true, das Modul HAT einen
 * Client-Kanal. Getragen wird die Zeile von null Lesern ausserhalb des Moduls, null Treffern im
 * Web-Client bei 88 dortigen Storage*-Dateien (starke Positivkontrolle), und davon, dass eine
 * ACL-Aufloesung eine Autorisierungsentscheidung ist: der Client erfaehrt das ERGEBNIS, nie die
 * Ableitung. Volle Begruendung in storage_acss_ownr_tag.hpp.
 * @created     2026-08-29
 * @modified    2026-08-29
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
 */

namespace ase::storage {

/**
 * StorageAcssRslvTag - realm and ACL rule are resolved; the gate ladder may run
 * State: StorageAcssRslvSystem finished this request without denying it
 * Filter: StorageAcssChkSystem views request entities carrying it
 * Added: StorageAcssRslvSystem, together with StorageStaAcssRslvComponent
 * Removed: never - the request is destroyed once granted or denied downstream
 */
struct StorageAcssRslvTag {};

}  // namespace ase::storage
