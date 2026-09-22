#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_kycd_ntfy_syn_sys.hpp
 * @brief       StorageKycdNtfySynSystem - spiegelt die Sternzeilen einer Ausstellungsanfrage
 * @description Die Hub-Haelfte des SYN-Schnitts an der Keycard-Anfrage. Dieses System LIEST den
 *              Stern und schreibt sechs Werte in zwei Bruecken-Zeilen auf derselben
 *              Anfrage-Entity; es rechnet nichts. Der Drain daneben rechnet und liest den Stern
 *              nicht mehr an.
 *
 *              WARUM DER SCHNITT. Gemessen 2026-09-21 meldete der Validator an
 *              storage_kycd_ntfy_drn_sys.cpp HUB_IO_MIXED_WITH_MATH: sechs `hub::get` und die
 *              Rekonstruktion der beiden Hashes standen in EINEM tick(). Die Regel nennt ihren
 *              Weg selbst — eine server-only Seite liest in eine `*_inp_*`-Zeile, die rechnende
 *              Seite nimmt sie von dort (WRFL_ASE_SYN_PATTERN).
 *
 *              DIE MELDUNG WANDERT MIT DEM LESEN. Die vier `log::error(HUB_NOT_FOUND)` gehoeren
 *              zu dem, der den Stern befragt — sie stehen jetzt hier. Fehlt eine Pflichtzeile,
 *              entsteht KEINE Bruecke, und der Drain sieht die Anfrage gar nicht erst: dasselbe
 *              Verhalten wie das frueherere `continue`, nur an der richtigen Stelle.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Ingestion
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
 * @brief StorageKycdNtfySynSystem - the star side of one keycard issuance request
 *
 * @schedule Ingestion - vor StorageKycdNtfyDrnSystem, das die Bruecke liest
 * @reads    Hub: SES_KYCD_NTF_USER_ID_HI/_LO, _REALM_ID_HI/_LO, _EXP_AT, _CLRN
 * @writes   StorageInpKycdNtfyIdnComponent + StorageInpKycdNtfyGrntComponent auf der Anfrage
 */
class StorageKycdNtfySynSystem : public ecs::System {
public:
    const char* name() const override { return "StorageKycdNtfySynSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
