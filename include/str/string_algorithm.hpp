#ifndef STR_STRING_ALGORITHM_HPP
#define STR_STRING_ALGORITHM_HPP

#include "my_types.hpp"
#include "option.hpp"

namespace my::str {

Option<usize> twoway_find(const u8* hay, const usize hlen, const u8* pat, const usize plen);

} // namespace my::str

#endif // STR_STRING_ALGORITHM_HPP
