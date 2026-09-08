#pragma once

/**
 * ASE STORAGE KEYCARD-PAYLOAD SCAN (module-internal)
 *
 * @file        storage_kycd_payload.hpp
 * @brief       Bounded field extraction out of a keycard-authz JSON payload
 * @description Drei reine Byte-Sonden ueber einen begrenzten Puffer: einen Token finden,
 *              eine Zahl hinter '"key":' lesen, einen String hinter '"key":"' kopieren.
 *              Kein std::string, kein strstr, keine Allokation, keine Registry - jede
 *              Funktion sieht nur Zeiger, Laenge und Zielpuffer.
 *
 *              WARUM SIE HIER STEHEN UND NICHT IM ANONYMEN NAMENSRAUM DES DRAINERS:
 *              das Herausloesen von Feldern aus einem Nutzdatenblock ist eine ANDERE
 *              Zustaendigkeit als das Leeren einer Transportbahn und das Veroeffentlichen
 *              einer Sitzung. Solange beides in einer Datei lag, war der Schnitt zwischen
 *              ihnen unsichtbar; als Einheit mit eigenem Kopf ist er benannt und pruefbar.
 *              Dies ist die Bauform, die storage_acss_ladder.hpp im selben Modul vorfuehrt:
 *              deklarierter Namensraum, eigener Header, keine Kapselung von ECS-Logik.
 *
 *              DIE FUNKTIONEN SIND PUR UEBER PRIMITIVE TYPEN. Das ist die ausdruecklich
 *              erlaubte Helferform - was verboten ist, ist ein Helfer, der die Registry
 *              nimmt und damit eine View oder eine Hub-Lesung hinter einem Funktionsaufruf
 *              verbirgt. Diese drei nehmen sie nicht und koennen es nicht.
 *
 *              EINE DOPPELUNG IST BEKANNT UND NICHT VON HIER AUS ZU SCHLIESSEN:
 *              modules/ase-replication/src/replica/replica_sess_clrn_sys.cpp traegt
 *              parse_num_field und parse_str_field ein zweites Mal, mit gleicher Bedeutung.
 *              Ein Modul darf nicht auf ein anderes zugreifen; die gemeinsame Heimat waere
 *              ase-utils (L0), wo str_len bereits liegt und das beide ohnehin einschliessen.
 *              Das ist eine Foundation-Entscheidung ueber zwei Module und wird nicht
 *              nebenbei getroffen - hier steht sie als benannter Befund, nicht als Kopie.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @created     2026-08-31
 * @modified    2026-08-31
 * @version     1.0.0
 */

#include <cstdint>

#include <ase/storage/types.hpp>
#include <ase/utils/strops.hpp>

namespace ase::storage {

// Find the first occurrence of needle in [doc, doc+len). Returns the index of the
// match start, or -1 when absent. Pure byte scan (no std::string, no strstr).
inline int32_t find_token(const char* doc, uint32_t len, const char* needle) {
    uint32_t nlen = ase::utils::str_len(needle, EDGE_KYCD_PAYLOAD_MAX);
    if (nlen == 0u || nlen > len) return -1;
    for (uint32_t i = 0; i + nlen <= len; ++i) {
        uint32_t j = 0;
        while (j < nlen && doc[i + j] == needle[j]) ++j;
        if (j == nlen) return static_cast<int32_t>(i);
    }
    return -1;
}

// Parse the unsigned integer value of a '"key":<digits>' field into out_val.
// Returns true when the key + a digit run is found. Bounded scan; tolerates
// whitespace between the colon and the first digit.
inline bool parse_num_field(const char* doc, uint32_t len, const char* quoted_key,
                            uint32_t& out_val) {
    int32_t at = find_token(doc, len, quoted_key);
    if (at < 0) return false;
    uint32_t i = static_cast<uint32_t>(at) + ase::utils::str_len(quoted_key, EDGE_KYCD_PAYLOAD_MAX);
    while (i < len && (doc[i] == ' ' || doc[i] == ':')) ++i;
    bool any = false;
    uint32_t v = 0;
    while (i < len && doc[i] >= '0' && doc[i] <= '9') {
        v = v * 10u + static_cast<uint32_t>(doc[i] - '0');
        any = true;
        ++i;
    }
    if (!any) return false;
    out_val = v;
    return true;
}

// Copy the string value of a '"key":"<value>"' field into out (bounded). Returns
// true when the key + a quoted value is found. Bounded scan.
inline bool parse_str_field(const char* doc, uint32_t len, const char* quoted_key,
                            char* out, uint32_t out_size) {
    if (out_size == 0u) return false;
    out[0] = '\0';
    int32_t at = find_token(doc, len, quoted_key);
    if (at < 0) return false;
    uint32_t i = static_cast<uint32_t>(at) + ase::utils::str_len(quoted_key, EDGE_KYCD_PAYLOAD_MAX);
    while (i < len && (doc[i] == ' ' || doc[i] == ':')) ++i;
    if (i >= len || doc[i] != '"') return false;
    ++i;  // opening quote
    uint32_t o = 0;
    while (i < len && doc[i] != '"' && o + 1u < out_size) {
        out[o++] = doc[i++];
    }
    out[o] = '\0';
    return o > 0u;
}

}  // namespace ase::storage
