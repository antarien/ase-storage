/**
 * ASE ECS SYSTEM IMPLEMENTATION
 *
 * @file        storage_quot_chk_sys.cpp
 * @brief       StorageQuotChkSystem - Quota monitoring per realm
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Observation
 * @created     2026-04-05
 * @modified    2026-06-24
 * @version     1.0.0
 *
 * CAUSAL CHAIN (Quota Check)
 *
 *   [Realm entities with tier data]
 *          │
 *          │ Observation schedule checks usage at 1Hz
 *          ▼
 *   ┌─────────────────────────────────────────────┐
 *   │  THIS SYSTEM: StorageQuotChkSystem          │
 *   │                                             │
 *   │  READS:                                     │
 *   │    - StorageStaRelmComponent (tier limits)  │
 *   │    - ResourceManager (realm usage bytes)    │
 *   │                                             │
 *   │  WRITES:                                    │
 *   │    - log::warn when usage exceeds 80%       │
 *   └─────────────────────────────────────────────┘
 *          │
 *          │ Quota warnings emitted for near-limit realms
 *          ▼
 *   Administrators notified of storage pressure
 *
 * HUB Pattern (N/A - No Hub reads/writes)
 *
 * READS (from Hub):
 *   (none)
 *
 * WRITES (to Hub):
 *   (none)
 *
 * FLYWEIGHT PATTERN (Active - StorageResourceManager via ctx)
 *   Realm usage calculation via ResourceManager filesystem scan.
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
#include <ase/storage/systems/quota/storage_quot_chk_sys.hpp>
// Components from same module
#include <ase/storage/components/state/storage_sta_relm_comp.hpp>
#include <ase/storage/components/state/storage_relm_quot_comp.hpp>
#include <ase/storage/components/tag/storage_relm_actv_tag.hpp>
#include <ase/storage/storage_resource_manager.hpp>
#include <ase/storage/types.hpp>
// Hub API (widget broadcast, change-based)
#include <ase/hub/api.hpp>
// Types (L0 — is_not_found sentinel check on hub::get reads)
#include <ase/types/types.hpp>

#include <entt/core/hashed_string.hpp>
// Logging
#include <ase/log/log.hpp>
// Strings (L0 — exakte uint64-Wiedergabe fuer die Quota-Zeile in tick())
#include <ase/utils/strops.hpp>

using namespace entt::literals;

namespace ase::storage {

// Anonymous namespace for helper FUNCTIONS (NOT static!)
namespace {

// Change-based publish of one Hub value: read-validate-compare-set, so the
// 1Hz Observation scan never floods the Hub broadcast with unchanged values.
void publish_changed(ecs::Registry& registry, uint32_t owner, uint32_t value_id, float value) {
    float current = hub::get(registry, owner, value_id, 0.0f);
    if (ase::types::is_not_found(current)) {
        current = -1.0f;  // unpublished — force the first publish
    }
    if (current == value) return;
    hub::set(registry, owner, value_id, value);
}

}  // anonymous namespace

// SYSTEM IMPLEMENTATION (ORDER: on_start → tick → on_stop)
// ALL THREE METHODS MUST BE IMPLEMENTED - NO EXCEPTIONS!

void StorageQuotChkSystem::on_start(ecs::Registry& /*registry*/) {
    log::debug("[StorageQuotChk] Started");
}

