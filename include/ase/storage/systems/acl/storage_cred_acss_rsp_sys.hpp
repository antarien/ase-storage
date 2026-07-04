#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_cred_acss_rsp_sys.hpp
 * @brief       StorageCredAcssRspSystem - Ships the A/ACS verdict back to the Replica
 * @description After StorageAcssChkSystem renders StorageAcssGrantTag/StorageAcssDenyTag on a
 *              credential access-check request (carrying StorageCredAcssPndComponent), this system
 *              emits the CACSS_WIRE_RES frame [87][req_id][verdict][reason] onto the transport
 *              outbound queue (the L2 demux ws->send()s it to the Replica) and destroys the request
 *              entity (deferred). The precise deny reason stays in the Engine audit; the wire carries
 *              a coarse, concealment-safe verdict so the Replica can conceal existence.
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
 * @brief StorageCredAcssRspSystem - outbound credential A/ACS verdict emitter.
 *
 * Reads request entities that carry StorageCredAcssPndComponent + a StorageAcssGrantTag/DenyTag,
 * pushes CACSS_WIRE_RES to the transport outbound queue, and destroys them (collect-then-destroy).
 *
 * @schedule Reception - after StorageCredAcssRcvSystem + the Integration ladder verdict
 * @reads    StorageReqAcssComponent, StorageCredAcssPndComponent, Grant/DenyTag; OutboundQueue (ctx)
 * @writes   transport outbound frames; destroys resolved request entities
 */
class StorageCredAcssRspSystem : public ecs::System {
public:
    const char* name() const override { return "StorageCredAcssRspSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
