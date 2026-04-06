#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_wflw_tran_sys.hpp
 * @brief       StorageWflwTranSystem - Asset workflow label transitions
 * @description Handles label changes: draft to review to approved to published
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
 * @brief StorageWflwTranSystem - Label transition engine
 *
 * @schedule Integration - run_after StorageFileWritSystem
 * @reads    StorageAcssRuleComponent (label field), StorageReqAcssComponent
 * @writes   Updates label field in StorageAcssRuleComponent
 * @depends  ACL grants with PERM_PROMOTE permission
 */
class StorageWflwTranSystem : public ecs::System {
public:
    const char* name() const override { return "StorageWflwTranSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
