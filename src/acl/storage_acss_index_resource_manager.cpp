/**
 * ASE RESOURCE MANAGER IMPLEMENTATION (OOP-ECS Bridge)
 *
 * @file        storage_acss_index_resource_manager.cpp
 * @brief       StorageAcssIndexResourceManager - O(1) lookup index for the A/ACS ladder
 * @description Every method takes the lock and touches exactly one bucket. Nothing in here
 *              iterates: a manager that walked its own contents would hand the caller back
 *              the very cost the index exists to remove.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    ecs/resource
 * @created     2026-08-16
 * @modified    2026-08-16
 * @version     1.0.0
 *
 * ECS RESOURCE MANAGER IMPLEMENTATION COMPLIANCE
 *
 * [ ] NOT a Component - lives outside ECS registry
 * [ ] Accessed via registry.ctx().get<ResourceManager&>()
 * [ ] Components store ONLY uint32_t IDs
 * [ ] Thread-safe via std::mutex
 * [ ] Proper cleanup in clear_all() - closes/stops BEFORE clearing
 * [ ] No ECS anti-patterns (this is intentionally OOP bridge code)
 * [ ] Implementations in .cpp to avoid mass rebuilds
 * [ ] Header contains ONLY declarations
 * [ ] Layer dependencies checked (only depend on lower layers)
 * [ ] NO file-level static/constexpr (constants in types.hpp)
 * [ ] Filename matches convention
 * [ ] 1 File = 1 ResourceManager
 * [ ] Folder structure matches convention (src/{category}/)
 * [ ] Layer dependencies respected (no upward dependencies)
 * [ ] NO std::shared_ptr in Components - stored HERE via Flyweight Pattern
 * [ ] External resources (shared_ptr, handles) accessed via registry.ctx()
 * [ ] ResourceManager registered in on_start() via registry.ctx().emplace<>()
 * [ ] clear_all() closes resources in REVERSE dependency order
 * [ ] clear_all() called from IniSystem::on_stop() or ShutdownSystem
 */

#include <ase/storage/storage_acss_index_resource_manager.hpp>
#include <ase/storage/types.hpp>

#include <entt/entt.hpp>

namespace ase::storage {

void StorageAcssIndexResourceManager::store_realm(uint64_t realm_id_hash, uint32_t realm_entity) {
    std::lock_guard<std::mutex> lock(mutex_);
    realm_by_id_[realm_id_hash] = realm_entity;
}

uint32_t StorageAcssIndexResourceManager::get_realm(uint64_t realm_id_hash) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = realm_by_id_.find(realm_id_hash);
    // INVALID_ENTITY, never 0: entity 0 is a perfectly valid realm, so a zero return
    // would read as "the first realm" at every call site.
    return it == realm_by_id_.end() ? INVALID_ENTITY : it->second;
}

bool StorageAcssIndexResourceManager::has_realm(uint64_t realm_id_hash) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return realm_by_id_.find(realm_id_hash) != realm_by_id_.end();
}

void StorageAcssIndexResourceManager::store_rule(uint32_t realm_entity, uint32_t rule_entity) {
    std::lock_guard<std::mutex> lock(mutex_);
    rules_by_realm_[realm_entity].push_back(rule_entity);
}

uint32_t StorageAcssIndexResourceManager::get_rule_count(uint32_t realm_entity) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = rules_by_realm_.find(realm_entity);
    return it == rules_by_realm_.end() ? 0u : static_cast<uint32_t>(it->second.size());
}

uint32_t StorageAcssIndexResourceManager::get_rule(uint32_t realm_entity, uint32_t index) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = rules_by_realm_.find(realm_entity);
    if (it == rules_by_realm_.end() || index >= it->second.size()) {
        return INVALID_ENTITY;
    }
    return it->second[index];
}

void StorageAcssIndexResourceManager::store_location_rule(uint32_t realm_entity,
                                                          uint32_t rule_entity) {
    std::lock_guard<std::mutex> lock(mutex_);
    location_rules_by_realm_[realm_entity].push_back(rule_entity);
}

uint32_t StorageAcssIndexResourceManager::get_location_rule_count(uint32_t realm_entity) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = location_rules_by_realm_.find(realm_entity);
    return it == location_rules_by_realm_.end() ? 0u : static_cast<uint32_t>(it->second.size());
}

