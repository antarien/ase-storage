#pragma once

/**
 * ASE RESOURCE MANAGER (NOT A COMPONENT!)
 *
 * @file        storage_acss_index_resource_manager.hpp
 * @brief       StorageAcssIndexResourceManager - O(1) lookup index for the A/ACS ladder
 * @description Holds the parent-to-children relations the access ladder needs:
 *              realms by id, ACL rules per realm, lattice links per target realm,
 *              required codewords per rule, need-to-know tasks per project, and the
 *              flattened set of codewords a user actually holds.
 *              Rebuilt once per tick by StorageAcssIdxSystem; read by the ladder.
 *              Accessed via registry.ctx().get<StorageAcssIndexResourceManager&>()
 *
 * WHY THIS EXISTS
 *   The A/ACS ladder answers six parent-to-children questions per access request.
 *   Each was a registry View walked inside the request loop, so the cost was
 *   requests x realms, requests x rules, requests x links, requests x tasks, and -
 *   nested three deep - requests x required-codewords x keycards x held-codewords
 *   (WS-K.2c M:N iteration). A Tag cannot carry these relations: every key is
 *   runtime DATA (a realm path, a user id, a codeword, an entity reference), and a
 *   Tag can only encode a type, never a value. Views stay the first choice wherever
 *   the question IS a type - StorageRelmPublicTag and StorageRelmConcealTag are
 *   answered by a Tag-filtered View plus contains(), not by this index.
 *
 * FLYWEIGHT PATTERN (from INST_ASE_ECS_SER):
 *   Components keep their uint32_t entity references. This manager keeps only the
 *   reverse direction - key to entity ids - so the systems never scan for them.
 *
 * USAGE:
 *   // In StorageAcssIdxSystem::on_start()
 *   static StorageAcssIndexResourceManager storage_acss_index;
 *   registry.ctx().emplace<StorageAcssIndexResourceManager&>(storage_acss_index);
 *
 *   // In StorageAcssIdxSystem::tick() - rebuild, single pass per relation
 *   idx.clear_all();
 *   idx.store_rule(rule.relm_ref, static_cast<uint32_t>(rule_ent));
 *
 *   // In StorageAcssChkSystem::tick() - read, O(1)
 *   const uint32_t n = idx.get_rule_count(req.relm_ref);
 *   for (uint32_t i = 0; i < n; ++i) { evaluate(idx.get_rule(req.relm_ref, i)); }
 *
 * THIS IS NOT A COMPONENT!
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    ecs/resource
 * @created     2026-08-16
 * @modified    2026-08-16
 * @version     1.0.0
 *
 * ECS RESOURCE MANAGER HEADER COMPLIANCE
 * [ ] NOT a Component - lives outside ECS registry
 * [ ] Accessed via registry.ctx().get<ResourceManager&>()
 * [ ] Components store ONLY uint32_t IDs
 * [ ] Header contains ONLY declarations (implementations in .cpp!)
 * [ ] Thread-safe method signatures (const where possible)
 * [ ] store_*(), get_*(), remove_*(), has_*() method pattern
 * [ ] clear_all() for shutdown cleanup
 * [ ] Private mutex for thread safety
 * [ ] Private maps for resource storage
 * [ ] NO inline implementations (prevents mass rebuilds!)
 */

#include <cstdint>
#include <mutex>
#include <ase/containers/hash_map.hpp>
#include <ase/containers/int_hash.hpp>
#include <ase/containers/vector.hpp>

namespace ase::storage {

/**
 * @brief Reverse index over the A/ACS relations (NOT ECS!)
 *
 * Every bucket is keyed by DATA, never by type. Integer keys go through
 * IntMixHash: libstdc++ hashes an integer to ITSELF, so a raw entity id or a
 * composite key would put every neighbouring value in a neighbouring bucket and
 * the map would degrade to a list exactly when it is full.
 */
class StorageAcssIndexResourceManager {
public:
    StorageAcssIndexResourceManager() = default;
    ~StorageAcssIndexResourceManager() = default;

