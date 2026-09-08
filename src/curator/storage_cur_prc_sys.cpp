/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_cur_prc_sys.cpp
 * @brief       StorageCurPrcSystem - Curator request processing (business logic)
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @created     2026-04-06
 * @modified    2026-06-24
 * @version     1.0.0
 *
 * CAUSAL CHAIN (Curator Request Processing)
 *
 *   [curator_routes.cpp creates StorageReqCurComponent + StorageCurReqTag]
 *          │
 *          │ Request entity with key, project_ref, action, data
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageCurPrcSystem           │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageReqCurComponent (requests)      │
 *   │    - StorageCurReqTag (pending marker)      │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - StorageStaCurCurComponent (mutated)    │
 *   │    - Status Tags (Unrated/Approved/etc.)    │
 *   │    - StorageCurDoneTag (processed marker)   │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Curation entity mutated, request tagged done
 *          ▼
 *   curator_routes.cpp reads result → forwards to Replica
 *
 * HUB Pattern (ARCH_ASE_HUB)
 *
 * READS (from Hub):
 *   (none — curator business logic does not read Hub values)
 *
 * WRITES (to Hub):
 *   (none — persistence via Replica Server, not Hub)
 *
 * FLYWEIGHT PATTERN (Active - StorageResourceManager via ctx)
 *   Curation state stored directly in ECS components (no large data).
 *   Status discrimination via Tags, not ResourceManager.
 *
 * ECS SYSTEM IMPLEMENTATION COMPLIANCE
 *
 * [ ] Layer dependencies checked (only depend on lower layers)
 * [ ] Existing functions checked (ase-math, ase-utils, ase-containers)
 * [ ] Abbreviations defined in types.hpp or documentation
 * [ ] types.hpp created with all constants and enums
 * [ ] STATELESS? No member variables?
 * [ ] Views created on demand, not stored?
 * [ ] NO direct calls to other systems?
 * [ ] Communication only via Components?
 * [ ] Helpers in anonymous namespace (NOT static!)?
 * [ ] Math functions from ase-math (Layer 0)?
 * [ ] NO file-level static/constexpr?
 * [ ] Registered in Module with correct Schedule?
 * [ ] Filename matches convention?
 * [ ] Class name derived correctly from filename?
 * [ ] Using Deferred Deletion Pattern? (Tag + Batch Destroy)
 * [ ] NO destroy() on other entities during iteration?
 * [ ] Cleanup System in Schedule::Conclusion?
 * [ ] NO local arrays/vectors for collection?
 * [ ] Safe deletion (first collect, then delete)?
 * [ ] Not deleting other entities during iteration?
 * [ ] Not invalidating references during iteration?
 * [ ] 1 File = 1 System?
 * [ ] Folder structure matches convention?
 * [ ] components/, systems/, src/ have IDENTICAL subfolder structure?
 * [ ] Layer dependencies respected (no upward dependencies)?
 * [ ] NO inline nlohmann::json + .dump() in broadcast systems?
 * [ ] Serializer functions in anonymous namespace?
 * [ ] *NetBctReqSystem + *NetBctSndSystem pattern?
 * [ ] Math functions from ase-math? (lerp, clamp, noise)
 * [ ] Containers from ase-containers? (RingBuffer)
 * [ ] Types from ase-types? (Result, Option)
 * [ ] Utils from ase-utils? (UUID, hash)
 * [ ] No duplicate functionality across modules?
 * [ ] ONLY primitive types: int, float, uint32_t, bool, etc.
 * [ ] ONLY ase-math for math (NO std::min, std::max, std::clamp!)
 * [ ] ONLY ase-containers for containers (NO std::vector, std::map, std::unordered_map!)
 * [ ] ONLY ase-types for Result/Option (NO std::optional, std::expected!)
 * [ ] std:: FORBIDDEN except: <cstdint>, <cmath> basics, <cassert>
 * [ ] NO ARRAYS! (use Entity-per-Item + Tags!)
 * [ ] CAUSAL CHAIN documented (Input → Processing → Output)
 * [ ] HUB Pattern documented (READS/WRITES)
 * [ ] hub::get() for reads
 * [ ] hub::set() for writes
 * [ ] Method order: on_start → tick → on_stop
 * [ ] ALL THREE METHODS implemented
 * [ ] on_start/on_stop: log::debug with system name
 * [ ] log::warn() if value EXISTS but invalid (e.g., health < 0, temp > 1000)
 * [ ] log::error() for EVERY NOT_FOUND check (see ase-log/log.hpp ERR::CAT::*)
 * [ ] Unused params: (void)dt; or commented parameter name
 * [ ] NO switch/case statements? (use Tag-filtered Views or lookup tables!)
 * [ ] NO if-else chains for type dispatch? (use separate Systems per type!)
 * [ ] NO instanceof/dynamic_cast checks? (use Tags for entity classification!)
 * [ ] NO factory patterns with type enums? (use Component composition!)
 * [ ] NO inheritance hierarchies? (use Component composition!)
 * [ ] NO virtual dispatch for game logic? (only ecs::System base class allowed!)
 * [ ] NO singleton patterns? (use Manager Tags on entities!)
 * [ ] NO state machines with switch? (use Tag-based state + separate Systems!)
 * [ ] ALL behavior driven by Component DATA, not hardcoded logic?
 * [ ] NO hardcoded entity types? (types defined by Component composition!)
 * [ ] NO hardcoded processing order? (order via Schedule + run_after!)
 * [ ] NO hardcoded value ranges? (ranges in types.hpp constants!)
 * [ ] NO hardcoded special cases? (special cases = Tags + dedicated Systems!)
 * [ ] Formulas use Component fields, not magic numbers?
 * [ ] New behavior = new Component + new System, NOT if-else in existing code?
 * [ ] NO `find_*()` with View/Query? (use DUAL-PATTERN)
 * [ ] NO `check_*()`/`has_*()`/`is_*()` with View/Query? (use DUAL-PATTERN)
 * [ ] NO `get_*()` with View/Query? (use DUAL-PATTERN)
 * [ ] NO struct in namespace {}? (use Component)
 * [ ] NO collect-then-process? (use single-pass)
 * [ ] NO View/Query in Helper? (only pure math)
 * [ ] NO `bool has_*` for type categories in Components? (use Tags!)
 * [ ] NO `bool is_*` for type categories in Components? (use Tags!)
 * [ ] NO `uint8_t *_type` field with if-chain dispatch? (use Tag-filtered Views!)
 * [ ] Type determined by Tag composition, not boolean field?
 * [ ] N-item support via Entity-per-Item + Tags, not type booleans?
 * [ ] Tag-filtered Views per type, not if-chain in single loop?
 * [ ] NO Entity-per-Character pattern when loading strings?
 * [ ] String loading uses char[N] fixed arrays or Pointer Pattern?
 * [ ] String hashing via entt::hashed_string for lookup keys?
 * [ ] String data stored as single attribute, not per-character entities?
 * [ ] NO std::shared_ptr in Components? (use Flyweight Pattern!)
 * [ ] NO void* in Components? (use Flyweight Pattern!)
 * [ ] NO static std::unordered_map for resource storage? (use ResourceManager via ctx!)
 * [ ] External resources (shared_ptr, handles) accessed via registry.ctx().get<ResourceManager&>()?
 * [ ] ResourceManager registered in on_start() via registry.ctx().emplace<ResourceManager&>()?
 * [ ] Components store ONLY uint32_t IDs referencing external resources?
 */

