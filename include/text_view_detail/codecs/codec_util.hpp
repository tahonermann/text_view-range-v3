// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_CODECS_CODEC_UTIL_HPP // {
#define TEXT_VIEW_CODECS_CODEC_UTIL_HPP


#include <utility>
#include <range/v3/range_concepts.hpp>


namespace std {
namespace experimental {
inline namespace text {
namespace text_detail {


namespace concepts {

/*
 * No exception input iterator concept.  Used when defining exception
 * specifications that are conditional on certain input iterator/sentinel
 * operations being non-throwing.
 */
struct NoExceptInputIterator {
    template<typename I, typename S>
    auto requires_(I &i, S &s) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::model_of<ranges::concepts::InputIterator, I>(),
            ranges::concepts::model_of<ranges::concepts::Sentinel, S, I>(),
            ranges::concepts::is_true(
                std::integral_constant<bool,
                    noexcept(*i)
                >{}
            ),
            ranges::concepts::is_true(
                std::integral_constant<bool,
                    noexcept(++i)
                >{}
            ),
            ranges::concepts::is_true(
                std::integral_constant<bool,
                    noexcept(i++)
                >{}
            ),
            ranges::concepts::is_true(
                std::integral_constant<bool,
                    noexcept(i == s)
                >{}
            ),
            ranges::concepts::is_true(
                std::integral_constant<bool,
                    noexcept(i != s)
                >{}
            )
        )
    );
};

/*
 * No exception output iterator concept.  Used when defining exception
 * specifications that are conditional on certain output iterator
 * operations being non-throwing.
 */
struct NoExceptOutputIterator {
    template<typename I, typename V>
    auto requires_(I &i, V &&v) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::model_of<ranges::concepts::OutputIterator, I, V>(),
            ranges::concepts::is_true(
                std::integral_constant<bool,
                    noexcept(*i = std::forward<V>(v))
                >{}
            ),
            ranges::concepts::is_true(
                std::integral_constant<bool,
                    noexcept(*i++ = std::forward<V>(v))
                >{}
            ),
            ranges::concepts::is_true(
                std::integral_constant<bool,
                    noexcept(++i)
                >{}
            ),
            ranges::concepts::is_true(
                std::integral_constant<bool,
                    noexcept(i++)
                >{}
            )
        )
    );
};

} // namespace concepts

template<typename I, typename S>
using NoExceptInputIterator =
    ranges::concepts::models<concepts::NoExceptInputIterator, I, S>;

template<typename I, typename V>
using NoExceptOutputIterator =
    ranges::concepts::models<concepts::NoExceptOutputIterator, I, V>;


/*
 * Utility class to perform an increment operation at scope exit.  Used to
 * ensure the increment is performed even if an exception is thrown.
 */
template<typename T>
class delayed_increment {
public:
    delayed_increment(T &v, T &distance) noexcept
        : v(v), distance(distance) {}
    ~delayed_increment() {
        v += distance;
    }

private:
    T &v;
    T &distance;
};


} // namespace text_detail
} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_CODECS_CODEC_UTIL_HPP
