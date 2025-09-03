#include "crawler.hpp"

#include <iostream>
#include <regex>
#include <unordered_set>
#include <fstream>

#include <userver/clients/http/component.hpp>
#include <userver/components/component_config.hpp>
#include <userver/logging/log.hpp>
#include <userver/yaml_config/schema.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include <userver/utils/async.hpp>

namespace src {
std::string UrlToFilename(const std::string& url) {
    std::string filename = url;
    for (auto& ch : filename) {
        if (!std::isalnum(ch)) {
            ch = '_';
        }
    }
    return filename + ".html";
}

void SavePageToFile(const std::string& url, const std::string& html) {
    std::string filename = UrlToFilename(url);
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        LOG_ERROR() << "Failed to open file for writing: " << filename;
        return;
    }
    file << html;
    LOG_INFO() << "Saved page to " << filename;
}


Crawler::Crawler(const userver::components::ComponentConfig& config,
              const userver::components::ComponentContext& context) : ComponentBase(config, context),
          client_(context.FindComponent<userver::components::HttpClient>().GetHttpClient()) {

    auto start_url = config["start-url"].As<std::string>();
    const auto fs_tp_name = config["fs-task-processor"].As<std::string>();
    auto& fs_task_processor = context.GetTaskProcessor(fs_tp_name);
    std::unordered_set<std::string> visited;
    userver::utils::Async(fs_task_processor, "crawler/fs-work", [&] {
        Crawl(start_url, 0, visited);
    }).Wait();
}


void Crawler::Crawl(const std::string& url, int depth, std::unordered_set<std::string>& visited) {
        const int max_depth = 1000;
        if (depth > max_depth) return;

        if (visited.find(url) != visited.end()) return;

        LOG_INFO() << std::string(depth * 2, ' ') << "Crawling " << url << " (depth " << depth << ")";

        visited.insert(url);

        auto request = client_.CreateRequest();
        request.url(url);
        auto response = request.perform();

        if (response->IsError()) {
            LOG_ERROR() << "Failed to GET " << url << ", status " << static_cast<int>(response->status_code());
            return;
        }

        const std::string html = response->body();
        auto links = ExtractLinks(html);

        for (const auto& link : links) {
            // Можно тут добавить фильтрацию или нормализацию ссылок
            Crawl(link, depth + 1, visited);
        }
}

std::vector<std::string> Crawler::ExtractLinks(const std::string& html) {
    std::vector<std::string> links;
    std::regex href_regex(R"(<a\s+(?:[^>]*?\s+)?href="([^"]*)\")", std::regex::icase);
    auto begin = std::sregex_iterator(html.begin(), html.end(), href_regex);
    auto end = std::sregex_iterator();

    for (auto i = begin; i != end; ++i) {
        links.push_back((*i)[1].str());;
    }
    return links;
}

userver::yaml_config::Schema Crawler::GetStaticConfigSchema() {
    return userver::yaml_config::MergeSchemas<userver::components::ComponentBase>(R"(
type: object
description: start url
additionalProperties: false
properties:
    start-url:
        type: string
        description: start url
    fs-task-processor:
        type: string
        description: fs-task-processor
)");
}
}