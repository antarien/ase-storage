#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_kycd_cwrd_pub_sys.hpp
 * @brief       StorageKycdCwrdPubSystem - Publishes session codewords to the Hub
 * @description Publishes each authenticated session's held keycard codewords to
 *              the Hub so the edge-distribution A/ACS gate can enforce them.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
 * @created     2026-06-24
 * @modified    2026-06-24
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
 * @brief StorageKycdCwrdPubSystem - Publishes session codewords to the Hub
 *
 * @schedule Ingestion - run_after StorageKycdLnkSystem
 * @reads    StorageStaIdnComponent + StorageKycdVldTag (authenticated session)
 *           StorageStaKycdComponent (keycard issued_to match)
 *           StorageKycdCwrdComponent (codewords by kycd_ref)
 * @writes   SES_KYCD_CWRD_COUNT (Hub, per-session owner)
 *           SES_KYCD_CWRD_<i> debug labels (Hub)
 * @depends  StorageKycdLnkSystem links validated identity to client entity
 */
class StorageKycdCwrdPubSystem : public ecs::System {
public:
    const char* name() const override { return "StorageKycdCwrdPubSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
