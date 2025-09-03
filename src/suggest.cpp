#include <suggest.hpp>

#include <userver/utils/text.hpp>

namespace suggest {
 
std::string Handler::HandleRequest(server::http::HttpRequest& request, server::request::RequestContext&) const {
    request.GetHttpResponse().SetContentType(http::content_type::kTextPlain);
    const auto word = request.GetArg("word");
    const auto result = trie_.StartsWith(word);
    return userver::utils::text::Join(result, " ");
}

}  // namespace suggest
