/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_kycd_lnk_cnt_sys.cpp
 * @brief       StorageKycdLnkCntSystem - zaehlt die Keycard-Bindungen dieses Takts
 * @description Nur Rechnung. Die Zahl entsteht hier aus den Marken des Bindungsschritts und wird
 *              von StorageKycdStatPubSystem angesagt.
 *
 * @usecase     usecase:100265 - check access clearance
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
 * @created     2026-10-13
 * @modified    2026-10-13
 * @version     00.00.00.00000 [seed]
 *
 * CAUSAL CHAIN (CAUSA_STG_KYCD_LNK_CNT: marked there, counted here, announced later)
 *
 *   StorageKycdLnkSystem (Ingestion)
 *          │
 *          │ StorageKycdLnkCntTag auf jeder eben gebundenen Sitzung
 *          ▼
 *   ┌─────────────────────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageKycdLnkCntSystem                       │
 *   │                                                             │
 *   │  READS:                                                     │
 *   │    - StorageKycdLnkCntTag (Bindungen dieses Takts)          │
 *   │    - StorageMgrTag (Verwalterentity)                        │
 *   │                                                             │
 *   │  WRITES:                                                    │
 *   │    - StorageStaSessStatComponent.linked                     │
 *   │    - entfernt StorageKycdLnkCntTag                          │
 *   └─────────────────────────────────────────────────────────────┘
 *          │
 *          │ Zaehlzeile auf der Verwalterentity
 *          ▼
 *   StorageKycdStatPubSystem (Dissemination) sagt STG_KYCD_LNK_COUNT an
 *
 * WARUM DER SCHNITT. Der Bindungsschritt beschreibt den Stern (SES_*), und ein System mit Hub-I/O
 * rechnet nicht (HUB_IO_MIXED_WITH_MATH). Er legt deshalb je Bindung eine Marke, und dieses
 * System macht daraus die Zahl. Ein Hub-Wert waere ausserdem EIN stehender Platz: zehn Bindungen
 * eines Takts hiessen zehn Schreibvorgaenge, von denen nur der letzte etwas bedeutet.
 *
 * HUB Pattern (MIG_ASE_HUB_API v2.0):
 *
 * READS (from the star):
 *   (none)
 *
 * WRITES (to the star):
 *   (none)
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
#include <ase/storage/systems/keycard/storage_kycd_lnk_cnt_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_sta_sess_stat_comp.hpp>
#include <ase/storage/components/tag/storage_kycd_lnk_cnt_tag.hpp>
#include <ase/storage/components/tag/storage_mgr_tag.hpp>
// Logging
#include <ase/log/log.hpp>

#include <entt/core/hashed_string.hpp>

#include <cstdint>

using namespace entt::literals;

namespace ase::storage {

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

void StorageKycdLnkCntSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdLnkCntSystem] Started");
}

void StorageKycdLnkCntSystem::tick(ecs::Registry& registry, float /*dt*/) {
    /**
     * DIE MARKEN SIND DIE BINDUNGEN DIESES TAKTS.
     *
     * Ein Takt ohne Bindung laeuft leer durch. Die Zaehlzeile entsteht beim ersten Zaehlen auf
     * der Verwalterentity; vorher gibt es nichts anzusagen.
     */
    const auto cnt_view = registry.view<StorageKycdLnkCntTag>();
    const auto linked_now = static_cast<uint32_t>(cnt_view.size());
    if (linked_now == 0u) {
        return;
    }
    for (auto mgr : registry.view<StorageMgrTag>()) {
        auto& stat = registry.get_or_emplace<StorageStaSessStatComponent>(mgr);
        stat.linked += linked_now;
    }
    registry.clear<StorageKycdLnkCntTag>();
}

void StorageKycdLnkCntSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageKycdLnkCntSystem] Stopped");
}

}  // namespace ase::storage
