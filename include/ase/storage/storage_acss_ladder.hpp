#pragma once

/**
 * ASE STORAGE ACCESS-LADDER SHARED PARTS (module-internal)
 *
 * @file        storage_acss_ladder.hpp
 * @design      DSGN_021
 * @brief       The path hashes and the audit record the access ladder is written on
 * @description Die Zugriffsleiter nach ARCH Section 14.1 ist seit 2026-08-29 zwei Systeme:
 *              StorageAcssRslvSystem loest AUF (welches Revier, welche Regel, welche wirksame
 *              Keycard), StorageAcssChkSystem TORT (Gitter, Freigabestufe, Codewort, Recht,
 *              Etikett, Kenntnisnahme, Kontingent, Erteilung). Beide brauchen dieselben zwei
 *              Dinge, und beide muessen sie auf DIESELBE Weise haben - eine zweite Kopie waere
 *              eine zweite Bedeutung von "dieser Pfad liegt unter jenem", also eine Aenderung an
 *              der Zugriffskontrolle, getarnt als Doppelung.
 *
 *              DIE PRAEFIX-HASHES SIND DER GRUND, WARUM DIE LEITER O(1) IST. FNV-1a faltet von
 *              links nach rechts, der laufende Wert nach L Zeichen IST der Hash des
 *              L-Zeichen-Praefixes - ein Gang ueber den Pfad liefert damit jede Frage, die eine
 *              Ortsregel, ein Gitter-Anteil oder ein Kenntnisnahme-Bereich stellen kann.
 *
 *              DER PRUEFSATZ BLEIBT MIT. Der rollende Faltungswert MUSS dieselben Zahlen
 *              liefern wie entt, weil die Regelseite entt-Hashes speichert; eine stille
 *              Abweichung verweigert JEDEN Zugriff und sieht wie ein Datenproblem aus. Der
 *              static_assert macht daraus einen Uebersetzungsfehler.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @created     2026-08-29
 * @modified    2026-08-29
 * @version     1.0.0
 */

#include <cstdint>

#include <ase/ecs/system.hpp>
#include <ase/storage/components/state/storage_buf_audt_comp.hpp>
#include <ase/storage/components/state/storage_audt_outc_comp.hpp>
#include <ase/storage/components/tag/storage_audt_pend_tag.hpp>
#include <ase/storage/types.hpp>
#include <ase/utils/strops.hpp>

namespace ase::storage {

// The rolling fold below must produce the SAME numbers entt produces, because the rule
// side stores entt hashes. Asserting it at compile time turns a silent divergence - the
// kind that denies every access and looks like a data problem - into a build failure.
static_assert(((ACSS_FNV_OFFSET ^ static_cast<uint32_t>('a')) * ACSS_FNV_PRIME)
                  == entt::hashed_string::value("a", 1),
              "ACSS_FNV_* no longer match entt::hashed_string");

/**
 * HIER STAND `emit_audit(ecs::Registry&, ...)`, entfallen am 2026-08-31. Sie stellte die
 * Pruefspur-Zeile einer Zugriffsentscheidung: eine Entity, zwei Components, eine Marke. Eine
 * freie Funktion, die die Registry im Argument nimmt, ist ein verstecktes System — sie verbirgt
 * ECS-Arbeit hinter einem Namen, und dass sie in einem Header mit eigenem Gegenstand stand,
 * aendert daran nichts: der ORT einer Deklaration entscheidet nie, ob ein Zugriff verdeckt ist.
 *
 * WO DIE ZEILE JETZT ENTSTEHT — an jeder Stelle, die entscheidet, und offen im Rumpf:
 *
 *   storage_acss_rslv_sys.cpp   2 Ablehnungen (not_authenticated, realm_not_found)
 *   storage_acss_chk_sys.cpp    5 Ablehnungen der Schluessel-Tore
 *   storage_acss_pol_sys.cpp    4 Ablehnungen der Politik-Tore und die Gewaehrung
 *
 * ZWEI FORMEN, UND DER UNTERSCHIED IST EINE EIGENSCHAFT DES RUMPFES, KEINE STILFRAGE. In
 * storage_acss_pol_sys endet JEDER Pfad in einer Entscheidung; dort entsteht die
 * anfrageabgeleitete Haelfte der Zeile EINMAL je Anfrage, und die Ausgaenge setzen nur noch
 * Ergebnis und Grund. In storage_acss_chk_sys geht ein Pfad ohne Entscheidung weiter — der, der
 * StorageAcssPassTag setzt —, und eine vorgezogene Zeile bliebe dort als halbe Entity ohne
 * Ergebnis liegen. Deshalb stellt dieser Rumpf sie an jeder Ablehnung einzeln.
 *
 * DIE MAGISCHEN ZAHLEN SIND MITGEGANGEN: die entfallene Form schrieb `str_copy(..., 64, ...)`,
 * `256` und `64` als Literale. Ausgeschrieben stehen dort MAX_OWNER_ID, MAX_PATH_LEN und
 * MAX_REASON_LEN aus types.hpp — dieselben Werte, jetzt an ihre Quelle gebunden.
 */

// Hash of EVERY prefix of the path, in one pass: out[L] is the hash of path[0..L).
//
// FNV-1a folds left to right - hash = (hash ^ c) * prime - so the running value after L
// characters IS the hash of the L-character prefix. One walk of the path therefore
// yields every prefix hash a rule could ask about, and a location rule is then a single
// 32-bit equality. Before, every rule walked the path again.
//
// The semantics are the ones the character comparison had, EXACTLY: a location rule
// matched when the path began with the pattern, the pattern's own trailing wildcard
// character included. Nothing here widens or narrows that - a refactor that quietly
// changed which assets a rule governs would be a change to access control disguised as
// a performance fix.
inline void path_prefix_hashes(const char* path, uint32_t path_len, uint32_t* out) {
    uint32_t running = ACSS_FNV_OFFSET;
    out[0] = running;
    for (uint32_t i = 0; i < path_len; ++i) {
        running = (running ^ static_cast<uint32_t>(path[i])) * ACSS_FNV_PRIME;
        out[i + 1u] = running;
    }
}

// Hashes of every EXTENSION an asset path ends with: each suffix beginning at a '.'.
// "build-1.2.3.spdx.json" yields ".json", ".spdx.json", ".3.spdx.json" and so on, so an
// extension rule finds itself no matter how many dots the version number carries.
inline uint32_t path_extension_hashes(const char* path, uint32_t path_len, uint32_t* out_hash,
                                      uint32_t* out_len, uint32_t max_out) {
    uint32_t count = 0;
    for (uint32_t at = 0; at < path_len && count < max_out; ++at) {
        if (path[at] != '.') { continue; }
        const uint32_t len = path_len - at;
        out_hash[count] = entt::hashed_string::value(path + at, len);
        out_len[count] = len;
        ++count;
    }
    return count;
}

}  // namespace ase::storage