uint32_t StorageAcssIndexResourceManager::get_location_rule(uint32_t realm_entity,
                                                            uint32_t index) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = location_rules_by_realm_.find(realm_entity);
    if (it == location_rules_by_realm_.end() || index >= it->second.size()) {
        return INVALID_ENTITY;
    }
    return it->second[index];
}

void StorageAcssIndexResourceManager::store_extension_rule(uint32_t realm_entity,
                                                           uint32_t rule_entity) {
    std::lock_guard<std::mutex> lock(mutex_);
    extension_rules_by_realm_[realm_entity].push_back(rule_entity);
}

uint32_t StorageAcssIndexResourceManager::get_extension_rule_count(uint32_t realm_entity) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = extension_rules_by_realm_.find(realm_entity);
    return it == extension_rules_by_realm_.end() ? 0u : static_cast<uint32_t>(it->second.size());
}

uint32_t StorageAcssIndexResourceManager::get_extension_rule(uint32_t realm_entity,
                                                             uint32_t index) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = extension_rules_by_realm_.find(realm_entity);
    if (it == extension_rules_by_realm_.end() || index >= it->second.size()) {
        return INVALID_ENTITY;
    }
    return it->second[index];
}

void StorageAcssIndexResourceManager::store_link(uint64_t realm_id_hash, uint32_t link_entity) {
    std::lock_guard<std::mutex> lock(mutex_);
    links_by_realm_[realm_id_hash].push_back(link_entity);
}

uint32_t StorageAcssIndexResourceManager::get_link_count(uint64_t realm_id_hash) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = links_by_realm_.find(realm_id_hash);
    return it == links_by_realm_.end() ? 0u : static_cast<uint32_t>(it->second.size());
}

uint32_t StorageAcssIndexResourceManager::get_link(uint64_t realm_id_hash, uint32_t index) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = links_by_realm_.find(realm_id_hash);
    if (it == links_by_realm_.end() || index >= it->second.size()) {
        return INVALID_ENTITY;
    }
    return it->second[index];
}

void StorageAcssIndexResourceManager::store_required(uint32_t rule_entity,
                                                     uint32_t codeword_entity) {
    std::lock_guard<std::mutex> lock(mutex_);
    required_by_rule_[rule_entity].push_back(codeword_entity);
}

uint32_t StorageAcssIndexResourceManager::get_required_count(uint32_t rule_entity) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = required_by_rule_.find(rule_entity);
    return it == required_by_rule_.end() ? 0u : static_cast<uint32_t>(it->second.size());
}

uint32_t StorageAcssIndexResourceManager::get_required(uint32_t rule_entity,
                                                       uint32_t index) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = required_by_rule_.find(rule_entity);
    if (it == required_by_rule_.end() || index >= it->second.size()) {
        return INVALID_ENTITY;
    }
    return it->second[index];
}

void StorageAcssIndexResourceManager::store_edge(uint64_t from_label_hash,
                                                 uint32_t edge_entity) {
    std::lock_guard<std::mutex> lock(mutex_);
    edges_by_from_[from_label_hash].push_back(edge_entity);
}

uint32_t StorageAcssIndexResourceManager::get_edge_count(uint64_t from_label_hash) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = edges_by_from_.find(from_label_hash);
    return it == edges_by_from_.end() ? 0u : static_cast<uint32_t>(it->second.size());
}

uint32_t StorageAcssIndexResourceManager::get_edge(uint64_t from_label_hash,
                                                   uint32_t index) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = edges_by_from_.find(from_label_hash);
    if (it == edges_by_from_.end() || index >= it->second.size()) {
        return INVALID_ENTITY;
    }
    return it->second[index];
}

void StorageAcssIndexResourceManager::store_task(uint32_t project_reference,
                                                 uint32_t task_entity) {
    std::lock_guard<std::mutex> lock(mutex_);
    tasks_by_project_[project_reference].push_back(task_entity);
}

uint32_t StorageAcssIndexResourceManager::get_task_count(uint32_t project_reference) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = tasks_by_project_.find(project_reference);
    return it == tasks_by_project_.end() ? 0u : static_cast<uint32_t>(it->second.size());
}