    StorageAcssIndexResourceManager(const StorageAcssIndexResourceManager&) = delete;
    StorageAcssIndexResourceManager& operator=(const StorageAcssIndexResourceManager&) = delete;

    /**
     * @brief Register a realm under the hash of its id string.
     *
     * The ladder resolves parent-realm ownership by walking the path segments of the
     * target realm id upwards ("org/adg/projects/x" → "org/adg/projects" → "org/adg"),
     * which is bounded by path depth instead of by the number of realms.
     */
    void store_realm(uint64_t realm_id_hash, uint32_t realm_entity);

    /**
     * @brief O(1) realm entity for an id hash.
     * @return The stored entity id, or INVALID_ENTITY if no realm carries that id.
     */
    uint32_t get_realm(uint64_t realm_id_hash) const;

    /**
     * @brief True if a realm is registered under that id hash.
     */
    bool has_realm(uint64_t realm_id_hash) const;

    /**
     * @brief Append one ACL rule to its realm's bucket.
     *
     * Location rules and extension rules are filed apart because they are matched by
     * two different computations. Keeping them in one bucket would force the reader to
     * inspect each rule and branch - the discriminator-in-a-loop shape the ECS rules
     * replace with a Tag-filtered View. The Tag decides the bucket HERE, once per tick,
     * so neither reading loop ever asks what kind of rule it is holding.
     */
    void store_location_rule(uint32_t realm_entity, uint32_t rule_entity);

    /**
     * @brief Number of location rules registered for a realm (0 if none).
     */
    uint32_t get_location_rule_count(uint32_t realm_entity) const;

    /**
     * @brief Location rule entity at index within a realm's bucket.
     * @return The stored entity id, or INVALID_ENTITY if the index is out of range.
     */
    uint32_t get_location_rule(uint32_t realm_entity, uint32_t index) const;

    /**
     * @brief Append one extension rule to its realm's bucket.
     */
    void store_extension_rule(uint32_t realm_entity, uint32_t rule_entity);

    /**
     * @brief Number of extension rules registered for a realm (0 if none).
     */
    uint32_t get_extension_rule_count(uint32_t realm_entity) const;

    /**
     * @brief Extension rule entity at index within a realm's bucket.
     * @return The stored entity id, or INVALID_ENTITY if the index is out of range.
     */
    uint32_t get_extension_rule(uint32_t realm_entity, uint32_t index) const;

    /**
     * @brief Append one ACL rule to the realm's combined bucket (exact-pattern lookups).
     *
     * The transition system adopts a rule by its verbatim pattern, where the location
     * or extension distinction does not apply, so it reads one bucket holding both.
     */
    void store_rule(uint32_t realm_entity, uint32_t rule_entity);

    /**
     * @brief Number of ACL rules registered for a realm (0 if none).
     */
    uint32_t get_rule_count(uint32_t realm_entity) const;

    /**
     * @brief ACL rule entity at index within a realm's bucket.
     * @return The stored entity id, or INVALID_ENTITY if the index is out of range.
     */
    uint32_t get_rule(uint32_t realm_entity, uint32_t index) const;

    /**
     * @brief Append one lattice link to the bucket of the realm it targets.
     */
    void store_link(uint64_t realm_id_hash, uint32_t link_entity);

    /**
     * @brief Number of lattice links targeting a realm (0 if none).
     */
    uint32_t get_link_count(uint64_t realm_id_hash) const;

    /**
     * @brief Lattice link entity at index within a target realm's bucket.
     * @return The stored entity id, or INVALID_ENTITY if the index is out of range.
     */
    uint32_t get_link(uint64_t realm_id_hash, uint32_t index) const;

    /**
     * @brief Append one required-codeword entity to its ACL rule's bucket.
     */
    void store_required(uint32_t rule_entity, uint32_t codeword_entity);

