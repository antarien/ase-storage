#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_wflw_edge_ini_sys.hpp
 * @brief       StorageWflwEdgeIniSystem - Seeds the release-pipeline transition graph
 * @description Seeds the allowed workflow label transitions as StorageWflwEdgeComponent
 *              entities: draft → review → approved → released → retired. The graph is
 *              DATA - StorageWflwTranSystem validates every promote request against
 *              these entities, so a new allowed transition is one more seeded entity
 *              and never a line of system code.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Initialization
 * @created     2026-08-31
 * @modified    2026-08-31
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
 * @brief StorageWflwEdgeIniSystem - Seeds the release-pipeline transition graph
 *
 * Creates one StorageWflwEdgeComponent entity per ALLOWED label transition of the
 * edge-binary release pipeline. Each entity carries both labels verbatim and their
 * hashes; StorageWflwTranSystem matches a promote request by comparing the request's
 * current rule label hash against from_label_hash and the target against
 * to_label_hash. No transition exists that is not seeded here.
 *
 * Dedication: the TRANSITION GRAPH and nothing else. The realm that the pipeline
 * governs is seeded by StorageEdgeIniSystem, the ACL rules that carry the labels by
 * StorageAcssEdgeIniSystem - three separate objects, three separate systems.
 *
 * @schedule Initialization - Runs once at boot, independent of realm and ACL seeding
 * @reads    (nothing - the graph is seeded from the EDGE_LABEL_* chain in types.hpp)
 * @writes   StorageWflwEdgeComponent
 * @depends  (none - no ordering constraint against the other two edge seeders)
 */
class StorageWflwEdgeIniSystem : public ecs::System {
public:
    const char* name() const override { return "StorageWflwEdgeIniSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