void StorageQuotChkSystem::tick(ecs::Registry& registry, float dt) {
    (void)dt;

    auto* mgr_ptr = registry.ctx().find<StorageResourceManager*>();
    if (!mgr_ptr || !(*mgr_ptr)) {
        return;  // manager not up yet (StorageIniSystem seeds it at Initialization)
    }
    auto& mgr = **mgr_ptr;

    const uint64_t now = mgr.get_wall_time_seconds();

    // The accounting row joins the view: a realm without it carries no ceiling, and
    // a realm without a ceiling was already skipped below - the two cases answer the
    // same way, so the join costs nothing and drops no realm that would be scanned.
    auto relm_view =
        registry.view<StorageStaRelmComponent, StorageRelmQuotComponent, StorageRelmActvTag>();
    for (auto [relm_ent, relm, quot] : relm_view.each()) {
        (void)relm_ent;
        if (quot.quota_bytes < 1u) continue;  // no ceiling configured for this realm

        // Pace the recursive FS scan: at most one scan per QUOTA_SCAN_INTERVAL_S
        // per realm (the pacing state is DATA on the realm, the system stays
        // stateless). First pass (usage_scanned_at == 0) scans immediately.
        if (quot.usage_scanned_at > 0u) {
            const uint64_t since_scan = now - quot.usage_scanned_at;
            if (since_scan < QUOTA_SCAN_INTERVAL_S) {
                continue;
            }
        }
        quot.usage_scanned_at = now;

        // The filesystem IS the byte authority for realm storage: the measured
        // scan result rehydrates the in-memory mirror (ground truth, NEVER a
        // display echo — the Hub values below are derived FROM this, not vice versa).
        const uint64_t used = mgr.get_realm_usage(relm.id);
        quot.used_bytes = used;

        // Widget broadcast over the Hub (NO engine HTTP endpoint): exact uint64
        // byte counts ride as two float-safe 24-bit words each — Hub values are
        // float32 and a single cast corrupts anything above 2^24; the client
        // widget reconstructs HI*2^24 + LO to the exact byte.
        const uint32_t owner = entt::hashed_string(relm.id).value();
        publish_changed(registry, owner, "STG_RELM_USED_HI"_hs, static_cast<float>(used >> 24));
        publish_changed(registry, owner, "STG_RELM_USED_LO"_hs, static_cast<float>(used & 0xFFFFFFu));
        publish_changed(registry, owner, "STG_RELM_QUOTA_HI"_hs, static_cast<float>(quot.quota_bytes >> 24));
        publish_changed(registry, owner, "STG_RELM_QUOTA_LO"_hs, static_cast<float>(quot.quota_bytes & 0xFFFFFFu));

        if (used > quot.quota_bytes) {
            // MIGRIERT. Hier stand ein Vermerk, dessen Praemisse falsch war: er behauptete,
            // ALLE wert-tragenden warn-Ueberladungen naehmen `float`, und schloss daraus, eine
            // exakte uint64-Byte-Zahl sei in der kategorisierten Form nicht darstellbar. Die
            // detail-Ueberladung nimmt einen STRING, und ase::utils::str_append_u64 gibt einen
            // uint64 exakt wieder — keine Mantissengrenze, keine Rundung. Es gab keine
            // Formgrenze, nur eine ungeprueft uebernommene Annahme ueber die vorhandenen Formen.
            //
            // In EINEM Punkt hatte der Vermerk recht, und der bleibt gefaehrlich: wer `used`
            // direkt in einen WERTPLATZ schreibt, bekommt weder Fehler noch Warnung — der
            // uint64 bindet still an die float-Ueberladung, und der Baum uebersetzt mit
            // -Wall -Wextra, nicht mit -Wconversion. Deshalb geht die Zahl hier ueber den
            // String und NICHT ueber den Wertplatz. Dieselbe Vorsicht, die zwanzig Zeilen
            // weiter oben die Hub-Werte in HI/LO teilt.
            //
            // VALUE_OUT_OF_RANGE: `used` ueberschreitet eine KONFIGURIERTE Obergrenze, und der
            // Hilfetext ueberlaesst die Reaktion dem Aufrufer — hier wird nichts durchgesetzt,
            // nur gemeldet. warn und nicht error, weil der Scan danach weiterlaeuft.
            char quota_detail[80] = {};
            ase::utils::str_copy(quota_detail, 80, "used_bytes=");
            ase::utils::str_append_u64(quota_detail, 80, used);
            ase::utils::str_append(quota_detail, 80, " ceiling_bytes=");
            ase::utils::str_append_u64(quota_detail, 80, quot.quota_bytes);
            log::warn(log::WRN::CAT::VALUE_OUT_OF_RANGE, "StorageQuotChk", relm.id, quota_detail);
        }
    }
}

void StorageQuotChkSystem::on_stop(ecs::Registry& /*registry*/) {
    log::debug("[StorageQuotChk] Stopped");
}

}  // namespace ase::storage
