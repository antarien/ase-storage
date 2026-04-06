#pragma once

/**
 * ASE RESOURCE MANAGER (NOT A COMPONENT!)
 *
 * @file        storage_resource_manager.hpp
 * @brief       StorageResourceManager - Flyweight Pattern for keycards, JWT, and filesystem
 * @description Manages developer JWT secrets, raw token strings, realm path
 *              resolution, and filesystem I/O operations outside ECS.
 *              Components store only uint32_t Flyweight IDs.
 *              Accessed via registry.ctx().get<StorageResourceManager&>()
 *
 * FLYWEIGHT PATTERN (from INST_ASE_ECS_SER):
 *   Component stores uint32_t token_id.
 *   This manager stores actual char* strings + filesystem paths.
 *   Systems access via registry.ctx().get<StorageResourceManager&>().
 *
 * USAGE:
 *   // In StgIniSystem::on_start()
 *   static StorageResourceManager storage_resources;
 *   registry.ctx().emplace<StorageResourceManager&>(storage_resources);
 *
 *   // In any System::tick()
 *   auto& stg = registry.ctx().get<StorageResourceManager&>();
 *   const char* token = stg.get_token(token_id);
 *   stg.write_asset(realm_id, proj_id, path, data, len);
 *
 * THIS IS NOT A COMPONENT!
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    ecs/resource
 * @created     2026-04-05
 * @modified    2026-04-05
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
#include <unordered_map>
#include <vector>

namespace ase::storage {

/**
 * @brief External resource manager for platform storage (NOT ECS!)
 *
 * Manages developer authentication (JWT), realm path resolution,
 * and filesystem I/O for the A/ACS platform storage system.
 *
 * Thread-safe: All methods are protected by mutex for callback safety.
 *
 * NOTE: Implementations are in storage_resource_manager.cpp to avoid
 * triggering mass rebuilds when methods change.
 */
class StorageResourceManager {
public:
    // =========================================================================
    // JWT SECRET MANAGEMENT (developer platform tokens)
    // =========================================================================

    void set_jwt_secret(const char* secret, uint16_t len);
    const char* get_jwt_secret() const;
    uint16_t get_jwt_secret_len() const;

    // =========================================================================
    // TOKEN STRING STORAGE — Flyweight (developer keycards)
    // =========================================================================

    uint32_t store_token(const char* token, uint16_t len);
    const char* get_token(uint32_t id) const;
    uint16_t get_token_len(uint32_t id) const;
    bool has_token(uint32_t id) const;
    void remove_token(uint32_t id);

    // =========================================================================
    // JWT VALIDATION (developer platform authentication)
    // =========================================================================

    struct ValidationResult {
        bool valid;               // true if JWT signature + format OK
        char user_id[64];         // from JWT "sub" claim
        uint64_t expires_at;      // from JWT "exp" claim
        bool is_access;           // true if type="access", false if type="refresh"
    };

    ValidationResult validate_jwt(uint32_t token_id) const;

    // =========================================================================
    // TOKEN BUFFER — thread-safe push from callbacks, drain from ECS system
    // =========================================================================

    void push_pending_token(uint32_t client_id, uint32_t token_id);
    uint32_t drain_pending_count();
    void drain_pending(uint32_t index, uint32_t& out_client_id, uint32_t& out_token_id);
    void drain_clear();

    // =========================================================================
    // WALL CLOCK — OOP bridge (<ctime> forbidden in ECS Systems)
    // =========================================================================

    uint64_t get_wall_time_seconds() const;

    // =========================================================================
    // REALM PATH RESOLUTION — maps realm/project/path to filesystem location
    // =========================================================================

    void set_data_dir(const char* path);
    const char* get_data_dir() const;

    /** Resolve realm/project/subpath to full filesystem path.
     *  Result written into out_path buffer (max 512 chars). */
    void resolve_path(const char* realm_id, const char* project_id,
                      const char* subpath, char* out_path, uint32_t out_len) const;

    // =========================================================================
    // FILESYSTEM I/O — OOP bridge (<filesystem>, fopen forbidden in ECS)
    // =========================================================================

    /** Ensure directory exists (mkdir -p equivalent). Returns true on success. */
    bool ensure_dir(const char* path);

    /** Write binary data to file. Returns true on success. */
    bool write_file(const char* path, const char* data, uint32_t len);

    /** Read file into buffer. Returns bytes read, 0 on failure. */
    uint32_t read_file(const char* path, char* out_buf, uint32_t buf_len) const;

    /** Delete file. Returns true on success. */
    bool delete_file(const char* path);

    /** Check if file exists. */
    bool file_exists(const char* path) const;

    /** Get file size in bytes. Returns 0 if not found. */
    uint64_t file_size(const char* path) const;

    // =========================================================================
    // REALM STORAGE METRICS
    // =========================================================================

    /** Calculate total bytes used by a realm (recursive directory scan). */
    uint64_t get_realm_usage(const char* realm_id) const;

    // =========================================================================
    // BULK CLEANUP
    // =========================================================================

    void clear_all();

private:
    mutable std::mutex mutex_;

    // JWT secret (fixed buffer, set once at initialization)
    char jwt_secret_buf_[256] = {};
    uint16_t jwt_secret_len_ = 0;

    // Token entry (fixed buffer per token)
    struct TokenEntry {
        char data[2048];
        uint16_t len;
    };

    // Token entries keyed by Flyweight ID
    std::unordered_map<uint32_t, TokenEntry> token_entries_;
    uint32_t next_token_id_ = 0;

    // Pending token buffer (pushed by callbacks, drained by StgKycdDrnSystem)
    struct PendingEntry {
        uint32_t client_id;
        uint32_t token_id;
    };
    std::vector<PendingEntry> pending_tokens_;

    // Data directory root (from ASE_DATA_DIR env or default)
    char data_dir_[512] = {};
};

}  // namespace ase::storage
