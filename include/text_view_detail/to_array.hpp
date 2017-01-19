// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_TO_ARRAY_HPP // {
#define TEXT_VIEW_TO_ARRAY_HPP


#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <text_view_detail/integer_sequence.hpp>


namespace std {
namespace experimental {
inline namespace text {
namespace text_detail {

template<typename T, std::size_t N, std::size_t ...Is>
constexpr std::array<typename std::remove_cv<T>::type, N>
to_array(T (&a)[N], text_detail::index_sequence<Is...>) {
    return { { a[Is]... } };
}

template<typename T, std::size_t N>
constexpr std::array<typename std::remove_cv<T>::type, N>
to_array(T (&a)[N]) {
    return text_detail::to_array(a, text_detail::make_index_sequence<N>{});
}

} // namespace text_detail
} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_TO_ARRAY_HPP
