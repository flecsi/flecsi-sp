#include "stream_util.hh"

namespace fsp {
namespace io {
namespace detail {

template class block_cursor<std::size_t>;
template class entity_cursor<std::size_t>;

}}}
