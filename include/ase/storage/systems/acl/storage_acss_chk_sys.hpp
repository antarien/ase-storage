#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_acss_chk_sys.hpp
 * @brief       StorageAcssChkSystem - 10-step Mandatory Access Control enforcement
 * @description Evaluates access requests against keycards, clearance, codewords, ACL rules
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @created     2026-04-05
 * @modified    2026-04-05
 * @version     1.0.0
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
 * @brief StorageAcssChkSystem - A/ACS 10-step enforcement
 *
 * @schedule Integration - Processes access requests each frame
 * @reads    StorageReqAcssComponent, StorageStaKycdComponent, StorageKycdCwrdComponent,
 *           StorageStaRelmComponent, StorageAcssRuleComponent, StorageAcssCwrdComponent,
 *           StorageLatLnkComponent, StorageStaTaskComponent
 * @writes   StorageAcssGrantTag or StorageAcssDenyTag + StorageBufAudtComponent
 * @depends  HTTP routes create StorageReqAcssComponent entities
 */
class StorageAcssChkSystem : public ecs::System {
public:
    const char* name() const override { return "StorageAcssChkSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
