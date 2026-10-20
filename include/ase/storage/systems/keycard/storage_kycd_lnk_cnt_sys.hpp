#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_kycd_lnk_cnt_sys.hpp
 * @brief       StorageKycdLnkCntSystem - zaehlt die Keycard-Bindungen dieses Takts
 * @description Zaehlt die Marken StorageKycdLnkCntTag, die StorageKycdLnkSystem auf die eben
 *              gebundenen Sitzungen gelegt hat, in StorageStaSessStatComponent.linked und nimmt
 *              die Marken wieder ab. Nur Rechnung, kein Hub: StorageKycdLnkSystem beschreibt den
 *              Stern und darf deshalb nicht rechnen (WRFL_ASE_SYN_PATTERN), und
 *              StorageKycdStatPubSystem sagt die Zahl an.
 *
 * @usecase     usecase:100265 - check access clearance
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
 * @parity      server_only
 * @created     2026-10-13
 * @modified    2026-10-13
 * @version     00.00.00.00000 [seed]
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
 * @brief StorageKycdLnkCntSystem - the link counter, raised from the markers of this tick
 *
 * @schedule Ingestion - nach StorageKycdLnkSystem
 * @reads    StorageKycdLnkCntTag, StorageMgrTag
 * @writes   StorageStaSessStatComponent.linked (Verwalterentity), entfernt StorageKycdLnkCntTag
 */
class StorageKycdLnkCntSystem : public ecs::System {
public:
    const char* name() const override { return "StorageKycdLnkCntSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
