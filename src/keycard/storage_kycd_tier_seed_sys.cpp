/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_kycd_tier_seed_sys.cpp
 * @brief       StorageKycdTierSeedSystem - Seeds the KCD_SERVICE keycards of the tier services
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @design      DSGN_021
 * @category    process
 * @schedule    Configuration
 * @created     2026-09-14
 * @modified    2026-09-14
 * @version     1.0.0
 *
 * CAUSAL CHAIN (Tier Service-Keycard Seed)
 *
 *   [Prozessstart des Tiers - die Startkontrolle IST der Vertrauensanker]
 *          │
 *          │ Lifecycle-Schedule Configuration laeuft genau einmal je Prozess
 *          ▼
 *   ┌─────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageKycdTierSeedSystem                     │
 *   │                                                             │
 *   │  READS:                                                     │
 *   │    - StorageResourceManager (Wanduhr fuer authenticated_at) │
 *   │                                                             │
 *   │  WRITES:                                                    │
 *   │    - StorageReqKycdComponent (je Teilnehmer eine Zeile)     │
 *   │    - StorageReqKycdRelmComponent (relm_ref 0 = eigener)     │
 *   │    - hub::HubStgKycdPendTag (Entdeckungsmarke des Drains)   │
 *   └─────────────────────────────────────────────────────────────┘
 *          │
 *          │ Die Anfrage betritt die BESTEHENDE Keycard-Strecke unveraendert
 *          ▼
 *   StorageKycdReqDrnSystem praegt → StorageKycdVldSystem prueft → dauerhaft
 *
 * HUB Pattern (ARCH_ASE_HUB_API v2.0)
 *
 * READS (from Hub):
 *   keine - der Teilnehmerbestand ist keine Laufzeitgroesse. Er steht als vier
 *   ausgeschriebene Bloecke hier und als vier Identitaetszeilen auf der Replica-Seite;
 *   ein Hub-Wert dazwischen waere ein dritter Ort fuer dieselbe Aussage.
 *
 * WRITES (to Hub):
 *   hub::HubStgKycdPendTag - die Entdeckungsmarke, ueber die StorageKycdReqDrnSystem die
 *   Anfrage findet, ohne L3 zu importieren. Sie traegt keinen Wert; die Kartendaten stehen
 *   in den Komponenten daneben und reisen nie durch den Float-Kanal.
 *
 * DAS PROBLEM, DAS DIESES SYSTEM LOEST - und es ist ein Ring, kein Loch.
 *   Seit die Knoten-Token-Praegung durch die A/ACS-Leiter laeuft, ist der anfragende Dienst
 *   ein TEILNEHMER (DSGN_109, 83/PSHARE) und braucht eine Keycard. Eine Karte bekommt man,
 *   indem man sich ausweist. Ausweisen kann sich eine Maschine nur mit der Karte. Ohne
 *   Schnitt startet die Flotte nie wieder: StorageCredAcssRcvSystem findet keine Karte,
 *   laesst user_id leer, und die Leiter verweigert bei Schritt 1 - korrekt, fail-closed,
 *   und dauerhaft.
 *
 * WO DER SCHNITT LIEGT, UND WARUM DORT.
 *   Beim START. Wer den Prozess startet, bestimmt, wer er ist - genau der Anker, mit dem die
 *   dist-Tier ihren ersten Operator seedet (EdgeOperSeedSystem: "Trust root = launch control
 *   of the tier, not a network credential"). Ein Netz-Geheimnis waere hier gerade falsch: es
 *   koennte altern, waehrend der Prozess laeuft, und genau daran ist die Compute-Flotte schon
 *   einmal gestorben.
 *
 * WARUM HIER KEINE ZWEITE PRAEGESCHIENE ENTSTEHT.
 *   Dieses System praegt NICHTS. Es stellt einen ANTRAG in genau der Form, die
 *   StorageKycdReqDrnSystem seit jeher drainiert - dieselbe Form, die die Auth-Gate-Route und
 *   der Operator-Mint stellen. Der Weg von der Anfrage zur dauerhaften Karte bleibt Zeile fuer
 *   Zeile derselbe; neu ist allein, dass ihn auch ein Maschinen-Teilnehmer beschreiten kann.
 *
 * WARUM DIE KARTE KEINEN REALM NENNT.
 *   StorageReqKycdRelmComponent.relm_ref bleibt 0. StorageCredAcssRcvSystem legt fuer eine
 *   Identitaet ohne expliziten Realm idempotent den PERSOENLICHEN an (id = name = owner = das
 *   Subjekt), und der Owner-Preset der Leiter gewaehrt ihrem Besitzer darin Zutritt. Ein
 *   vorab benannter Realm waere ein zweiter Ort, an dem dieselbe Zugehoerigkeit steht - und
 *   der erste, der veraltet.
 *
 * WARUM VIER AUSGESCHRIEBENE BLOECKE UND KEINE TABELLE.
 *   Entity-per-Item: eine Tabelle waere ein Array, ein Durchlauf ueber sie eine
 *   Index-Dispatch. Die vier Bloecke sind der Bestand, den die Gegenseite fuehrt
 *   (ReplicaStaTierIdnComponent, vier Zeilen mit denselben vier Subjekten). Beide Mengen
 *   muessen uebereinstimmen: eine Identitaetszeile ohne Karte kommt bis zur Leiter und wird
 *   dort abgewiesen, eine Karte ohne Zeile wird nie vorgezeigt.
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
#include <ase/storage/systems/keycard/storage_kycd_tier_seed_sys.hpp>

// Die Antragsform, die StorageKycdReqDrnSystem drainiert
#include <ase/storage/components/request/storage_req_kycd_comp.hpp>
#include <ase/storage/components/request/storage_req_kycd_relm_comp.hpp>
// Wanduhr (Flyweight via ctx)
#include <ase/storage/storage_resource_manager.hpp>
// Kartenwerte (KYCD_TIER_*)
#include <ase/storage/types.hpp>

// Hub-Entdeckungsmarke: der Drain findet die Anfrage ueber sie, ohne L3 zu importieren
#include <ase/hub/api.hpp>

// str_copy - begrenzte Zeichenkettenkopie (Layer 0)
#include <ase/utils/strops.hpp>

#include <entt/core/hashed_string.hpp>

// Logging
#include <ase/log/log.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
namespace {

/* Kein Helfer moeglich und keiner noetig: der gemeinsame Teil der vier Bloecke legt Entities
 * an und schreibt Komponenten. Eine Funktion, die die Registry nimmt, ist kein Helfer,
 * sondern I/O hinter einem Aufruf - dieselbe Auflage, unter der storage_kycd_req_drn_sys.cpp
 * seine `remove_*`-Familie an ihre Aufrufstelle zurueckgeholt hat.
 * Only pure functions over primitive types belong here - no registry, no view, no query. */

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageKycdTierSeedSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdTierSeed] Started");
}

void StorageKycdTierSeedSystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    // Der Verwalter haelt die Wanduhr. Fehlt er, wird KEINE Karte beantragt: ein Antrag mit
    // authenticated_at 0 traegt eine Unwahrheit ueber seinen eigenen Zeitpunkt, und eine
    // Karte, die aus einer Unwahrheit entsteht, ist schlimmer als keine.
    //
    // `find` UND NICHT `get`: `get` setzt voraus, dass der Eintrag da ist, und ist andernfalls
    // undefiniert - eine Nullpruefung dahinter kaeme nie zum Zug. Die Schwesterstelle
    // storage_ini_sys.cpp:249 fragt aus demselben Grund mit `find` und prueft beide Ebenen.
    // Der Eintrag entsteht in Initialization (Schedule 0), diese Zeile laeuft in Configuration
    // (Schedule 1) - er SOLLTE stehen; genau deshalb ist sein Fehlen ein Befund und kein Zufall.
    auto* mgr_slot = registry.ctx().find<StorageResourceManager*>();
    if (mgr_slot == nullptr || *mgr_slot == nullptr) {
        log::error(log::ERR::CAT::RESOURCE_UNAVAIL, "StorageKycdTierSeedSystem", 0u,
                   "StorageResourceManager");
        return;
    }
    const uint64_t issued_at = (*mgr_slot)->get_wall_time_seconds();

    // ── svc:world ── Compute-Knoten der Flotte (CONN_ROLE_PUBLISHER auf der Replica-Seite)
    {
        auto ent = registry.create();
        auto& req = registry.emplace<StorageReqKycdComponent>(ent);
        ase::utils::str_copy(req.user_id, sizeof(req.user_id), "svc:world");
        req.user_id_hash = entt::hashed_string(req.user_id).value();
        req.authenticated_at = issued_at;
        req.expires_at = KYCD_TIER_EXP_AT;
        req.clearance = KYCD_TIER_CLEARANCE;
        auto& relm = registry.emplace<StorageReqKycdRelmComponent>(ent);
        relm.perm = KYCD_TIER_PERMS;
        registry.emplace<hub::HubStgKycdPendTag>(ent);
    }

    // ── svc:engine ── Kapazitaets-Orchestrator; er stellt den Knoten-Token-Antrag
    {
        auto ent = registry.create();
        auto& req = registry.emplace<StorageReqKycdComponent>(ent);
        ase::utils::str_copy(req.user_id, sizeof(req.user_id), "svc:engine");
        req.user_id_hash = entt::hashed_string(req.user_id).value();
        req.authenticated_at = issued_at;
        req.expires_at = KYCD_TIER_EXP_AT;
        req.clearance = KYCD_TIER_CLEARANCE;
        auto& relm = registry.emplace<StorageReqKycdRelmComponent>(ent);
        relm.perm = KYCD_TIER_PERMS;
        registry.emplace<hub::HubStgKycdPendTag>(ent);
    }

    // ── svc:reasoning ── Reasoning-Tier
    {
        auto ent = registry.create();
        auto& req = registry.emplace<StorageReqKycdComponent>(ent);
        ase::utils::str_copy(req.user_id, sizeof(req.user_id), "svc:reasoning");
        req.user_id_hash = entt::hashed_string(req.user_id).value();
        req.authenticated_at = issued_at;
        req.expires_at = KYCD_TIER_EXP_AT;
        req.clearance = KYCD_TIER_CLEARANCE;
        auto& relm = registry.emplace<StorageReqKycdRelmComponent>(ent);
        relm.perm = KYCD_TIER_PERMS;
        registry.emplace<hub::HubStgKycdPendTag>(ent);
    }

    // ── svc:dist ── Auslieferungsserver der Edge-Binaries
    {
        auto ent = registry.create();
        auto& req = registry.emplace<StorageReqKycdComponent>(ent);
        ase::utils::str_copy(req.user_id, sizeof(req.user_id), "svc:dist");
        req.user_id_hash = entt::hashed_string(req.user_id).value();
        req.authenticated_at = issued_at;
        req.expires_at = KYCD_TIER_EXP_AT;
        req.clearance = KYCD_TIER_CLEARANCE;
        auto& relm = registry.emplace<StorageReqKycdRelmComponent>(ent);
        relm.perm = KYCD_TIER_PERMS;
        registry.emplace<hub::HubStgKycdPendTag>(ent);
    }

    log::info("[StorageKycdTierSeed] service keycards requested for the tier participants "
              "(clearance={} perms={} issued_at={})",
              static_cast<uint32_t>(KYCD_TIER_CLEARANCE),
              static_cast<uint32_t>(KYCD_TIER_PERMS), issued_at);
}

void StorageKycdTierSeedSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdTierSeed] Stopped");
}

}  // namespace ase::storage
