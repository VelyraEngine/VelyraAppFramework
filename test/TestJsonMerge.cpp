#include "TestPch.hpp"

#include "../src/VelyraAppFramework/Settings/JsonMerge.hpp"

using namespace Velyra;
using namespace Velyra::App;
using json = nlohmann::json;

class TestJsonMerge: public ::testing::Test {
protected:
    std::vector<std::string> conflicts;
};

TEST_F(TestJsonMerge, NothingChanged) {
    const json base = {{"a", 1}};
    EXPECT_EQ(mergeJson(&base, base, base, conflicts), base);
    EXPECT_TRUE(conflicts.empty());
}

TEST_F(TestJsonMerge, OnlyRemoteChanged) {
    const json base = {{"a", 1}, {"b", 1}};
    const json remote = {{"a", 2}, {"b", 1}};
    EXPECT_EQ(mergeJson(&base, base, remote, conflicts), remote);
    EXPECT_TRUE(conflicts.empty());
}

TEST_F(TestJsonMerge, OnlyLocalChanged) {
    const json base = {{"a", 1}, {"b", 1}};
    const json local = {{"a", 1}, {"b", 2}};
    EXPECT_EQ(mergeJson(&base, local, base, conflicts), local);
    EXPECT_TRUE(conflicts.empty());
}

TEST_F(TestJsonMerge, BothChangedDifferentKeys) {
    const json base = {{"a", 1}, {"b", 1}};
    const json local = {{"a", 1}, {"b", 2}, {"c", 3}};
    const json remote = {{"a", 5}, {"b", 1}, {"d", 4}};
    const json expected = {{"a", 5}, {"b", 2}, {"c", 3}, {"d", 4}};
    EXPECT_EQ(mergeJson(&base, local, remote, conflicts), expected);
    EXPECT_TRUE(conflicts.empty());
}

TEST_F(TestJsonMerge, BothChangedNestedObject) {
    const json base = {{"Example", {{"x", 1}, {"y", 1}}}};
    const json local = {{"Example", {{"x", 2}, {"y", 1}}}};
    const json remote = {{"Example", {{"x", 1}, {"y", 3}}}};
    const json expected = {{"Example", {{"x", 2}, {"y", 3}}}};
    EXPECT_EQ(mergeJson(&base, local, remote, conflicts), expected);
    EXPECT_TRUE(conflicts.empty());
}

TEST_F(TestJsonMerge, ConflictTakesRemote) {
    const json base = {{"Example", {{"x", 1}}}, {"b", 1}};
    const json local = {{"Example", {{"x", 2}}}, {"b", 2}};
    const json remote = {{"Example", {{"x", 3}}}, {"b", 1}};
    const json expected = {{"Example", {{"x", 3}}}, {"b", 2}};
    EXPECT_EQ(mergeJson(&base, local, remote, conflicts), expected);
    EXPECT_EQ(conflicts, std::vector<std::string>{"/Example/x"});
}

TEST_F(TestJsonMerge, DeletedOnOneSide) {
    const json base = {{"a", 1}, {"b", 1}};
    const json local = {{"a", 1}}; // b deleted locally
    const json remote = {{"a", 1}, {"b", 1}, {"c", 1}};
    const json expected = {{"a", 1}, {"c", 1}};
    EXPECT_EQ(mergeJson(&base, local, remote, conflicts), expected);
    EXPECT_TRUE(conflicts.empty());
}

TEST_F(TestJsonMerge, NoBaseRemoteWinsAndLocalOnlyKept) {
    // First sync on a new PC that already had some local settings
    const json local = {{"a", 1}, {"onlyLocal", 1}};
    const json remote = {{"a", 2}, {"onlyRemote", 2}};
    const json expected = {{"a", 2}, {"onlyLocal", 1}, {"onlyRemote", 2}};
    EXPECT_EQ(mergeJson(nullptr, local, remote, conflicts), expected);
    EXPECT_EQ(conflicts, std::vector<std::string>{"/a"});
}

TEST_F(TestJsonMerge, NoBaseEmptyLocal) {
    // First sync on a new PC
    const json remote = {{"a", 2}};
    EXPECT_EQ(mergeJson(nullptr, json::object(), remote, conflicts), remote);
    EXPECT_TRUE(conflicts.empty());
}
