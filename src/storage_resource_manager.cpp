/**
 * ASE RESOURCE MANAGER IMPLEMENTATION (OOP-ECS Bridge)
 *
 * @file        storage_resource_manager.cpp
 * @brief       StorageResourceManager - Flyweight storage + realm paths + filesystem I/O
 * @description Platform developer authentication + realm filesystem. Components store ONLY uint32_t IDs.
 *
 * @module      ase-storage
 * @layer       3 (Modules)
 * @category    ecs/resource
 * @created     2026-04-05
 * @modified    2026-04-05
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

#include <ase/storage/storage_resource_manager.hpp>
#include <ase/utils/strops.hpp>

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <ase/crypto/hmac.hpp>
#include <ase/crypto/compare.hpp>

namespace ase::storage {

namespace {

int b64url_decode_char(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '-') return 62;
    if (c == '_') return 63;
    return -1;
}

uint32_t b64url_decode(const char* in, uint16_t in_len, char* out, uint32_t out_max) {
    uint32_t olen = 0;
    uint32_t acc = 0;
    int bits = 0;
    for (uint16_t i = 0; i < in_len && olen < out_max; ++i) {
        int val = b64url_decode_char(in[i]);
        if (val < 0) continue;
        acc = (acc << 6) | static_cast<uint32_t>(val);
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out[olen++] = static_cast<char>((acc >> bits) & 0xFF);
        }
    }
    return olen;
}

uint16_t json_extract(const char* json, uint32_t jlen,
                       const char* key, uint16_t klen,
                       char* out, uint16_t omax) {
    for (uint32_t i = 0; i + klen + 4 < jlen; ++i) {
        if (json[i] != '"') continue;
        bool match = true;
        for (uint16_t k = 0; k < klen; ++k) {
            if (json[i + 1 + k] != key[k]) { match = false; break; }
        }
        if (!match || json[i + 1 + klen] != '"') continue;
        uint32_t vi = i + 1 + klen + 1;
        while (vi < jlen && (json[vi] == ':' || json[vi] == ' ')) ++vi;
        if (vi >= jlen) return 0;
        if (json[vi] == '"') {
            ++vi;
            uint16_t ol = 0;
            while (vi < jlen && json[vi] != '"' && ol < omax - 1) out[ol++] = json[vi++];
            out[ol] = '\0';
            return ol;
        }
        uint16_t ol = 0;
        while (vi < jlen && json[vi] != ',' && json[vi] != '}' && ol < omax - 1) out[ol++] = json[vi++];
        out[ol] = '\0';
        return ol;
    }
    return 0;
}

uint64_t parse_u64(const char* s, uint16_t len) {
    uint64_t r = 0;
    for (uint16_t i = 0; i < len; ++i) {
        if (s[i] >= '0' && s[i] <= '9') r = r * 10 + static_cast<uint64_t>(s[i] - '0');
    }
    return r;
}

void mkdir_recursive(const char* path) {
    char buf[512];
    ase::utils::str_copy(buf, 512, path);
    for (char* p = buf + 1; *p; ++p) {
        if (*p == '/') {
            *p = '\0';
            mkdir(buf, 0755);
            *p = '/';
        }
    }
    mkdir(buf, 0755);
}

uint64_t dir_size_recursive(const char* path) {
    uint64_t total = 0;
    DIR* d = opendir(path);
    if (!d) return 0;
    struct dirent* ent;
    while ((ent = readdir(d)) != nullptr) {
        if (ent->d_name[0] == '.') continue;
        char child[512];
        ase::utils::str_path(child, 512, path, ent->d_name, nullptr, nullptr);
        struct stat st;
        if (stat(child, &st) != 0) continue;
        if (S_ISDIR(st.st_mode)) {
            total += dir_size_recursive(child);
        } else {
            total += static_cast<uint64_t>(st.st_size);
        }
    }
    closedir(d);
    return total;
}

}  // anonymous namespace

// =============================================================================
// JWT SECRET MANAGEMENT
// =============================================================================

void StorageResourceManager::set_jwt_secret(const char* secret, uint16_t len) {
    std::lock_guard<std::mutex> lock(mutex_);
    uint16_t safe_len = (len < 255) ? len : 255;
    std::memcpy(jwt_secret_buf_, secret, safe_len);
    jwt_secret_buf_[safe_len] = '\0';
    jwt_secret_len_ = safe_len;
}

const char* StorageResourceManager::get_jwt_secret() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return (jwt_secret_len_ > 0) ? jwt_secret_buf_ : nullptr;
}

uint16_t StorageResourceManager::get_jwt_secret_len() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return jwt_secret_len_;
}

// =============================================================================
// TOKEN STRING STORAGE (Flyweight)
// =============================================================================

uint32_t StorageResourceManager::store_token(const char* token, uint16_t len) {
    std::lock_guard<std::mutex> lock(mutex_);
    uint32_t id = next_token_id_++;
    TokenEntry entry{};
    uint16_t safe_len = (len < 2047) ? len : 2047;
    std::memcpy(entry.data, token, safe_len);
    entry.data[safe_len] = '\0';
    entry.len = safe_len;
    token_entries_[id] = entry;
    return id;
}

const char* StorageResourceManager::get_token(uint32_t id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = token_entries_.find(id);
    return (it != token_entries_.end()) ? it->second.data : nullptr;
}

uint16_t StorageResourceManager::get_token_len(uint32_t id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = token_entries_.find(id);
    return (it != token_entries_.end()) ? it->second.len : 0;
}

bool StorageResourceManager::has_token(uint32_t id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return token_entries_.find(id) != token_entries_.end();
}

void StorageResourceManager::remove_token(uint32_t id) {
    std::lock_guard<std::mutex> lock(mutex_);
    token_entries_.erase(id);
}

// =============================================================================
// TOKEN BUFFER (thread-safe push/drain)
// =============================================================================

void StorageResourceManager::push_pending_token(uint32_t client_id, uint32_t token_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    pending_tokens_.push_back(PendingEntry{client_id, token_id});
}

uint32_t StorageResourceManager::drain_pending_count() {
    std::lock_guard<std::mutex> lock(mutex_);
    return static_cast<uint32_t>(pending_tokens_.size());
}

void StorageResourceManager::drain_pending(uint32_t index, uint32_t& out_client_id, uint32_t& out_token_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (index < pending_tokens_.size()) {
        out_client_id = pending_tokens_[index].client_id;
        out_token_id = pending_tokens_[index].token_id;
    }
}

void StorageResourceManager::drain_clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    pending_tokens_.clear();
}

// =============================================================================
// JWT VALIDATION
// =============================================================================

StorageResourceManager::ValidationResult StorageResourceManager::validate_jwt(uint32_t token_id) const {
    ValidationResult result{};
    result.valid = false;
    result.user_id[0] = '\0';
    result.expires_at = 0;
    result.is_access = false;

    std::lock_guard<std::mutex> lock(mutex_);

    auto it = token_entries_.find(token_id);
    if (it == token_entries_.end()) return result;
    const char* token = it->second.data;
    uint16_t token_len = it->second.len;

    if (jwt_secret_len_ < 1) return result;

    int dot1 = -1;
    int dot2 = -1;
    for (int i = 0; i < static_cast<int>(token_len); ++i) {
        if (token[i] == '.') {
            if (dot1 < 0) dot1 = i;
            else { dot2 = i; break; }
        }
    }
    if (dot1 < 0 || dot2 < 0) return result;

    std::uint8_t expected[ase::crypto::HMAC_SHA256_SIZE];
    if (!ase::crypto::hmac_sha256(jwt_secret_buf_, jwt_secret_len_,
                                   token, static_cast<size_t>(dot2),
                                   expected)) {
        return result;
    }

    const char* sig_b64 = token + dot2 + 1;
    uint16_t sig_b64_len = token_len - static_cast<uint16_t>(dot2) - 1;
    char actual[32];
    uint32_t actual_len = b64url_decode(sig_b64, sig_b64_len, actual, 32);

    if (actual_len != ase::crypto::HMAC_SHA256_SIZE) return result;
    if (!ase::crypto::constant_time_equal(actual, expected, ase::crypto::HMAC_SHA256_SIZE)) {
        return result;
    }

    const char* payload_b64 = token + dot1 + 1;
    uint16_t payload_b64_len = static_cast<uint16_t>(dot2 - dot1 - 1);
    char payload[1024];
    uint32_t payload_len = b64url_decode(payload_b64, payload_b64_len, payload, 1024);

    char sub_val[64] = {};
    char type_val[16] = {};
    char exp_val[32] = {};
    uint16_t sub_len = json_extract(payload, payload_len, "sub", 3, sub_val, 64);
    json_extract(payload, payload_len, "type", 4, type_val, 16);
    uint16_t exp_len = json_extract(payload, payload_len, "exp", 3, exp_val, 32);

    result.valid = true;
    std::memcpy(result.user_id, sub_val, 64);
    result.expires_at = parse_u64(exp_val, exp_len);
    result.is_access = (sub_len > 0 && type_val[0] == 'a');
    (void)sub_len;

    return result;
}

// =============================================================================
// WALL CLOCK
// =============================================================================

uint64_t StorageResourceManager::get_wall_time_seconds() const {
    return static_cast<uint64_t>(time(nullptr));
}

// =============================================================================
// REALM PATH RESOLUTION
// =============================================================================

void StorageResourceManager::set_data_dir(const char* path) {
    std::lock_guard<std::mutex> lock(mutex_);
    ase::utils::str_copy(data_dir_, 512, path);
}

const char* StorageResourceManager::get_data_dir() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return data_dir_;
}

void StorageResourceManager::resolve_path(const char* realm_id, const char* project_id,
                                           const char* subpath, char* out_path, uint32_t out_len) const {
    std::lock_guard<std::mutex> lock(mutex_);
    char realms_dir[512];
    ase::utils::str_path(realms_dir, 512, data_dir_, "realms", realm_id, nullptr);

    if (project_id && project_id[0] != '\0') {
        char proj_dir[512];
        ase::utils::str_path(proj_dir, 512, realms_dir, "projects", project_id, nullptr);
        ase::utils::str_path(out_path, out_len, proj_dir, subpath, nullptr, nullptr);
    } else {
        ase::utils::str_path(out_path, out_len, realms_dir, subpath, nullptr, nullptr);
    }
}

// =============================================================================
// FILESYSTEM I/O
// =============================================================================

bool StorageResourceManager::ensure_dir(const char* path) {
    mkdir_recursive(path);
    struct stat st;
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}

bool StorageResourceManager::write_file(const char* path, const char* data, uint32_t len) {
    char parent[512];
    ase::utils::str_copy(parent, 512, path);
    int32_t slash_pos = ase::utils::str_rfind(parent, 512, '/');
    if (slash_pos >= 0) {
        parent[slash_pos] = '\0';
        mkdir_recursive(parent);
    }

    FILE* f = fopen(path, "wb");
    if (!f) return false;
    fwrite(data, 1, len, f);
    fclose(f);
    return true;
}

uint32_t StorageResourceManager::read_file(const char* path, char* out_buf, uint32_t buf_len) const {
    FILE* f = fopen(path, "rb");
    if (!f) return 0;
    uint32_t bytes_read = static_cast<uint32_t>(fread(out_buf, 1, buf_len, f));
    fclose(f);
    return bytes_read;
}

bool StorageResourceManager::delete_file(const char* path) {
    return (unlink(path) == 0);
}

bool StorageResourceManager::file_exists(const char* path) const {
    struct stat st;
    return (stat(path, &st) == 0);
}

uint64_t StorageResourceManager::file_size(const char* path) const {
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    return static_cast<uint64_t>(st.st_size);
}

// =============================================================================
// REALM STORAGE METRICS
// =============================================================================

uint64_t StorageResourceManager::get_realm_usage(const char* realm_id) const {
    char realm_path[512];
    {
        std::lock_guard<std::mutex> lock(mutex_);
        ase::utils::str_path(realm_path, 512, data_dir_, "realms", realm_id, nullptr);
    }
    return dir_size_recursive(realm_path);
}

// =============================================================================
// BULK CLEANUP
// =============================================================================

void StorageResourceManager::clear_all() {
    std::lock_guard<std::mutex> lock(mutex_);
    jwt_secret_buf_[0] = '\0';
    jwt_secret_len_ = 0;
    pending_tokens_.clear();
    token_entries_.clear();
    next_token_id_ = 1;
    data_dir_[0] = '\0';
}

}  // namespace ase::storage
