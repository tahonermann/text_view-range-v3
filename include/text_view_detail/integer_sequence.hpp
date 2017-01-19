// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

// This implementation of integer_sequence and friends adapted from Peter
// Dimov's implementation available at
// http://www.pdimov.com/cpp2/simple_cxx11_metaprogramming.html

#ifndef TEXT_VIEW_INTEGER_SEQUENCE_HPP // {
#define TEXT_VIEW_INTEGER_SEQUENCE_HPP


#include <cstddef>


namespace std {
namespace experimental {
inline namespace text {
namespace text_detail {

template<typename T, T ...Ints>
struct integer_sequence {
    using value_type = T;
    static constexpr std::size_t size() noexcept {
        return sizeof...(Ints);
    }
};

template<typename T>
struct next_integer_sequence;

template<typename T, T ...Ints>
struct next_integer_sequence<integer_sequence<T, Ints...>> {
    using type = integer_sequence<T, Ints..., sizeof...(Ints)>;
};

template<typename T, T I, T N>
struct make_int_seq_impl;

template<typename T, T I, T N>
struct make_int_seq_impl {
    using type = typename next_integer_sequence<
        typename make_int_seq_impl<T, I+1, N>::type>::type;
};

template<typename T, T N>
struct make_int_seq_impl<T, N, N> {
    using type = integer_sequence<T>;
};

template<typename T, T N>
using make_integer_sequence = typename make_int_seq_impl<T, 0, N>::type;

template<std::size_t ...Ints>
using index_sequence = integer_sequence<std::size_t, Ints...>;

template<std::size_t N>
using make_index_sequence = make_integer_sequence<std::size_t, N>;

} // namespace text_detail
} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_INTEGER_SEQUENCE_HPP