    /**
     * @brief Number of codewords an ACL rule requires (0 if none).
     */
    uint32_t get_required_count(uint32_t rule_entity) const;

    /**
     * @brief Required-codeword entity at index within a rule's bucket.
     * @return The stored entity id, or INVALID_ENTITY if the index is out of range.
     */
    uint32_t get_required(uint32_t rule_entity, uint32_t index) const;

    /**
     * @brief Append one workflow-transition edge under the label it leads away from.
     *
     * The transition graph is seeded data (draft, review, approved, released, retired),
     * so "may this asset move to that label" is a lookup on the outgoing edges of the
     * asset's current label - not a walk over the whole graph per request.
     */
    void store_edge(uint64_t from_label_hash, uint32_t edge_entity);

    /**
     * @brief Number of transitions leading away from a label (0 if none).
     */
    uint32_t get_edge_count(uint64_t from_label_hash) const;

    /**
     * @brief Transition edge entity at index within a label's outgoing bucket.
     * @return The stored entity id, or INVALID_ENTITY if the index is out of range.
     */
    uint32_t get_edge(uint64_t from_label_hash, uint32_t index) const;

    /**
     * @brief Append one need-to-know task to its project's bucket.
     */
    void store_task(uint32_t project_reference, uint32_t task_entity);

    /**
     * @brief Number of need-to-know tasks in a project (0 if none).
     */
    uint32_t get_task_count(uint32_t project_reference) const;

    /**
     * @brief Need-to-know task entity at index within a project's bucket.
     * @return The stored entity id, or INVALID_ENTITY if the index is out of range.
     */
    uint32_t get_task(uint32_t project_reference, uint32_t index) const;

    /**
     * @brief Compose the membership key for "user U holds codeword C in realm scope R".
     *
     * Lives here, next to the set it addresses, because the writer and the reader are
     * different systems: two private copies of the composition would agree today and
     * drift apart the first time one of them is touched, and a drifted key does not
     * fail loudly - it silently answers "codeword not held" and denies every access.
     */
    static uint64_t compose_held_key(uint32_t user_hash, uint32_t realm_scope,
                                     uint32_t codeword_hash);

    /**
     * @brief Record that a user holds a codeword in a realm scope.
     *
     * The key is the composite of user id, realm scope and codeword. Flattening the
     * keycard and its held codewords into one set is what removes two of the three
     * nesting levels from the codeword gate: the ladder asks a membership question
     * instead of walking every keycard and every codeword it carries.
     */
    void store_held(uint64_t held_key);

    /**
     * @brief True if the composite key was recorded this tick.
     */
    bool has_held(uint64_t held_key) const;

    /**
     * @brief Compose the lookup key for one curation row: project plus curation key.
     */
    static uint64_t compose_curation_key(uint32_t project_reference, uint32_t key_hash);

    /**
     * @brief Register a curation row under its (project, key) pair.
     *
     * The curator CREATES rows while it processes requests, so a row minted for request
     * N must be visible to request N+1 in the SAME pass - otherwise two requests naming
     * the same asset each create their own row and the later rating silently lands on a
     * duplicate. The curator therefore registers a fresh row here the moment it exists,
     * not on the next tick's rebuild.
     */
    void store_curation(uint64_t curation_key, uint32_t curation_entity);

    /**
     * @brief O(1) curation row for a (project, key) pair.
     * @return The stored entity id, or INVALID_ENTITY if no row exists yet.
     */
    uint32_t get_curation(uint64_t curation_key) const;

    /**
     * @brief Record that a user currently holds an authenticated session.
     *
     * The codeword projection may only publish an axis for a user who HAS a session.
     * Walking sessions and searching their keycards is the M:N shape; walking keycards
     * and asking this set is the same decision without the product - but only because
     * the set is asked, never assumed. Dropping the question would publish clearance for
     * every keycard in the registry, which is a wider grant, not a faster loop.
     */
    void store_session(uint32_t user_hash);

