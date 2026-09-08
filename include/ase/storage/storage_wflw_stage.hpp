#pragma once

/**
 * ASE STORAGE WORKFLOW STAGE VOCABULARY (module-internal)
 *
 * @file        storage_wflw_stage.hpp
 * @brief       The stage names, their ordinals, and the audit record a transition leaves
 * @description WAS EINE STUFE IST und WER SICH VON EINER ZUR NAECHSTEN BEWEGT sind zwei Dinge.
 *              Das Vokabular - welcher Etikett-Hash welche Stufe bedeutet, wie ein
 *              Uebergangsgrund lesbar zusammengesetzt wird, wie der Pruefsatz eines Entscheids
 *              aussieht - gehoert nicht in den Rumpf des Systems, das den Uebergang faehrt.
 *              Seit der Trennung von 2026-08-29 lesen es zwei: StorageWflwIniSystem, das einen
 *              unverwalteten Build als Entwurf adoptiert, und StorageWflwTranSystem, das ihn
 *              weiterbewegt.
 *
 *              DER PRUEFSATZ BLEIBT BEWUSST EINE EIGENE FUNKTION: er traegt IMMER AUD_PROMOTE und
 *              kein Projekt, weil ein Stufenwechsel keins hat. Zwei Formen mit zwei festen
 *              Bedeutungen sind ehrlicher als eine mit zwei Sonderfaellen.
 *
 *              HIER STAND, ER SEI DAS SPIEGELBILD VON `emit_audit` IN storage_acss_ladder.hpp.
 *              DIESE FUNKTION GIBT ES SEIT DEM 2026-08-31 NICHT MEHR: sie nahm `ecs::Registry&`
 *              und war damit ein verstecktes System; ihre Arbeit steht jetzt offen in den drei
 *              Ruempfen der Zugriffsleiter. Der Verweis blieb als lebender Name ueber einer toten
 *              Sache stehen — die teurere Sorte toter Verweis, weil sie nicht scheitert, sondern
 *              etwas Falsches ueber eine Datei behauptet, die es noch gibt.
 *
 *              WAS DER SATZ RICHTIG SAGTE, GILT WEITER, nur ohne Vorbild: dieser Pruefsatz ist
 *              REIN — er nimmt Werte und gibt Text zurueck, keine Registry. Genau deshalb hat ihn
 *              die Aufraeumrunde nicht getroffen.
 *
 *              DIE ORDINALZUORDNUNG IST EINE TABELLE, KEIN switch. Die Etiketten kommen als
 *              Hashes an, weil Identitaet ein Nachschlagen ist und ein Nachschlagen Hashes
 *              vergleicht, nie Zeichen (WRFL_ASE_STRING_HANDLING Section 3) - vier
 *              32-Bit-Tests, wo die fruehere Leiter bis zu vier Zeichenketten-Laeufe je Anfrage
 *              fuhr.
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

// DIE AUDIT-ZEILE WIRD HIER NICHT MEHR GESCHRIEBEN — die Zeilen stehen seit 2026-08-31 an ihren
// vier Entscheidungsstellen (storage_wflw_ini_sys, storage_wflw_perm_sys, zweimal
// storage_wflw_tran_sys).
//
// An dieser Stelle stand `emit_tran_audit(ecs::Registry&, uint32_t, const char*, const char*,
// uint64_t, uint8_t, const char*)`: eine Registry im Argument, ein `create` und drei `emplace` —
// also ECS-Arbeit hinter einem Namen, und im Header fuer jede einbindende Einheit sichtbar. Ein
// Helfer darf pur ueber primitive Typen und Component-Felder rechnen; sobald er die Registry
// braucht, gehoert die Arbeit inline.
//
// WAS DIE FORM ZUSICHERTE, GILT UNVERAENDERT UND STEHT JETZT AN JEDER DER VIER STELLEN: EINE
// Entity je Entscheidung, mit StorageAudtPendTag fuer den Preservation-Stapelschreiber, und die
// Identitaet des Anfragenden IST die Keycard-Zuschreibung, die der Vertrag verlangt. `proj_ref`
// bleibt 0 (ein Uebergang gehoert einem Realm, keinem Projekt) und `action` bleibt AUD_PROMOTE —
// beide waren im Rumpf fest verdrahtet und sind es an jeder Stelle weiterhin.
//
// `compose_edge_reason` darunter bleibt, wo es ist: es rechnet PUR ueber Zeichenketten, ohne
// Registry, ohne View — genau die Haelfte, die ein Helfer sein darf.

// Bounded "wflw...(from->to)" reason composition. Pure string math, no views.
inline void compose_edge_reason(char* out, uint32_t out_size, const char* prefix,
                                const char* from_label, const char* to_label) {
    ase::utils::str_copy(out, out_size, prefix);
    ase::utils::str_append(out, out_size, "(");
    ase::utils::str_append(out, out_size, from_label);
    ase::utils::str_append(out, out_size, "->");
    ase::utils::str_append(out, out_size, to_label);
    ase::utils::str_append(out, out_size, ")");
}

// Label hash → display stage ordinal (WFLW_STAGE_*). Sequential value mapping over
// the fixed label chain. The labels arrive as hashes because identity is a lookup and
// a lookup compares hashes, never characters (WRFL_ASE_STRING_HANDLING Section 3) —
// four 32-bit tests where the former ladder ran up to four string walks per request.
inline float stage_ordinal(uint32_t label_hash) {
    if (label_hash == EDGE_LABEL_REVIEW_HASH)   return static_cast<float>(WFLW_STAGE_REVIEW);
    if (label_hash == EDGE_LABEL_APPROVED_HASH) return static_cast<float>(WFLW_STAGE_APPROVED);
    if (label_hash == EDGE_LABEL_RELEASED_HASH) return static_cast<float>(WFLW_STAGE_RELEASED);
    if (label_hash == EDGE_LABEL_RETIRED_HASH)  return static_cast<float>(WFLW_STAGE_RETIRED);
    return static_cast<float>(WFLW_STAGE_DRAFT);
}

}  // namespace ase::storage
