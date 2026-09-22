/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_kycd_stat_pub_sys.cpp
 * @brief       StorageKycdStatPubSystem - announces the issue count on the star
 * @description Hub WRITE only. Die Zahl entsteht in StorageKycdReqDrnSystem, hier wird sie
 *              ausschliesslich weitergereicht.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Dissemination
 * @created     2026-09-21
 * @modified    2026-09-21
 * @version     00.00.00.00000 [seed]
 *
 * CAUSAL CHAIN (CAUSA_STG_KYCD_STAT: counted here, announced there)
 *
 *   StorageKycdReqDrnSystem (Ingestion)
 *          │
 *          │ StorageStaKycdStatComponent.issued += Karten dieses Takts
 *          ▼
 *   ┌─────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageKycdStatPubSystem                      │
 *   │                                                             │
 *   │  READS:                                                     │
 *   │    - StorageStaKycdStatComponent (auf StorageMgrTag)        │
 *   │                                                             │
 *   │  WRITES:                                                    │
 *   │    - "STG_KYCD_ISSUED_COUNT"_hs unter hub::GLOBAL           │
 *   └─────────────────────────────────────────────────────────────┘
 *          │
 *          │ Betriebssicht, Zaehlwerk
 *          ▼
 *   Wer die Ausstellungen beobachtet, liest EINE Zeile
 *
 * WARUM DER SCHNITT — GEMESSEN 2026-09-21. Der Ausstellungsschritt fuehrte den Zaehler bis dahin
 * selbst im Stern: lesen, eins addieren, zurueckschreiben, je Karte, INNERHALB seiner Schleife.
 * Das ist zweimal falsch. Es mischt Hub-I/O mit Rechnung in einem System und faellt damit unter
 * HUB_IO_MIXED_WITH_MATH; und ein Hub-Wert ist EIN stehender Platz, also bedeutete bei zehn
 * Karten in einem Takt nur der zehnte Schrieb etwas. Der Bestand gehoert dem Modul, der Stern
 * bekommt ihn angesagt.
 *
 * DIES IST KEINE ZWEITE PRAEGESCHIENE — die Abgrenzung steht im Kopf von
 * storage_kycd_tier_seed_sys.cpp und gilt hier genauso. Das System stellt keine Karte aus, nimmt
 * keinen Antrag entgegen und beruehrt keine Keycard-Entity; es liest eine Zahl und schreibt sie.
 * Auch der dortige Satz "ein Hub-Wert waere ein dritter Ort fuer dieselbe Aussage" trifft nicht
 * zu: er gilt dem TEILNEHMERBESTAND, der keine Laufzeitgroesse ist. Eine Ausstellungszahl ist
 * genau das.
 *
 * HUB Pattern (MIG_ASE_HUB_API v2.0):
 *
 * READS (from the star):
 *   (none)
 *
 * WRITES (to the star, under hub::GLOBAL):
 *   "STG_KYCD_ISSUED_COUNT"_hs → StorageStaKycdStatComponent.issued
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
 * [ ] NO switch/case statements? (use Tag-filtered Views (separate View per type)!)
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
#include <ase/storage/systems/keycard/storage_kycd_stat_pub_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_sta_kycd_stat_comp.hpp>
#include <ase/storage/components/tag/storage_mgr_tag.hpp>
// Hub for the O(1) API - die Zaehlzeile verlaesst das Modul hier und nur hier
#include <ase/hub/api.hpp>
// Logging
#include <ase/log/log.hpp>

#include <cstdint>

namespace ase::storage {
using namespace entt::literals;

/**
 * Anonymous namespace for helper FUNCTIONS (NOT static!)
 * IMPORTANT: Use anonymous namespace, NOT static keyword!
 *   OK: namespace { void helper() {...} }   // CORRECT
 *   NO: static void helper() {...}          // WRONG!
 * NO STRUCTS HERE! Structs = Data = Components!
 */
namespace {

/**
 * No helper functions needed for this system.
 * All logic is inline in tick().
 */

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageKycdStatPubSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdStatPubSystem] Started");
}

void StorageKycdStatPubSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * EINE SICHT, EIN SCHRIEB, KEINE RECHNUNG.
     *
     * Die Umwandlung nach float ist die FORM der Hub-Zeile, nicht eine Rechnung ueber den
     * Wert - der Stern fuehrt jede Groesse als float, und die Zahl geht unveraendert hinein.
     */
    for (auto [mgr, stat] : registry.view<StorageStaKycdStatComponent, StorageMgrTag>().each()) {
        (void)mgr;
        hub::set(registry, hub::GLOBAL, "STG_KYCD_ISSUED_COUNT"_hs,
                 static_cast<float>(stat.issued));
    }
}

void StorageKycdStatPubSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdStatPubSystem] Stopped");
}

}  // namespace ase::storage
