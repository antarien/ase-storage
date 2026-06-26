#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_edge_audt_drn_sys.hpp
 * @brief       StorageEdgeAudtDrnSystem - Drains edge A/ACS gate audit-signals
 * @description Converts the owner-keyed SES_EDGE_AUDIT_* Hub signals emitted by
 *              the L4 edge-distribution A/ACS gate into StorageBufAudtComponent
 *              + StorageAudtPendTag, feeding the StorageAudtWritSystem path.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Observation
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
 * @brief StorageEdgeAudtDrnSystem - Persists every edge A/ACS gate decision
 *
 * The L4 edge-distribution gate (ase-pl-edge-webserver acl_gate) cannot emplace
 * storage Components, so it records each decision as an owner-keyed Hub signal
 * (SES_EDGE_AUDIT_*). This system drains those signals at 1Hz into the storage
 * audit buffer, mirroring StorageAcssChkSystem's audit emission shape so the
 * existing StorageAudtWritSystem persists them (ARCH_ASE_STORAGE.md 14.1 step 10).
 *
 * @schedule Observation - 1Hz drain alongside StorageSrvlLogSystem
 * @reads    StorageStaIdnComponent + StorageKycdVldTag (authenticated sessions)
 *           SES_EDGE_AUDIT_SEQ / _ACTION / _RESULT / _CWRD (Hub, owner-keyed)
 *           SES_EDGE_AUDIT_DRAINED_SEQ (Hub, per-owner drain watermark)
 * @writes   StorageBufAudtComponent + StorageAudtPendTag (one per advanced owner)
 *           SES_EDGE_AUDIT_DRAINED_SEQ (advances the watermark)
 * @depends  StorageKycdLnkSystem links validated identity to session entity
 */
class StorageEdgeAudtDrnSystem : public ecs::System {
public:
    const char* name() const override { return "StorageEdgeAudtDrnSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