uint32_t StorageAcssIndexResourceManager::get_task(uint32_t project_reference,
                                                   uint32_t index) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = tasks_by_project_.find(project_reference);
    if (it == tasks_by_project_.end() || index >= it->second.size()) {
        return INVALID_ENTITY;
    }
    return it->second[index];
}

uint64_t StorageAcssIndexResourceManager::compose_held_key(uint32_t user_hash,
                                                          uint32_t realm_scope,
                                                          uint32_t codeword_hash) {
    // Takes hashes, never strings: the caller already holds them, and re-deriving them
    // here would put a string walk back on the hot path this index exists to clear.
    //
    // The splitmix64 finalizer is applied BETWEEN the fields, not once at the end: two
    // keycards differing only in realm scope must land in unrelated buckets, and a plain
    // XOR or shift would leave them adjacent - libstdc++ hashes an integer to itself, so
    // the bucket comes straight from the low bits.
    const ase::containers::IntMixHash mix;
    uint64_t key = static_cast<uint64_t>(user_hash);
    key = static_cast<uint64_t>(mix(key ^ static_cast<uint64_t>(realm_scope)));
    key = static_cast<uint64_t>(mix(key ^ static_cast<uint64_t>(codeword_hash)));
    return key;
}

void StorageAcssIndexResourceManager::store_held(uint64_t held_key) {
    std::lock_guard<std::mutex> lock(mutex_);
    held_keys_[held_key] = 1u;
}

bool StorageAcssIndexResourceManager::has_held(uint64_t held_key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return held_keys_.find(held_key) != held_keys_.end();
}

void StorageAcssIndexResourceManager::store_client(uint32_t net_client_id,
                                                   uint32_t client_entity) {
    std::lock_guard<std::mutex> lock(mutex_);
    client_by_net_id_[net_client_id] = client_entity;
}

uint32_t StorageAcssIndexResourceManager::get_client(uint32_t net_client_id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = client_by_net_id_.find(net_client_id);
    return it == client_by_net_id_.end() ? INVALID_ENTITY : it->second;
}

uint64_t StorageAcssIndexResourceManager::compose_curation_key(uint32_t project_reference,
                                                               uint32_t key_hash) {
    // Same discipline as the held-codeword key: the finalizer runs BETWEEN the fields, so
    // two rows of neighbouring projects do not land in neighbouring buckets.
    const ase::containers::IntMixHash mix;
    uint64_t key = static_cast<uint64_t>(project_reference);
    key = static_cast<uint64_t>(mix(key ^ static_cast<uint64_t>(key_hash)));
    return key;
}

void StorageAcssIndexResourceManager::store_curation(uint64_t curation_key,
                                                     uint32_t curation_entity) {
    std::lock_guard<std::mutex> lock(mutex_);
    curation_by_key_[curation_key] = curation_entity;
}

uint32_t StorageAcssIndexResourceManager::get_curation(uint64_t curation_key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = curation_by_key_.find(curation_key);
    return it == curation_by_key_.end() ? INVALID_ENTITY : it->second;
}

void StorageAcssIndexResourceManager::store_session(uint32_t user_hash) {
    std::lock_guard<std::mutex> lock(mutex_);
    session_users_[user_hash] = 1u;
}

bool StorageAcssIndexResourceManager::has_session(uint32_t user_hash) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return session_users_.find(user_hash) != session_users_.end();
}

void StorageAcssIndexResourceManager::clear_ingestion() {
    std::lock_guard<std::mutex> lock(mutex_);
    client_by_net_id_.clear();
    realm_by_id_.clear();
}

void StorageAcssIndexResourceManager::clear_integration() {
    std::lock_guard<std::mutex> lock(mutex_);
    // Reverse dependency order: the flattened membership set is derived from the keycard
    // relations, the buckets from the entities that name their parent.
    curation_by_key_.clear();
    session_users_.clear();
    held_keys_.clear();
    edges_by_from_.clear();
    tasks_by_project_.clear();
    required_by_rule_.clear();
    links_by_realm_.clear();
    extension_rules_by_realm_.clear();
    location_rules_by_realm_.clear();
    rules_by_realm_.clear();
}

void StorageAcssIndexResourceManager::clear_all() {
    clear_integration();
    clear_ingestion();
}

}  // namespace ase::storage
