#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <userver/components/component_base.hpp>
#include <userver/components/component_context.hpp>

namespace src {

class TrieNode {
public:
    bool isEndOfWord;
    std::unordered_map<char, std::shared_ptr<TrieNode>> children;

    TrieNode();
};

class Trie  final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "trie";

    Trie(const userver::components::ComponentConfig& config,
            const userver::components::ComponentContext& context);

    void Insert(const std::string& word);
    std::vector<std::string> StartsWith(const std::string& prefix);
    void InsertFromFile(const std::string& filename);
    static userver::yaml_config::Schema GetStaticConfigSchema();

private:
    std::shared_ptr<TrieNode> root;

    void FreeNode(std::shared_ptr<TrieNode> node);
    void CollectAllWords(std::shared_ptr<TrieNode> node, std::string prefix, std::vector<std::string>& results);
};

} // namespace src