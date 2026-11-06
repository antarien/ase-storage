#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_kycd_rev_syn_sys.hpp
 * @brief       StorageKycdRevSynSystem - marks the keycard one revocation row names
 * @description The star side of the keycard revocation. This system walks the hub rows with the
 *              mark HUB_STG_KYCD_REV that ase-pl-webserver files for the operator route
 *              DELETE /api/realms/:realmId/keycards/:hash, and in ONE pass over the keycards that
 *              are not yet revoked it emplaces StorageKycdRevTag on the keycard whose kycd_hash
 *              equals the digest of the row (CAUSA_ASE_STORAGE, package 4, step 3). A digest that
 *              names no keycard is rejected with a warning.
 *
 *              THE MARK IS THE ANSWER. Every row read loses its mark in the same pass, matched or
 *              not; the writer takes every unmarked row back (WebserverKycdRevDrnSystem,
 *              Integration). The reader never destroys a row it did not file.
 *
 *              From the tag on, StorageKycdRevSystem and StorageKycdPstEmitSystem carry the
 *              revocation to the Replica on frame 35 (package 3).
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @parity      server_only
 * @schedule    Ingestion
 * @usecase     usecase:101096 - revoke edge token
 * @created     2026-11-05
 * @modified    2026-11-05
 * @version     00.00.00.00002 [seed]
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
 * @brief StorageKycdRevSynSystem - the star side of one keycard revocation request
 *
 * @schedule Ingestion
 * @reads    Hub: STG_CONST_KYCD_HASH_BYTES (GLOBAL), the rows with the mark HUB_STG_KYCD_REV,
 *           field HUB_STG_KYCD_REV_REQ_HASH; StorageStaKycdComponent
 * @writes   StorageKycdRevTag on the matched keycard; takes the mark HUB_STG_KYCD_REV off
 */
class StorageKycdRevSynSystem : public ecs::System {
public:
    const char* name() const override { return "StorageKycdRevSynSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
