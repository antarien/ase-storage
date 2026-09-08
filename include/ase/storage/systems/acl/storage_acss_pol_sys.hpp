#pragma once

/**
 * ASE ECS SYSTEM HEADER
 *
 * @file        storage_acss_pol_sys.hpp
 * @brief       StorageAcssPolSystem - die Politik-Tore der A/ACS-Leiter (Step 7-10)
 * @description Entscheidet ueber einen Zugriff, dessen SCHLUESSEL-Tore bereits gehalten haben:
 *              Label (zurueckgezogenes Gut, Entwurf/Pruefung nur fuer das Team), Kenntnisnahme
 *              (laufende Aufgabe begrenzt den Pfadbereich), Kontingent (Revier-Budget bei
 *              Schreibzugriff) — und spricht am Ende die Gewaehrung aus.
 *
 *              ABGETRENNT VON StorageAcssChkSystem (2026-08-31). Die Leiter stellte ZWEI Fragen in
 *              einem Rumpf: WER DARF (Ausweis, Revier, Gitter, Schutzstufe, Codewort, Recht) und
 *              WAS GILT FUER DIESEN GEGENSTAND (Label, laufende Aufgabe, Kontingent). Die erste
 *              wiegt den ANRUFER, die zweite den GEGENSTAND — zwei Fragen, zwei Fehlerbilder,
 *              zwei Dateien. Dieselbe Trennung, die am 2026-08-29 schon Aufloesung und Torleiter
 *              geschieden hat.
 *
 *              DER ANLASS WAR EIN VERSTECKTER HELFER: die Pruefspur-Zeile wurde bis dahin von
 *              `emit_audit(ecs::Registry&, ...)` gestellt, einer freien Funktion mit der Registry
 *              im Argument. Ausgeschrieben traegt sie jede Ausgangsstelle selbst — und der Rumpf
 *              fiel damit in das GOD_SYSTEM_UNSPLIT_BAND. Der Schnitt folgt der Dedizierung, nicht
 *              der Zeilenzahl; die Zahl war der Anlass, die Naht war schon vorher da.
 *
 *              DIE MARKE IST DER KANAL: StorageAcssPassTag. Was sie nicht traegt, hat entweder ein
 *              Schluessel-Tor nicht gehalten (dann traegt es StorageAcssDenyTag) oder die Leiter
 *              gar nicht erreicht.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    process
 * @schedule    Integration
 * @parity      server_only
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
 * @brief StorageAcssPolSystem - A/ACS policy gates, Step 7-10
 *
 * @schedule Integration - run_after StorageAcssChkSystem
 * @reads    StorageReqAcssComponent, StorageReqCredComponent, StorageStaAcssRslvComponent,
 *           StorageAcssPassTag, StorageAcssOwnrTag, StorageStaRelmComponent,
 *           StorageStaTaskComponent, StorageTaskIdnComponent
 * @writes   StorageAcssGrntTag or StorageAcssDenyTag + StorageBufAudtComponent
 * @depends  StorageAcssChkSystem stamps StorageAcssPassTag once the key gates hold
 *
 * WER NACH DIESEM SYSTEM LAEUFT, LIEF FRUEHER NACH StorageAcssChkSystem: die Gewaehrungsmarke
 * entsteht seit dem 2026-08-31 HIER, nicht mehr dort. StorageFileWritSystem, StorageCncmFltSystem
 * und StorageCurPrcSystem tragen ihre Kante deshalb auf dieses System. Eine Kante, die auf der
 * halben Leiter stehen bliebe, saehe NULL Gewaehrungen und wuerde still gar nichts tun.
 */
class StorageAcssPolSystem : public ecs::System {
public:
    const char* name() const override { return "StorageAcssPolSystem"; }
    void on_start(ecs::Registry& registry) override;
    void tick(ecs::Registry& registry, float dt) override;
    void on_stop(ecs::Registry& registry) override;
};

}  // namespace ase::storage
