#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_edge_ini_sys.hpp
 * @brief       StorageEdgeIniSystem - Edge-binary distribution realm seeding
 * @description Seeds the dedicated edge_binaries realm the one-line installer downloads the
 *              edge daemon from: directory tree, realm entity + Public/Active tags and the
 *              StorageRelmEdgeTag that gives the realm its identity. The ACL rules scoped to
 *              this realm belong to StorageAcssEdgeIniSystem, the release-pipeline transition
 *              graph to StorageWflwEdgeIniSystem, the license tier to StorageEdgeTierSystem.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Initialization
 * @design      DSGN_021
 * @usecase     usecase:100292 - install edge daemon
 * @actor       Edge Release Conductor (EDGERELCON)
 * @created     2026-06-24
 * @modified    2026-11-05
 * @version     1.1.0
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
 * Creates the edge_binaries realm directory tree (release/, keys/) and the realm
 * entity (StorageStaRelmComponent id=edge_binaries, default_protection=Public) with
 * StorageRelmGlobTag + StorageRelmActvTag (no conceal tag → concealment=false), its
 * quota and its id hash. The tier byte stays zero-initialised: the license tier is a hub
 * constant that exists only from Integration on, where StorageEdgeTierSystem writes it.
 *
 * StorageRelmEdgeTag is placed here and ONLY here: this system is the sole producer
 * of the edge realm, which is what makes the tag the SSOT for "which entity is
 * EDGE_REALM_ID". Every workflow system and the ACL seeder read it instead of
 * scanning all realms and comparing id strings.
 *
 * Dedication: the REALM and nothing else. The ACL rules scoped to it belong to
 * StorageAcssEdgeIniSystem, the release-pipeline transition graph to
 * StorageWflwEdgeIniSystem, the license tier to StorageEdgeTierSystem.
 *
 * @schedule Initialization - Runs once at boot, after StorageIniSystem
 * @reads    StorageResourceManager* from ctx() (data dir + path resolution)
 * @writes   StorageStaRelmComponent, StorageRelmIdnComponent,
 *           StorageRelmQuotComponent, StorageRelmGlobTag, StorageRelmActvTag,
 *           StorageRelmEdgeTag
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
