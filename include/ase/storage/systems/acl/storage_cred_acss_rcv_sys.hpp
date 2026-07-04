#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_cred_acss_rcv_sys.hpp
 * @brief       StorageCredAcssRcvSystem - Drives the A/ACS ladder for a Replica credential request
 * @description Drains the CACSS_WIRE_REQ frames the Replica sends before a customer-credential Vault
 *              op (LANE_CACSS). For each: resolves the session keycard by user_hash (issued_to hash),
 *              resolves the requested project realm by project_id, and emplaces a StorageReqAcssComponent
 *              (+ StorageCredAcssPndComponent correlation) so the single ACL authority
 *              StorageAcssChkSystem renders Grant/DenyTag. The verdict is shipped back by
 *              StorageCredAcssRspSystem. Fail-closed: no keycard → empty user_id (ladder denies).
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Reception
 * @created     2026-07-04
 * @modified    2026-07-04
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
 * @brief StorageCredAcssRcvSystem - inbound credential A/ACS access-check driver.
 *
 * Registers the CACSS_WIRE_REQ route onto LANE_CACSS, drains it, resolves keycard + project realm,
 * and emplaces StorageReqAcssComponent + StorageCredAcssPndComponent. The Integration ladder then
 * renders the verdict; StorageCredAcssRspSystem emits it back to the Replica.
 *
 * @schedule Reception - drains the inbound lane the L2 kernel demux fills
 * @reads    transport::InboundQueueResourceManager (ctx), StorageStaKycdComponent, StorageStaRelmComponent
 * @writes   StorageReqAcssComponent, StorageCredAcssPndComponent
 */
class StorageCredAcssRcvSystem : public ecs::System {
public:
    const char* name() const override { return "StorageCredAcssRcvSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
