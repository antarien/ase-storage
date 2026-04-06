#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_vote_prc_sys.hpp
 * @brief       StorageVotePrcSystem - Evaluates Vote of Confidence outcomes
 * @description Checks ballots, deadlines, and quorum for open votes
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Observation
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
 * @brief StorageVotePrcSystem - Vote evaluation at 1Hz
 *
 * @schedule Observation - Checks open votes for threshold or deadline
 * @reads    StorageVotePendTag + StorageStaVoteComponent + StorageVoteCntComponent
 * @writes   Removes StorageVotePendTag, issues keycard if accepted
 * @depends  HTTP routes create vote entities with StorageVotePendTag
 */
class StorageVotePrcSystem : public ecs::System {
public:
    const char* name() const override { return "StorageVotePrcSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
