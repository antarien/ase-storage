#include <ase/ecs/plugin_interface.hpp>
// Der Einstiegspunkt zieht den Umbrella, nicht den Modulheader allein: der Umbrella
// enthaelt ihn und stellt zusaetzlich jeden Header des Moduls vor einen Compiler.
// Ohne diese Kante wird das Sammelinclude von keiner Uebersetzungseinheit erreicht -
// es existiert, wird ausgeliefert und ist nie uebersetzt worden.
#include <ase/storage/storage.hpp>

namespace {

AseModuleInfo get_info() {
    return {"ase-storage", ase::storage::StorageModule::version(), ASE_API_VERSION, 3};
}

uint32_t on_load(AseLoadContext* ctx) {
    ase::storage::StorageModule instance;
    instance.build(*ctx->app);
    return ASE_LOAD_OK;
}

uint32_t on_unload(AseLoadContext*) { return ASE_LOAD_OK; }

void on_config_changed(AseLoadContext*, const char*, const char*) {}

void cleanup() {}

}  // anonymous namespace

ASE_MODULE_EXPORT(get_info, on_load, on_unload, on_config_changed, cleanup)
