#include <Pch.hpp>

#include "JsonMerge.hpp"

#include <set>

namespace Velyra::App {

    using json = nlohmann::json;

    // nullptr means the value does not exist in that version
    inline bool sameValue(const json* a, const json* b) {
        if (a == nullptr || b == nullptr) {
            return a == b;
        }
        return *a == *b;
    }

    inline std::optional<json> copyValue(const json* value) {
        return value != nullptr ? std::optional<json>(*value) : std::nullopt;
    }

    inline const json* findKey(const json* object, const std::string& key) {
        if (object == nullptr || !object->is_object()) {
            return nullptr;
        }
        const auto it = object->find(key);
        return it != object->end() ? &*it : nullptr;
    }

    /**
     * @return The merged value, std::nullopt if the value should not exist in the merged version
     */
    static std::optional<json> mergeValue(const json* base, const json* local, const json* remote,
                                          const std::string& path, std::vector<std::string>& conflicts) {
        if (sameValue(local, remote)) {
            return copyValue(local);
        }
        if (base != nullptr && sameValue(base, local)) {
            return copyValue(remote); // Only remote changed
        }
        if (base != nullptr && sameValue(base, remote)) {
            return copyValue(local); // Only local changed
        }
        if (base == nullptr && (local == nullptr || remote == nullptr)) {
            return copyValue(local != nullptr ? local : remote); // Without a base, values that only exist on one side are kept
        }
        if (local != nullptr && remote != nullptr && local->is_object() && remote->is_object()) {
            // Both sides changed something in this object, merge it key by key
            std::set<std::string> keys;
            for (const json* version : {base, local, remote}) {
                if (version != nullptr && version->is_object()) {
                    for (const auto& item : version->items()) {
                        keys.insert(item.key());
                    }
                }
            }
            json merged = json::object();
            for (const auto& key : keys) {
                auto value = mergeValue(findKey(base, key), findKey(local, key), findKey(remote, key), path + "/" + key, conflicts);
                if (value.has_value()) {
                    merged[key] = std::move(*value);
                }
            }
            return std::make_optional(std::move(merged));
        }
        // Both sides changed the same value, git is the source of truth
        conflicts.push_back(path.empty() ? "/" : path);
        return copyValue(remote);
    }

    json mergeJson(const json* base, const json& local, const json& remote, std::vector<std::string>& conflicts) {
        return mergeValue(base, &local, &remote, "", conflicts).value_or(json::object());
    }

}