// INCLUDES - ONLY THESE ARE ALLOWED!
// FORBIDDEN: <vector>, <map>, <unordered_map>, <optional>, <algorithm>
// ALLOWED:   <cstdint>, <cmath>, <cassert>, ase-* headers

// Own header FIRST
#include <ase/storage/systems/curator/storage_cur_prc_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_sta_cur_cur_comp.hpp>
#include <ase/storage/components/state/storage_req_cur_comp.hpp>
#include <ase/storage/components/state/storage_req_cur_prm_comp.hpp>
#include <ase/storage/components/state/storage_cur_asmt_comp.hpp>
#include <ase/storage/components/tag/storage_cur_req_tag.hpp>
#include <ase/storage/components/tag/storage_cur_done_tag.hpp>
#include <ase/storage/components/tag/storage_rvw_pend_tag.hpp>
#include <ase/storage/components/tag/storage_rvw_acpt_tag.hpp>
#include <ase/storage/components/tag/storage_rvw_rjct_tag.hpp>
#include <ase/storage/components/tag/storage_rvw_rvse_tag.hpp>
#include <ase/storage/storage_acss_index_resource_manager.hpp>
// types.hpp for constants (NO magic numbers!)
#include <ase/storage/types.hpp>
#include <ase/utils/strops.hpp>
// Logging
#include <ase/log/log.hpp>

