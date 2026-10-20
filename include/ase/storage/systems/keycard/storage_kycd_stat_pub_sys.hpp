#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_kycd_stat_pub_sys.hpp
 * @brief       StorageKycdStatPubSystem - sagt die Keycard-Zaehler im Stern an
 * @description Liest die drei Zaehlzeilen StorageStaKycdStatComponent, StorageStaTknStatComponent
 *              und StorageStaSessStatComponent von der Verwalterentity und schreibt sie nach
 *              STG_KYCD_*_COUNT. Nur Hub-Schrieb, keine Rechnung - die Zahlen entstehen in den
 *              Systemen der Stufen und werden hier ausschliesslich weitergereicht.
 *
 *              WARUM ES DIESES SYSTEM GIBT. Eine Stufe, die ihren Zaehler selbst im Stern
 *              fuehrt (lesen, eins addieren, zurueckschreiben, je Ereignis, INNERHALB ihrer
 *              Schleife), mischt Hub-I/O mit Rechnung in einem System (HUB_IO_MIXED_WITH_MATH)
 *              und schreibt mehrfach auf denselben stehenden Platz. Der Schnitt folgt
 *              WRFL_ASE_SYN_PATTERN: wer rechnet, rechnet auf Components; wer den Stern
 *              beschreibt, tut sonst nichts.
 *
 * @usecase     usecase:100264 - issue customer keycard
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Dissemination
 * @created     2026-09-21
 * @modified    2026-10-13
 * @version     00.00.00.00000 [seed]
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
 * @brief StorageKycdStatPubSystem - the keycard stage counters, announced once per tick
 *
 * @schedule Dissemination - nach jeder Stufe dieses Takts
 * @reads    StorageStaKycdStatComponent, StorageStaTknStatComponent,
 *           StorageStaSessStatComponent + StorageMgrTag
 * @writes   Hub: STG_KYCD_ISSUED_COUNT, STG_KYCD_DRN_COUNT, STG_KYCD_VLD_ACCEPT_COUNT,
 *           STG_KYCD_VLD_REJECT_COUNT, STG_KYCD_LNK_COUNT, STG_KYCD_EXP_COUNT
 */
class StorageKycdStatPubSystem : public ecs::System {
public:
    const char* name() const override { return "StorageKycdStatPubSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
