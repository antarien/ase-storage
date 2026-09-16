#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_kycd_tier_seed_sys.hpp
 * @brief       StorageKycdTierSeedSystem - Seeds the KCD_SERVICE keycards of the tier services at launch
 * @description Emplaces one keycard request per machine participant (StorageReqKycdComponent +
 *              StorageReqKycdRelmComponent + hub::HubStgKycdPendTag) so the existing
 *              StorageKycdReqDrnSystem mints them through the SAME pipeline every other
 *              keycard travels. No new minting path, no second channel: only a new applicant.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Configuration
 * @created     2026-09-14
 * @modified    2026-09-14
 * @version     1.0.0
 *
 * WARUM EIN START-SEED UND KEIN AUSSTELLBEFEHL
 *   Eine Maschine kann sich nicht mit einem Netz-Credential ausweisen, um die Karte zu holen,
 *   die sie braucht, um sich auszuweisen. Denselben Ring hat der Operator-Seed der dist-Tier
 *   schon einmal aufgeschnitten (EdgeOperSeedSystem): der Vertrauensanker ist die
 *   STARTKONTROLLE ueber den Tier, nicht ein vorgezeigtes Geheimnis. Wer den Prozess startet,
 *   bestimmt, wer er ist. Dieses System ist derselbe Schnitt fuer die Tier-Dienste.
 *
 * WARUM DIE KARTE KEINEN REALM NENNT
 *   StorageReqKycdRelmComponent.relm_ref bleibt 0. StorageCredAcssRcvSystem legt fuer eine
 *   Identitaet ohne expliziten Realm idempotent den PERSOENLICHEN an (id = name = owner =
 *   das Subjekt) und der Owner-Preset der Leiter gewaehrt ihrem Besitzer darin Zutritt.
 *   Ein vorab benannter Realm waere ein zweiter Ort, an dem dieselbe Zugehoerigkeit steht —
 *   und der erste, der veraltet.
 *
 * ECS SYSTEM HEADER COMPLIANCE
 *
 * [ ] STATELESS - No member variables
 * [ ] Views created on demand, not stored
 * [ ] NO direct calls to other systems
 * [ ] Communication only via Components
 * [ ] Helpers in anonymous namespace (in .cpp, NOT static functions!)
 * [ ] Math functions from ase-math (Layer 0)
 * [ ] NO file-level static/constexpr (constants → types.hpp)
 * [ ] Registered in Module with correct Schedule
 * [ ] Filename matches convention
 * [ ] Class name derived from filename
 * [ ] ALL THREE METHODS DECLARED: on_start, tick, on_stop
 */

#include <ase/ecs/system.hpp>

namespace ase::storage {

/**
 * @brief StorageKycdTierSeedSystem - Launch-rooted KCD_SERVICE keycards for the tier services
 *
 * @schedule Configuration - a lifecycle schedule, so the seed happens exactly once per process
 * @reads    nothing (the participant set is the one the Replica identity rows carry)
 * @writes   StorageReqKycdComponent + StorageReqKycdRelmComponent + hub::HubStgKycdPendTag
 * @depends  StorageKycdReqDrnSystem drains the requests into durable keycards (Ingestion)
 */
class StorageKycdTierSeedSystem : public ecs::System {
public:
    const char* name() const override { return "StorageKycdTierSeedSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
