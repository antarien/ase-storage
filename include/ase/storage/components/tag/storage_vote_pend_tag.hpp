#pragma once

/**
 * ASE ECS COMPONENT (TAG)
 *
 * @file        storage_vote_pend_tag.hpp
 * @brief       StorageVotePendTag - Die Vertrauensabstimmung nimmt noch Stimmen an
 * @description Marke auf einer Abstimmungs-Entity, solange sie Stimmen sammelt.
 *              `StgVotPrcSystem` filtert danach; die Marke faellt, wenn die Abstimmung
 *              endet - angenommen, abgelehnt oder verfallen.
 *
 *              ZWEI DINGE AM ALTEN NAMEN WAREN FALSCH. Die Datei hiess
 *              `storage_tag_vot_pend.hpp`: das Typwort `tag` stand in der MITTE, wo nach
 *              INST_ASE_ECS_SER Taxonomie steht - der Typ gehoert allein ins Suffix.
 *
 *              UND `vot` WAR KEIN KUERZEL, SONDERN EINE KUERZUNG. Der Katalog fuehrt das
 *              Wort "vote" als `vote` (15_soc_social.json, soc/econ/trd), also
 *              UNABGEKUERZT; "voting" hat gar keinen Eintrag. Ein Wort, das der Katalog
 *              ungekuerzt fuehrt, wird nicht gekuerzt - `vot` sah nur aus wie ein Kuerzel.
 *              Der Struct zieht mit, weil das Namenstor Datei und Struct gegeneinander
 *              prueft.
 *
 * storage = module, vote = vote, pend = pending
 *
 * @module      ase-storage
 * @layer       3 (Module)
 * @category    tag/vote
 * @parity      shared
 * @created     2026-04-04
 * @modified    2026-08-20
 * @version     2.0.0
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
 * @brief StorageVotePendTag - Vote of Confidence actively collecting ballots
 *
 * State: Members can cast ballots until deadline or threshold reached
 * Filter: Used by StgVotPrcSystem to evaluate open votes
 * Added: HTTP route handler when vote is initiated
 * Removed: StgVotPrcSystem when vote concludes (accepted/rejected/lapsed)
 */
struct StorageVotePendTag {};

}  // namespace ase::storage
