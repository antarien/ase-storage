#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_acss_chk_sys.hpp
 * @brief       StorageAcssChkSystem - the key gates of the A/ACS ladder (steps 1-6)
 * @description Weighs WHO MAY for a request whose realm and ACL rule are resolved
 *              (StorageAcssRslvTag): credential, realm, lattice, protection level, codeword and
 *              permission. A request that holds every key gate carries StorageAcssPassTag into
 *              StorageAcssPolSystem, which weighs the subject and alone grants.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @design      DSGN_021
 * @usecase     usecase:100265 - check access clearance
 * @actor       Provider Credential Keeper (PROVCREKEE)
 * @created     2026-04-05
 * @modified    2026-11-16
 * @version     00.00.39.00039 [core]
 *
 * Copyright (c) Jan Ohlmann. All rights reserved.
 * Licensed to ANTARES SIMULACAO LTDA. Use is governed by LICENSE.md and PJV.md in this repository.
 * SPDX-License-Identifier: LicenseRef-ASE-LIC
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
 * @brief StorageAcssChkSystem - A/ACS key gates, steps 1 to 6
 *
 * @schedule Integration - after StorageAcssRuleSystem, before StorageAcssPolSystem
 * @reads    StorageReqAcssComponent, StorageReqCredComponent, StorageStaAcssRslvComponent,
 *           StorageStaKycdComponent, StorageKycdCwrdComponent, StorageStaRelmComponent,
 *           StorageAcssCwrdComponent, StorageLatLnkComponent
 * @writes   StorageAcssPassTag, or StorageAcssDenyTag + StorageBufAudtComponent
 * @depends  StorageAcssRuleSystem sets StorageAcssRslvTag on the resolved request
 */
class StorageAcssChkSystem : public ecs::System {
public:
    const char* name() const override { return "StorageAcssChkSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
