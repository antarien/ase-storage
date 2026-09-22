#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_kycd_stat_pub_sys.hpp
 * @brief       StorageKycdStatPubSystem - sagt die Ausstellungszahl im Stern an
 * @description Liest StorageStaKycdStatComponent von der Verwalterentity und schreibt sie nach
 *              STG_KYCD_ISSUED_COUNT. Nur Hub-Schrieb, keine Rechnung - die Zahl entsteht
 *              woanders und wird hier ausschliesslich weitergereicht.
 *
 *              WARUM ES DIESES SYSTEM GIBT. Bis 2026-09-21 fuehrte StorageKycdReqDrnSystem den
 *              Zaehler selbst im Stern: lesen, eins addieren, zurueckschreiben, und das je Karte
 *              INNERHALB seiner Schleife. Das mischte Hub-I/O mit Rechnung in einem System
 *              (HUB_IO_MIXED_WITH_MATH) und schrieb mehrfach auf denselben stehenden Platz.
 *              Der Schnitt folgt WRFL_ASE_SYN_PATTERN: wer rechnet, rechnet auf Components;
 *              wer den Stern beschreibt, tut sonst nichts.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Dissemination
 * @created     2026-09-21
 * @modified    2026-09-21
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
 * @brief StorageKycdStatPubSystem - the issue counter, announced once per tick
 *
 * @schedule Dissemination - nach jeder Ausstellung dieses Takts
 * @reads    StorageStaKycdStatComponent + StorageMgrTag
 * @writes   Hub: STG_KYCD_ISSUED_COUNT
 */
class StorageKycdStatPubSystem : public ecs::System {
public:
    const char* name() const override { return "StorageKycdStatPubSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
