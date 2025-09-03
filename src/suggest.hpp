#pragma once
 
#include <userver/components/component_list.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
 
// Note: this is for the purposes of tests/samples only
#include <userver/utest/using_namespace_userver.hpp>
#include <trie.hpp>
 
namespace suggest {
 
class Handler final : public server::handlers::HttpHandlerBase {
public:
    // `kName` is used as the component name in static config
    static constexpr std::string_view kName = "handler-suggest";
 
    // Component is valid after construction and is able to accept requests
    Handler(const userver::components::ComponentConfig& config,
            const userver::components::ComponentContext& context)
      : HttpHandlerBase(config, context),
        trie_(context.FindComponent<src::Trie>()) {}

 
    std::string HandleRequest(server::http::HttpRequest& request, server::request::RequestContext&) const override;
private:
    src::Trie& trie_;
};
 
}  // namespace suggest
