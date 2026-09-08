#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        storage_acss_ownr_tag.hpp
 * @brief       StorageAcssOwnrTag - Der Anrufer besitzt dieses Revier oder einen seiner Vorfahren
 * @description Marke auf einer Zugriffsanfrage, deren Anrufer als Eigentümer des Ziel-Reviers
 *              aufgelöst wurde - direkt oder über einen Pfad-Vorfahren: wer "org/adg" besitzt,
 *              regiert auch "org/adg/projects/x".
 *
 *              SIE IST DIE IN-LEITER-FORM DER FRÜHEREN realm_owner-MACHT. Vor der Leiter stand
 *              einmal eine Abkürzung, die den Eigentümer an allen Toren vorbeiließ; heute läuft
 *              er DURCH die Leiter, nur mit der Eigentümer-Vorgabe als wirksamer Keycard:
 *              Freigabestufe ACSS_OWNER_CLEARANCE, alle Rechte, Codewort-Joker. Verschleierung,
 *              Gitter und Kenntnisnahme fragen die Marke ebenfalls - sie sind für den Eigentümer
 *              gegenstandslos, nicht übersprungen.
 *
 *              WARUM ALS MARKE UND NICHT ALS FELD (2026-08-29): die Auflösung, die den Eigentümer
 *              feststellt, und die Leiter, die ihn berücksichtigt, sind seit der Trennung zwei
 *              Systeme. Ein bool in einer Komponente wäre genau der Typ-Schalter, den die
 *              ECS-Regeln in einen View-Filter verweisen.
 *
 * storage = module, acss = access, ownr = owner
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    tag/access
 * @parity      server_only
 *
 * DIE PARITAETSENTSCHEIDUNG, GEMESSEN AM 2026-08-30 — sie gilt fuer diesen Tag und die beiden
 * Geschwister, die Phase A am selben Tag als unentschieden gemeldet hat: StorageAcssRslvTag und
 * StorageStaAcssRslvComponent. Ohne sie brach Phase A fuer das GANZE Modul ab.
 *
 * DIE SONST TRAGENDE BESTANDSABLEITUNG SAGT HIER DAS GEGENTEIL, und das ist der Grund fuer die
 * Ausfuehrlichkeit: ase-storage fuehrt 44 SHARED Components und `client_relevance: true`. Das
 * Modul HAT einen Client-Kanal — anders als ase-replication oder ase-pl-redis, wo „kein shared
 * im Bestand" die Entscheidung trug. Hier traegt sie NICHT, und wer sie trotzdem anwendet,
 * bekommt das falsche Vorzeichen.
 *
 * WAS TRAEGT, sind drei Messungen und ein Sicherheitsargument:
 *   LESER IM BAUM:   null ausserhalb von modules/ase-storage — gesucht ueber modules, plugins,
 *                    servers und core, fuer alle drei Namen.
 *   LESER IM CLIENT: null. Die Positivkontrolle ist hier BESONDERS stark: der Web-Client fuehrt
 *                    88 Dateien mit Storage*-Typen. Er kennt dieses Modul sehr gut — er kennt
 *                    nur diese drei nicht. Waeren es null Storage-Dateien, waere die Null eine
 *                    Nichtmessung; so ist sie eine Aussage.
 *   WAS SIE SIND:    die ACL-Aufloesung selbst. Dieser Tag sagt „der Anrufer besitzt dieses
 *                    Revier", der zweite „die Aufloesung ist gelaufen", die Component haelt,
 *                    was sie gefunden hat.
 *   UND DESHALB:     eine Autorisierungsentscheidung gehoert nicht in den Browser. Wer die
 *                    aufgeloeste Torleiter kennt, kennt die Regel, gegen die er probiert. Der
 *                    Client erfaehrt das ERGEBNIS einer Anfrage, nie die Ableitung dahinter —
 *                    dieselbe Linie wie DEFAULT-DENY bei Routen zum Client.
 *
 * DASS DAS MODUL EINEN KANAL HAT, MACHT NICHT JEDE COMPONENT ZU EINER, DIE IHN NEHMEN DARF.
 * Genau diese Verwechslung ist der teure Fehler bei einem gemischten Bestand.
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
 * StorageAcssOwnrTag - the caller owns the target realm or one of its path ancestors
 * State: the owner keycard preset applies to this request for the whole ladder
 * Filter: StorageAcssChkSystem views request entities carrying it, per gate
 * Added: StorageAcssRslvSystem while resolving realm ownership
 * Removed: never - the request is destroyed once granted or denied downstream
 */
struct StorageAcssOwnrTag {};

}  // namespace ase::storage
