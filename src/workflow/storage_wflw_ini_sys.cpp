/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_wflw_ini_sys.cpp
 * @brief       StorageWflwIniSystem - Adopt an unmanaged on-disk build as a draft
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @created     2026-08-29
 * @modified    2026-08-29
 * @version     1.0.0
 *
 * TRENNUNG 2026-08-29 — ADOPTIEREN UND BEWEGEN WAREN EINE tick()
 *
 * Einen unverwalteten Build in den Arbeitsablauf AUFNEHMEN und ein bereits aufgenommenes Gut von
 * einer Stufe zur naechsten BEWEGEN sind zwei Vorgaenge. Sie standen in derselben Funktion, und
 * der erste sah dort aus wie ein Sonderfall des zweiten - dabei hat er eine eigene Bedingung
 * (gibt es die Datei ueberhaupt?), einen eigenen Ausgang (wflw_no_asset) und ein eigenes
 * Ergebnis (eine neue Regel, kein Uebergang).
 *
 * EIN GUT OHNE REGEL IST DER ENTWURF. Publish legt Builds direkt ins Revier; die erste
 * Beförderung trifft also auf ein Gut, das noch keine ACL-Regel regiert. Das ist kein Fehler,
 * sondern der Anfangszustand - und er wird datengetrieben hergestellt (Spiegel der
 * EDGE_LABEL_DRAFT-Semantik), nicht als Sonderzweig im Uebergang.
 *
 * WAS NICHT AUF DER PLATTE LIEGT, WIRD NICHT ADOPTIERT. Eine Beförderung fuer einen Pfad ohne
 * Datei endet hier: Ergebnis NOT_FOUND im Hub, Pruefsatz, Fehlerzeile, Anfrage abgeschlossen.
 * Kein spaeterer Durchlauf holt sie nach - es gibt nichts, worauf er warten koennte.
 *
 * CAUSAL CHAIN (CHAIN_STORAGE_WFLW_INI: Request → Draft rule)
 *
 *   [HTTP-Route legt die Beförderungs-Anfrage an]
 *          │
 *          │ StorageReqWflwTranComponent + StorageWflwPendTag
 *          ▼
 *   ┌─────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageWflwIniSystem                          │
 *   │                                                             │
 *   │  READS:                                                     │
 *   │    StorageReqWflwTranComponent (Pfad, Ziel-Etikett)         │
 *   │    StorageRuleIdnComponent (gibt es schon eine Regel?)      │
 *   │    StorageResourceManager (liegt die Datei auf der Platte?) │
 *   │                                                             │
 *   │  WRITES:                                                    │
 *   │    StorageAcssRuleComponent + StorageRuleIdnComponent       │
 *   │        (die Entwurfs-Regel des adoptierten Guts)            │
 *   │    STG_WFLW_RES = NOT_FOUND + Pruefsatz, wenn nichts da ist │
 *   └─────────────────────────────────────────────────────────────┘
 *          │
 *          │ die Regel steht - die Anfrage bleibt offen
 *          ▼
 *   StorageWflwTranSystem - prueft Recht und Kante und bewegt das Gut
 *
 * HUB Pattern (ARCH_ASE_HUB)
 *
 * READS (from ase-storage internal):
 *   StorageReqWflwTranComponent → the path a promote names
 *   StorageRuleIdnComponent     → whether a rule already governs that path
 *
 * WRITES (over the hub API and to ase-storage internal):
 *   STG_WFLW_RES                → WFLW_RES_NOT_FOUND when the asset is not on disk
 *   StorageAcssRuleComponent    → the bootstrapped draft rule of an adopted build
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
 * [ ] NO switch/case statements? (use Tag-filtered Views!)
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
#include <ase/storage/systems/workflow/storage_wflw_ini_sys.hpp>
// Components from same module
#include <ase/storage/components/request/storage_req_wflw_tran_comp.hpp>
#include <ase/storage/components/state/storage_rule_idn_comp.hpp>
#include <ase/storage/components/state/storage_acss_rule_comp.hpp>
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/tag/storage_wflw_pend_tag.hpp>
#include <ase/storage/components/tag/storage_relm_edge_tag.hpp>
#include <ase/storage/components/tag/storage_wflw_gate_tag.hpp>
#include <ase/storage/storage_acss_index_resource_manager.hpp>
#include <ase/storage/storage_resource_manager.hpp>
#include <ase/storage/storage_wflw_stage.hpp>
#include <ase/storage/types.hpp>
// Hub API (verdict publish)
#include <ase/hub/api.hpp>
// String ops (L0)
#include <ase/utils/strops.hpp>

#include <entt/core/hashed_string.hpp>
// Logging
#include <ase/log/log.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
// NO View/Query operations in helpers! Only pure math!
namespace {

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageWflwIniSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageWflwIni] Started");
}

void StorageWflwIniSystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    auto* mgr_ptr = registry.ctx().find<StorageResourceManager*>();
    if (!mgr_ptr || !(*mgr_ptr)) {
        return;  // manager not up yet (StorageIniSystem seeds it at Initialization)
    }
    auto& mgr = **mgr_ptr;

    auto* idx_ptr = registry.ctx().find<StorageAcssIndexResourceManager*>();
    if (!idx_ptr || !(*idx_ptr)) {
        log::error(log::ERR::CAT::SCHEDULE_ORDER, "StorageWflwIniSystem",
                   "StorageAcssIndexResourceManager");
        return;
    }
    auto& idx = **idx_ptr;

    // Deferred deletion: only the denied requests are collected here; an adopted one stays
    // pending and is carried by StorageWflwTranSystem in the same tick.
    ecs::Entity done[WFLW_REQ_BATCH];
    uint32_t done_n = 0;

    // Edge realm entity ref (rule scope + audit record). The answer does not depend on
    // the request, so it is resolved ONCE per tick instead of once per request. The tag
    // carries the identity (StorageEdgeIniSystem is its only producer).
    uint32_t relm_ref = 0;
    for (auto relm_ent : registry.view<StorageStaRelmComponent, StorageRelmEdgeTag>()) {
        relm_ref = static_cast<uint32_t>(relm_ent);
        break;
    }

    // Requests still carrying StorageWflwGateTag belong to StorageWflwGateSystem.
    auto req_view = registry.view<StorageReqWflwTranComponent, StorageWflwPendTag>(
        entt::exclude<StorageWflwGateTag>);
    for (auto [req_ent, req] : req_view.each()) {
        if (done_n >= WFLW_REQ_BATCH) break;

        // Locate the per-asset ACL rule (EXACT pattern match, realm-scoped). Only the rules
        // of THIS realm are examined - the realm filter is the bucket itself, taken from the
        // index StorageAcssIdxSystem rebuilt earlier this tick.
        const uint32_t req_path_hash = entt::hashed_string(req.path).value();
        bool rule_exists = false;
        const uint32_t rule_count = idx.get_rule_count(relm_ref);
        for (uint32_t rule_index = 0; rule_index < rule_count; ++rule_index) {
            const uint32_t rule_id = idx.get_rule(relm_ref, rule_index);
            if (rule_id == INVALID_ENTITY) continue;
            auto* r_idn =
                registry.try_get<StorageRuleIdnComponent>(static_cast<ecs::Entity>(rule_id));
            if (r_idn == nullptr) continue;
            if (r_idn->pattern_hash != req_path_hash) continue;
            rule_exists = true;
            break;
        }
        if (rule_exists) {
            continue;  // already managed - the transition system takes it from here
        }

        const uint32_t owner = entt::hashed_string(req.path).value();
        const uint64_t now = mgr.get_wall_time_seconds();

        // No rule yet: an on-disk build without one IS the draft stage — publish
        // deposits builds straight into the realm; the first promote adopts them
        // (data-driven bootstrap, mirror of EDGE_LABEL_DRAFT semantics).
        char asset_abs[512] = {};
        mgr.resolve_path(EDGE_REALM_ID, nullptr, req.path, asset_abs, 512);
        if (!mgr.file_exists(asset_abs)) {
            hub::set(registry, owner, "STG_WFLW_RES"_hs, static_cast<float>(WFLW_RES_NOT_FOUND));
            // EINE Audit-Entity je Entscheidung, mit StorageAudtPendTag fuer den
            // Preservation-Stapelschreiber. Die Identitaet des Anfragenden IST die
            // Keycard-Zuschreibung, die der Vertrag verlangt. proj_ref bleibt 0 (ein Uebergang
            // gehoert einem Realm, keinem Projekt), action bleibt AUD_PROMOTE.
            auto aud_ent = registry.create();
            auto& aud = registry.emplace<StorageBufAudtComponent>(aud_ent);
            aud.relm_ref = relm_ref;
            aud.proj_ref = 0;
            ase::utils::str_copy(aud.user_id, MAX_OWNER_ID, req.requested_by);
            ase::utils::str_copy(aud.path, MAX_PATH_LEN, req.path);
            aud.timestamp = now;
            auto& outc = registry.emplace<StorageAudtOutcComponent>(aud_ent);
            outc.action = AUD_PROMOTE;
            outc.result = AUD_DENIED;
            ase::utils::str_copy(outc.reason, MAX_REASON_LEN, "wflw_no_asset");
            registry.emplace<StorageAudtPendTag>(aud_ent);
            // Diese Zeile ging schon vor der Migration der beiden DENIED-Zeilen glatt
            // durch: sie fuehrt genau EINEN Bezeichner (req.path), und der ist bei
            // RESOURCE_UNAVAIL die Kennung der Ressource selbst — Punkt 1 des Hilfetexts
            // fragt woertlich, ob sie existiert und erreichbar ist. owner steht in
            // Reichweite und bleibt erhalten, es geht nichts verloren.
            log::error(log::ERR::CAT::RESOURCE_UNAVAIL, "StorageWflwIni", owner, req.path);
            done[done_n] = req_ent;
            ++done_n;
            continue;
        }

        auto new_rule_ent = registry.create();
        auto& r = registry.emplace<StorageAcssRuleComponent>(new_rule_ent);
        r.relm_ref = relm_ref;
        r.proj_ref = 0;
        ase::utils::str_copy(r.path_pattern, MAX_PATH_LEN, req.path);
        r.protection_level = PROTECTION_PUBLIC;
        ase::utils::str_copy(r.label, MAX_LABEL_LEN, EDGE_LABEL_DRAFT);
        // Identity beside the record, in the same breath as the strings.
        // A bootstrapped rule governs exactly one asset, so its pattern carries no
        // wildcard: pattern and literal match are the same string, and it is a
        // location rule (no StorageAcssRuleSufxTag).
        auto& r_idn = registry.emplace<StorageRuleIdnComponent>(new_rule_ent);
        r_idn.pattern_hash = req_path_hash;
        r_idn.label_hash = EDGE_LABEL_DRAFT_HASH;
        r_idn.match_hash = req_path_hash;
        r_idn.match_len = ase::utils::str_len(req.path, MAX_PATH_LEN);
        log::info("[StorageWflwIni] Draft rule bootstrapped for on-disk asset {}", req.path);
    }

    for (uint32_t i = 0; i < done_n; ++i) {
        registry.destroy(done[i]);
    }
}

void StorageWflwIniSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageWflwIni] Stopped");
}

}  // namespace ase::storage
