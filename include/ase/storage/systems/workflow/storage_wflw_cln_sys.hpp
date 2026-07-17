#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_wflw_cln_sys.hpp
 * @brief       StorageWflwClnSystem - retired-build retention cleanup (90 days)
 * @description Tag-filtered sweep over StorageWflwRetrComponent records: once the
 *              WFLW_RETIRED_RETENTION_S window elapses, the asset and its four
 *              companion artifacts are deleted from the realm, the retiring ACL
 *              rule and the record entity are destroyed, and the deletion is
 *              audited. The cleanup, not the quota ceiling, keeps the realm small.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Observation
 * @created     2026-07-11
 * @modified    2026-07-11
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
 * @brief StorageWflwClnSystem - retired-build retention cleanup
 *
 * @schedule Observation - sibling of StorageQuotChkSystem
 * @reads    StorageWflwRetrComponent + StorageWflwRetrTag (Tag-filtered sweep),
 *           StorageStaRelmComponent (audit realm ref)
 * @writes   deletes asset + companion files via StorageResourceManager,
 *           destroys the retired ACL rule + record entity,
 *           StorageBufAudtComponent + StorageAudtPendTag (AUD_DELETE audit)
 * @depends  StorageResourceManager in ctx; StorageWflwTranSystem creates records
 */
class StorageWflwClnSystem : public ecs::System {
public:
    const char* name() const override { return "StorageWflwClnSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
