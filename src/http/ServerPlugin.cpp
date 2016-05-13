#include "native/http/ServerPlugin.hpp"

namespace native {
namespace http {

std::shared_ptr<ServerPlugin> ServerPlugin::getInstance() { return shared_from_this(); }

} /* namespace http */
} /* namespace native */
