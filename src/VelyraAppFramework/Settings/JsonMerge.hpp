#pragma once

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace Velyra::App {

    /**
     * @brief Three-way merge of two versions of a json document that were both derived from the same base version,
     *        the same way git merges files. Objects are merged per key (recursively), every other value is compared
     *        as a whole:
     *          - Only one side changed a value compared to the base: that change is taken
     *          - Both sides changed a value in a different way: conflict, the remote value is taken
     * @param base The version both sides started from, nullptr if unknown (e.g. the first sync on a new PC). Without
     *        a base every difference is a conflict, values that only exist on one side are kept.
     * @param local The local version
     * @param remote The remote version
     * @param conflicts Receives the paths of all conflicting values, e.g. "/ExampleSetting/value"
     * @return The merged document
     */
    nlohmann::json mergeJson(const nlohmann::json* base, const nlohmann::json& local, const nlohmann::json& remote,
                             std::vector<std::string>& conflicts);

}