namespace ase::storage {
using namespace entt::literals;  // For "_hs hashed strings (Hub)

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * IMPORTANT: Use anonymous namespace, NOT static keyword!
 *   OK: namespace { void helper() {...} }
 *   NO: static void helper() {...}
 * NO STRUCTS HERE! Structs = Data = Components!
 * NO View/Query in helpers! (pure math only)
 */
namespace {

/**
 * @brief Remove all curation status Tags from entity (no View, no Query)
 * Called before emplacing the new status Tag to ensure mutual exclusion.
 */
/* HIER STANDEN `remove_all_cur_tags` UND `emplace_cur_tag`, BEIDE MIT `ecs::Registry&`.
 *
 * Der zweite rief den ersten - eine Kette aus zwei verdeckten Markenschreibern hinter EINEM
 * Aufruf. Eine Funktion, die die Registry nimmt, ist kein Helfer, und kein Zaehler sieht sie
 * (INST_ASE_LINT.md, "Anonymous Namespace"). Beide Rumpfe stehen jetzt an ihrer einen
 * Aufrufstelle in tick(), wo die gegenseitige Ausschliessung der vier Marken am Code
 * ablesbar ist statt in zwei Funktionsnamen verteilt.
 *
 * Only pure functions over primitive types belong here - no registry, no view, no query. */
}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageCurPrcSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageCurPrcSystem] Started");
}

