#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        storage_acss_pass_tag.hpp
 * @brief       StorageAcssPassTag - die Schlüssel-Tore sind passiert, die Politik-Tore dürfen ansetzen
 * @description Marke auf einer Zugriffsanfrage, die StorageAcssChkSystem durch Revier, Gitter,
 *              Schutzstufe, Codewort und Berechtigung gebracht hat, ohne sie abzuweisen. Erst mit
 *              ihr betritt die Anfrage die Politik-Tore in StorageAcssPolSystem: Label,
 *              Kenntnisnahme, Kontingent, Gewährung.
 *
 *              SIE IST DAS TOR, NICHT EINE NOTIZ — dieselbe Bauform wie StorageAcssRslvTag eine
 *              Stufe davor. Ohne sie müsste der Politik-Durchgang die fünf Schlüssel-Tore erneut
 *              stellen, um zu wissen, ob sie gehalten haben; das wäre eine zweite Stelle, an der
 *              über Zulässigkeit entschieden wird, und zwei Stellen laufen auseinander.
 *
 *              ABGEWIESENE ANFRAGEN BEKOMMEN SIE NICHT. Wer an einem der fünf Tore scheitert,
 *              trägt StorageAcssDenyTag und ist damit ohnehin aus jeder weiteren View heraus.
 *
 *              WARUM DIE LEITER ÜBERHAUPT GETEILT IST: die Prüfspur-Zeile wurde bis zum 2026-08-31
 *              von einem Helfer mit `ecs::Registry&` im Argument gestellt — ein verstecktes System.
 *              Ausgeschrieben trägt sie jede der zehn Ausgangsstellen selbst, und der Rumpf fiele
 *              damit in das GOD_SYSTEM_UNSPLIT_BAND. Die Naht liegt zwischen zwei Fragen, die
 *              ohnehin verschieden sind: WER DARF (Ausweis, Revier, Stufe, Codewort, Recht) und
 *              WAS GILT FÜR DIESEN GEGENSTAND (Label, laufende Aufgabe, Kontingent).
 *
 * storage = module, acss = access, pass = passed
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    tag/access
 * @parity      server_only
 *
 * PARITAET ABGELEITET VON StorageAcssRslvTag, derselbe Gegenstand eine Stufe später: eine
 * ACL-Zwischenstufe ist eine Autorisierungsentscheidung, und der Client erfaehrt das ERGEBNIS,
 * nie die Ableitung. Die volle Begruendung samt Positivkontrolle steht in
 * storage_acss_ownr_tag.hpp; sie gilt fuer diese Marke unveraendert, weil sie denselben Weg
 * beschreibt.
 *
 * DIE ZEILE IST PFLICHT UND HAT GENAU DREI WERTE: server, client oder shared. Sie wird NIE
 * geloescht — eine fehlende Paritaetsangabe erzeugt keine Erkennung, sie laesst die Frage
 * unbeantwortet. Am 2026-08-31 war sie hier kurzzeitig entfernt; das war falsch und ist
 * zurueckgenommen.
 * @created     2026-08-31
 * @modified    2026-08-31
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
 * StorageAcssPassTag - the key gates held; the policy gates may run
 * State: StorageAcssChkSystem finished the key gates for this request without denying it
 * Filter: StorageAcssPolSystem views request entities carrying it
 * Added: StorageAcssChkSystem, after the permission gate
 * Removed: never - the request is destroyed once granted or denied downstream
 */
struct StorageAcssPassTag {};

}  // namespace ase::storage
