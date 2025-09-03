#include <trie.hpp>

#include <userver/components/component_base.hpp>

#include <fstream>
#include <iostream>
#include <userver/components/component_config.hpp>
#include <userver/logging/log.hpp>
#include <userver/yaml_config/schema.hpp>
#include <userver/yaml_config/merge_schemas.hpp>

namespace src { 

TrieNode::TrieNode() : isEndOfWord(false) {}


Trie::Trie(const userver::components::ComponentConfig& config,
              const userver::components::ComponentContext& context) : ComponentBase(config, context) {
    root = std::make_shared<TrieNode>();
    LOG_INFO() << "load file";
    const auto filename = config["filename"].As<std::string>();
    InsertFromFile(filename);
    LOG_INFO() << "loaded file";
}


void Trie::Insert(const std::string& word) {
    std::shared_ptr<TrieNode> node = root;
    for (char ch : word) {
        if (node->children.find(ch) == node->children.end()) {
            node->children[ch] = std::make_shared<TrieNode>();
        }
        node = node->children[ch];
    }
    node->isEndOfWord = true;
}

std::vector<std::string> Trie::StartsWith(const std::string& prefix) {
    std::vector<std::string> results;
    std::shared_ptr<TrieNode> node = root;

    for (char ch : prefix) {
        if (node->children.find(ch) == node->children.end()) {
            return results;  // пустой список
        }
        node = node->children[ch];
    }

    CollectAllWords(node, prefix, results);
    return results;
}

void Trie::CollectAllWords(std::shared_ptr<TrieNode> node, std::string prefix, std::vector<std::string>& results) {
    if (node->isEndOfWord) {
        results.push_back(prefix);
    }

    for (auto& pair : node->children) {
        CollectAllWords(pair.second, prefix + pair.first, results);
    }
}

void Trie::InsertFromFile(const std::string& filename) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(infile, line)) {
        if (!line.empty()) {
            Insert(line);
        }
    }

    infile.close();
}

userver::yaml_config::Schema Trie::GetStaticConfigSchema() {
    return userver::yaml_config::MergeSchemas<userver::components::ComponentBase>(R"(
type: object
description: file info
additionalProperties: false
properties:
    filename:
        type: string
        description: filename
)");
}
}