void StorageCurPrcSystem::tick(ecs::Registry& registry, float /*dt*/) {
    auto* idx_ptr = registry.ctx().find<StorageAcssIndexResourceManager*>();
    if (!idx_ptr || !(*idx_ptr)) {
        // SCHEDULE_ORDER: der Erzeuger (StorageAcssIdxSystem) hat den ctx-Halter noch nicht angelegt.
        log::error(log::ERR::CAT::SCHEDULE_ORDER, "StorageCurPrcSystem",
                   "StorageAcssIndexResourceManager");
        return;
    }
    auto& idx = **idx_ptr;

    // PASS 1: Process pending requests (single-pass)
    // View: requests with StorageCurReqTag, excluding already-processed (StorageCurDoneTag)
    auto req_view = registry.view<StorageReqCurComponent, StorageReqCurPrmComponent,
                                  StorageCurReqTag>(entt::exclude<StorageCurDoneTag>);

    for (auto req_entity : req_view) {
        auto& req = req_view.get<StorageReqCurComponent>(req_entity);
        auto& prm = req_view.get<StorageReqCurPrmComponent>(req_entity);

        // The curation row is reached by its (project, key) pair - the key the index is
        // built on. The former version walked EVERY curation row for EVERY request
        // (WS-K.2c) to answer a question that is one number against one number.
        entt::entity cur_entity = entt::null;
        const uint32_t req_key_hash = entt::hashed_string(req.key).value();
        const uint64_t cur_key =
            StorageAcssIndexResourceManager::compose_curation_key(req.project_ref, req_key_hash);
        const uint32_t found = idx.get_curation(cur_key);
        if (found != INVALID_ENTITY) {
            cur_entity = static_cast<entt::entity>(found);
        }

        // Create curation entity if not found
        if (cur_entity == entt::null) {
            cur_entity = registry.create();
            auto& cur = registry.emplace<StorageStaCurCurComponent>(cur_entity);
            ase::utils::str_copy(cur.key, CUR_MAX_KEY, req.key);
            cur.key_hash = req_key_hash;
            cur.project_ref = req.project_ref;
            registry.emplace<StorageCurAsmtComponent>(cur_entity);
            registry.emplace<StorageRvwPendTag>(cur_entity);
            // Registered IMMEDIATELY, not on the next rebuild: a second request naming the
            // same asset in this same pass must find THIS row, or both create their own
            // and the later rating lands on a duplicate nobody reads.
            idx.store_curation(cur_key, static_cast<uint32_t>(cur_entity));
            log::info("[StorageCurPrcSystem] Created curation entity for key '{}'", req.key);
        }

        auto& asmt = registry.get<StorageCurAsmtComponent>(cur_entity);

        // Apply action — separate View per action type not needed here
        // because action is from external input, not entity classification
        if (req.action == CUR_ACT_RATE) {
            if (prm.rating > 5) {
                // INPUT_REJECTED: die Eingabe kommt von AUSSEN, und der Bestand nennt genau
                // diesen Fall — "foreign caller sent something unserviceable", mit dem Zusatz
                // "caller must correct it - not an engine fault". Der Durchlauf geht weiter,
                // also die WRN-Haelfte. NICHT VALUE_INVALID: das beschriebe einen Wert, der IM
                // SYSTEM ungueltig wurde — hier ist die HERKUNFT der Unterschied.
                //
                // DIE FRUEHERE BEGRUENDUNG IST ABGELAUFEN: sie stuetzte die Wahl darauf, dass
                // VALUE_INVALID "Fix: Value will be set to default" ZUSAGE und damit eine
                // Korrektur behaupte, die hier nicht stattfindet. Der Bestand hat jede Zusage
                // ueber den Kontrollfluss zurueckgezogen. Dass der else-Zweig zuweist und
                // dieser Zweig nicht, bleibt als BEOBACHTUNG richtig — es traegt die
                // Kategoriewahl nur nicht mehr.
                log::warn(log::WRN::CAT::INPUT_REJECTED, "StorageCurPrcSystem", "cur_rating");
            } else {
                asmt.rating = prm.rating;
            }
        } else if (req.action == CUR_ACT_STATUS) {
            /**
             * VIER SICH AUSSCHLIESSENDE ZUSTANDSMARKEN: ERST ALLE VIER WEG, DANN EINE.
             *
             * Das Abraeumen steht VOR dem Setzen und raeumt ALLE vier ab, nicht nur die
             * zuletzt gesetzte. Ohne diesen Schritt traegt ein Eintrag, dessen Bewertung
             * wechselt, zwei einander ausschliessende Marken gleichzeitig - und keine Sicht
             * und kein Tor meldet das, weil beide Marken fuer sich korrekt aussehen.
             * `registry.remove` auf eine abwesende Marke ist ein No-op.
             *
             * DIE KETTE UNTEN IST KEIN TYP-DISPATCH, und der Unterschied ist wichtig:
             * `prm.target_tag` kommt aus EXTERNER EINGABE und steht erst zur Laufzeit fest.
             * Waeren es Konstanten der Aufrufstellen - wie bei den Marker-Sichten in
             * ase-recognition -, waere die Kette kuenstlich und muesste in tag-gefilterte
             * Sichten aufgeloest werden. Hier ist sie die einmalige Uebersetzung eines
             * eingegangenen Wertes in eine Marke.
             *
             * DER else-ZWEIG IST DIE VORGABE UND KEIN FEHLERFALL: ein unbekannter oder
             * fehlender Wert landet auf "ausstehend" - dem Zustand, in dem ein Eintrag ohne
             * Entscheidung stehen soll. Wer daraus einen Fehlerzweig macht, laesst Eintraege
             * ganz ohne Marke zurueck.
             */
            registry.remove<StorageRvwPendTag>(cur_entity);
            registry.remove<StorageRvwAcptTag>(cur_entity);
            registry.remove<StorageRvwRjctTag>(cur_entity);
            registry.remove<StorageRvwRvseTag>(cur_entity);

            if (prm.target_tag == CUR_ST_APPROVED) {
                registry.emplace<StorageRvwAcptTag>(cur_entity);
            } else if (prm.target_tag == CUR_ST_REJECTED) {
                registry.emplace<StorageRvwRjctTag>(cur_entity);
            } else if (prm.target_tag == CUR_ST_NEEDS_REWORK) {
                registry.emplace<StorageRvwRvseTag>(cur_entity);
            } else {
                registry.emplace<StorageRvwPendTag>(cur_entity);
            }
        } else if (req.action == CUR_ACT_NOTES) {
            ase::utils::str_copy(asmt.notes, CUR_MAX_NOTES, prm.notes);
        } else {
            // Gleiche Lage wie beim Rating: die Aktion kommt aus externer Eingabe und wird
            // zurueckgewiesen — der Aufrufer muss sie korrigieren, es ist kein Engine-Fehler.
            log::warn(log::WRN::CAT::INPUT_REJECTED, "StorageCurPrcSystem", "cur_action");
        }

        // Update metadata
        ase::utils::str_copy(asmt.user_id, CUR_MAX_USERID, req.user_id);
        // updated_at is set by the route handler (wall clock from HTTP request)

        // Mark request as processed (deferred deletion)
        registry.emplace<StorageCurDoneTag>(req_entity);
    }

    // PASS 2: Deferred cleanup of processed request entities
    auto done_view = registry.view<StorageCurDoneTag, StorageCurReqTag>();
    for (auto entity : done_view) {
        registry.destroy(entity);
    }
}

void StorageCurPrcSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageCurPrcSystem] Stopped");
}

}  // namespace ase::storage
