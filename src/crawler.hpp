#pragma once

#include <string>
#include <userver/clients/http/component.hpp>
#include <vector>
#include <unordered_set>

#include <userver/clients/http/client.hpp>
#include <userver/components/component_base.hpp>
#include <userver/components/component_context.hpp>


namespace src {

class Crawler final : public userver::components::ComponentBase {
public:
    static constexpr std::string_view kName = "crawler";

    Crawler(const userver::components::ComponentConfig& config,
            const userver::components::ComponentContext& context);

    // Загружает страницу по url и возвращает найденные ссылки
    void Crawl(const std::string& url, int depth, std::unordered_set<std::string>& visited);
    static userver::yaml_config::Schema GetStaticConfigSchema();

private:
    // Парсит html и возвращает ссылки
    std::vector<std::string> ExtractLinks(const std::string& html);

    userver::clients::http::Client& client_;
};
}