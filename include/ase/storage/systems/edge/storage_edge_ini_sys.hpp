#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_edge_ini_sys.hpp
 * @brief       StorageEdgeIniSystem - Edge-binary distribution realm seeding
 * @description Seeds the dedicated edge_binaries realm: directory tree, realm
 *              entity + Public/Active tags, and the release-path ACL rule plus
 *              its required BINARY codeword. Phase 12 A/ACS edge distribution.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Initialization
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
 * @brief StorageEdgeIniSystem - Seeds the edge_binaries distribution realm
 *
 * Creates the edge_binaries realm directory tree (release/, keys/), the realm
 * entity (StorageStaRelmComponent id=edge_binaries, tier=Enterprise,
 * default_protection=Public) with StorageRelmPublicTag + StorageRelmActiveTag
 * (no conceal tag → concealment=false), and the release-path ACL rule
 * (protection=Public, label=released) plus its required BINARY codeword entity.
 *
 * @schedule Initialization - Runs once at boot, after StorageIniSystem
 * @reads    StorageResourceManager* from ctx() (data dir + path resolution)
 * @writes   StorageStaRelmComponent, realm Tags, StorageAcssRuleComponent,
 *           StorageAcssCwrdComponent
 * @depends  StorageIniSystem (ResourceManager registered in ctx)
 */
class StorageEdgeIniSystem : public ecs::System {
public:
    const char* name() const override { return "StorageEdgeIniSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