    /**
     * @brief True if that user held an authenticated session when the index was built.
     */
    bool has_session(uint32_t user_hash) const;

    /**
     * @brief Register a network client entity under the NET_CLAI_ID it publishes.
     *
     * The keycard link binds an authenticated session to its client entity by that id.
     * Reading it back from the Hub for every client on every session was a walk inside
     * a walk; the id is DATA, so it is a key, not a Tag.
     */
    void store_client(uint32_t net_client_id, uint32_t client_entity);

    /**
     * @brief O(1) client entity for a NET_CLAI_ID.
     * @return The stored entity id, or INVALID_ENTITY if no client publishes that id.
     */
    uint32_t get_client(uint32_t net_client_id) const;

    /**
     * @brief Drop the relations the INGESTION-stage builder owns (realms, clients).
     *
     * Two builders, disjoint ownership, because one rebuild cannot serve both stages:
     * realms are created in Reception and read from Ingestion onwards, while keycards
     * are minted DURING Ingestion and must be visible to the Integration ladder in the
     * SAME frame. A single early build would hide a freshly minted keycard for a tick
     * (its owner's first access would be refused); a single late build leaves the
     * Ingestion readers with nothing. Each relation therefore has exactly ONE writer.
     */
    void clear_ingestion();

    /**
     * @brief Drop the relations the INTEGRATION-stage builder owns (everything else).
     */
    void clear_integration();

    /**
     * @brief Drop every bucket (tick rebuild boundary and shutdown cleanup).
     *
     * The index is derived state: it is discarded and rebuilt rather than maintained
     * incrementally, so a destroyed keycard or a retired rule can never leave a stale
     * grant behind.
     */
    void clear_all();

private:
    mutable std::mutex mutex_;

    // Realm entity keyed by the hash of its id string (1:1 - an id names one realm).
    ase::containers::HashMap<uint64_t, uint32_t, ase::containers::IntMixHash> realm_by_id_;

    // Parent-to-children buckets. The key is always the parent the child names in its
    // own foreign-key field, so a rebuild is one pass over the children.
    ase::containers::HashMap<uint32_t, ase::containers::Vector<uint32_t>,
                             ase::containers::IntMixHash> rules_by_realm_;
    ase::containers::HashMap<uint32_t, ase::containers::Vector<uint32_t>,
                             ase::containers::IntMixHash> location_rules_by_realm_;
    ase::containers::HashMap<uint32_t, ase::containers::Vector<uint32_t>,
                             ase::containers::IntMixHash> extension_rules_by_realm_;
    ase::containers::HashMap<uint64_t, ase::containers::Vector<uint32_t>,
                             ase::containers::IntMixHash> links_by_realm_;
    ase::containers::HashMap<uint32_t, ase::containers::Vector<uint32_t>,
                             ase::containers::IntMixHash> required_by_rule_;
    ase::containers::HashMap<uint32_t, ase::containers::Vector<uint32_t>,
                             ase::containers::IntMixHash> tasks_by_project_;
    ase::containers::HashMap<uint64_t, ase::containers::Vector<uint32_t>,
                             ase::containers::IntMixHash> edges_by_from_;

    // Flattened keycard x held-codeword membership: mix(user, realm scope, codeword).
    ase::containers::HashMap<uint64_t, uint32_t, ase::containers::IntMixHash> held_keys_;

    // Network client entity by the NET_CLAI_ID it publishes (1:1 - an id names one client).
    ase::containers::HashMap<uint32_t, uint32_t, ase::containers::IntMixHash> client_by_net_id_;

    // Users holding an authenticated session, by user hash (membership only).
    ase::containers::HashMap<uint32_t, uint32_t, ase::containers::IntMixHash> session_users_;

    // Curation row by mix(project_ref, key_hash) (1:1 - a pair names one row).
    ase::containers::HashMap<uint64_t, uint32_t, ase::containers::IntMixHash> curation_by_key_;
};

}  // namespace ase::storage
