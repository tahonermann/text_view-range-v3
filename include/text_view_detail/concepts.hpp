// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_CONCEPTS_HPP // {
#define TEXT_VIEW_CONCEPTS_HPP


#include <utility>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <text_view_detail/error_status.hpp>
#include <text_view_detail/traits.hpp>
#include <text_view_detail/character_set_id.hpp>
#include <text_view_detail/error_policy.hpp>


namespace std {
namespace experimental {
inline namespace text {

namespace text_detail {
template<typename T>
constexpr bool is_code_unit() {
    return std::is_integral<T>::value
        && (std::is_unsigned<T>::value
            || std::is_same<typename std::remove_cv<T>::type, char>::value
            || std::is_same<typename std::remove_cv<T>::type, wchar_t>::value);
}

template<typename T>
constexpr bool is_code_point() {
    return std::is_integral<T>::value
        && (std::is_unsigned<T>::value
            || std::is_same<typename std::remove_cv<T>::type, char>::value
            || std::is_same<typename std::remove_cv<T>::type, wchar_t>::value);
}
} // text_detail namespace


namespace concepts {

/*
 * Code unit concept
 */
struct CodeUnit {
    template<typename T>
    auto requires_(T &&) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::is_true(
                std::integral_constant<bool, text_detail::is_code_unit<T>()>{}
            )
        )
    );
};


/*
 * Code point concept
 */
struct CodePoint {
    template<typename T>
    auto requires_(T &&) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::is_true(
                std::integral_constant<bool, text_detail::is_code_point<T>()>{}
            )
        )
    );
};


/*
 * Character set concept
 */
struct CharacterSet {
    template<typename T>
    auto requires_(T &&) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::convertible_to<const char *>(T::get_name()),
            ranges::concepts::is_true(
                std::integral_constant<bool, noexcept(T::get_name())>{}
            ),
            ranges::concepts::convertible_to<code_point_type_t<T>>(
                T::get_substitution_code_point()),
            ranges::concepts::is_true(
                std::integral_constant<
                    bool, noexcept(T::get_substitution_code_point())>{}
            ),
            ranges::concepts::model_of<CodePoint, code_point_type_t<T>>()
        )
    );
};


/*
 * Character concept
 */
struct Character
    : ranges::concepts::refines<ranges::concepts::Regular>
{
    template<typename T>
    const T ct() noexcept;

    template<typename T>
    using cpt = code_point_type_t<character_set_type_t<T>>;

    template<typename T>
    cpt<T> cp() noexcept;

    template<typename T>
    auto requires_(T &&t) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::model_of<
                ranges::concepts::Constructible, T, cpt<T>
            >(),
            ranges::concepts::model_of<CharacterSet, character_set_type_t<T>>(),
            (t.set_code_point(cp<T>()), 0),
            ranges::concepts::is_true(
                std::integral_constant<bool,
                    noexcept(t.set_code_point(cp<T>()))
                >{}
            ),
            ranges::concepts::convertible_to<cpt<T>>(
                ct<T>().get_code_point()
            ),
            ranges::concepts::is_true(
                std::integral_constant<bool,
                    noexcept(ct<T>().get_code_point())
                >{}
            ),
            ranges::concepts::convertible_to<character_set_id>(
                ct<T>().get_character_set_id()
            )
        )
    );
};


/*
 * Code unit iterator concept
 */
struct CodeUnitIterator
    : ranges::concepts::refines<ranges::concepts::Iterator>
{
    template<typename T>
    auto requires_(T &&) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::model_of<CodeUnit, ranges::value_type_t<T>>()
        )
    );
};


/*
 * Code unit output iterator concept
 */
struct CodeUnitOutputIterator
    : ranges::concepts::refines<
          ranges::concepts::OutputIterator(
              ranges::concepts::_1,
              ranges::concepts::_2)>
{
    template<typename T, typename V>
    auto requires_(T &&, V &&) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::model_of<CodeUnit, V>()
        )
    );
};


/*
 * Text encoding state concept
 * These requirements are intended to match the char_traits<T>::state_type
 * requirements described in C++11 [char.traits.typedefs] 21.2.2p4.
 */
struct TextEncodingState
    : ranges::concepts::refines<ranges::concepts::SemiRegular>
{
};


/*
 * Text encoding state transition concept
 */
struct TextEncodingStateTransition
    : ranges::concepts::refines<ranges::concepts::SemiRegular>
{
};


/*
 * Text error policy concept
 */
struct TextErrorPolicy {
    template<typename T>
    auto requires_(T &&) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::model_of<
                ranges::concepts::SemiRegular, T
            >(),
            ranges::concepts::model_of<
                ranges::concepts::DerivedFrom, T, text_error_policy
            >(),
            ranges::concepts::is_false(
                std::is_same<typename std::remove_cv<T>::type,
                             text_error_policy>{}
            )
        )
    );
};


/*
 * Text encoding concept
 */
struct TextEncoding {
    template<typename T>
    auto requires_(T &&) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::model_of<
                TextEncodingState, typename T::state_type
            >(),
            ranges::concepts::model_of<
                TextEncodingStateTransition, typename T::state_transition_type
            >(),
            ranges::concepts::model_of<
                CodeUnit, code_unit_type_t<T>
            >(),
            ranges::concepts::model_of<
                Character, character_type_t<T>
            >(),
            ranges::concepts::convertible_to<int>(
                T::min_code_units
            ),
            ranges::concepts::convertible_to<int>(
                T::max_code_units
            ),
            ranges::concepts::convertible_to<const typename T::state_type&>(
                T::initial_state()
            ),
            ranges::concepts::is_true(
                std::integral_constant<bool,
                    noexcept(T::initial_state())
                >{}
            )
        )
    );
};


/*
 * Text encoder concept
 */
struct TextEncoder
    : ranges::concepts::refines<TextEncoding(ranges::concepts::_1)>
{
    template<typename T>
    typename T::state_type& s() noexcept;

    template<typename T>
    typename T::state_transition_type st() noexcept;

    template<typename T>
    character_type_t<T> c() noexcept;

    int& encoded_code_units() noexcept;

    template<typename T, typename CUIT>
    auto requires_(T &&, CUIT &&out) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::model_of<
                CodeUnitOutputIterator, CUIT, code_unit_type_t<T>
            >(),
            ranges::concepts::convertible_to<encode_status>(
                T::encode_state_transition(s<T>(), out, st<T>(), encoded_code_units())
            ),
            ranges::concepts::convertible_to<encode_status>(
                T::encode(s<T>(), out, c<T>(), encoded_code_units())
            )
        )
    );
};


/*
 * Text forward decoder concept
 */
struct TextForwardDecoder
    : ranges::concepts::refines<TextEncoding(ranges::concepts::_1)>
{
    template<typename T>
    typename T::state_type& s() noexcept;

    template<typename T>
    character_type_t<T>& c() noexcept;

    int& decoded_code_units() noexcept;

    template<typename T, typename CUIT>
    auto requires_(T &&, CUIT &&in) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::model_of<
                ranges::concepts::ForwardIterator, CUIT
            >(),
            ranges::concepts::model_of<
                ranges::concepts::ConvertibleTo,
                ranges::value_type_t<CUIT>,
                code_unit_type_t<T>
            >(),
            ranges::concepts::convertible_to<decode_status>(
                T::decode(s<T>(), in, in, c<T>(), decoded_code_units())
            )
        )
    );
};


/*
 * Text bidirectional decoder concept
 */
struct TextBidirectionalDecoder
    : ranges::concepts::refines<
          TextForwardDecoder(
              ranges::concepts::_1,
              ranges::concepts::_2)>
{
    template<typename T>
    typename T::state_type& s() noexcept;

    template<typename T>
    character_type_t<T>& c() noexcept;

    int& decoded_code_units() noexcept;

    template<typename T, typename CUIT>
    auto requires_(T &&, CUIT &&in) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::model_of<
                ranges::concepts::BidirectionalIterator, CUIT
            >(),
            ranges::concepts::convertible_to<decode_status>(
                T::rdecode(s<T>(), in, in, c<T>(), decoded_code_units())
            )
        )
    );
};


/*
 * Text random access decoder concept
 */
struct TextRandomAccessDecoder
    : ranges::concepts::refines<
          TextBidirectionalDecoder(
              ranges::concepts::_1,
              ranges::concepts::_2)>
{
    template<typename T, typename CUIT>
    auto requires_(T &&, CUIT &&) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::model_of<
                ranges::concepts::RandomAccessIterator, CUIT
            >(),
            ranges::concepts::is_true(
                std::integral_constant<
                    bool,
                    T::min_code_units == T::max_code_units>{}
            ),
            ranges::concepts::is_true(
                std::integral_constant<bool,
                    std::is_empty<typename T::state_type>::value
                >{}
            )
        )
    );
};


/*
 * Text iterator concept
 */
struct TextIterator
    : ranges::concepts::refines<ranges::concepts::Iterator>
{
    template<typename T>
    const T ct() noexcept;

    template<typename T>
    auto requires_(T &&) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::model_of<
                TextEncoding, encoding_type_t<T>
            >(),
            ranges::concepts::model_of<
                TextErrorPolicy, typename T::error_policy
            >(),
            ranges::concepts::model_of<
                TextEncodingState, typename T::state_type
            >(),
            ranges::concepts::convertible_to<
                const typename encoding_type_t<T>::state_type&
            >(
                ct<T>().state()
            ),
            ranges::concepts::is_true(
                std::integral_constant<bool,
                    noexcept(ct<T>().state())
                >{}
            ),
            ranges::concepts::convertible_to<
                bool
            >(
                ct<T>().error_occurred()
            ),
            ranges::concepts::is_true(
                std::integral_constant<bool,
                    noexcept(ct<T>().error_occurred())
                >{}
            )
        )
    );
};


/*
 * Text sentinel concept
 */
struct TextSentinel
    : ranges::concepts::refines<
          ranges::concepts::Sentinel(
              ranges::concepts::_1,
              ranges::concepts::_2)>
{
    template<typename T, typename I>
    auto requires_(T &&, I &&) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::model_of<
                TextIterator, I
            >(),
            ranges::concepts::model_of<
                TextErrorPolicy, typename T::error_policy
            >()
        )
    );
};


/*
 * Text output iterator concept
 */
struct TextOutputIterator
    : ranges::concepts::refines<
          TextIterator>
{
    template<typename T>
    const T ct() noexcept;

    template<typename T>
    auto requires_(T &&) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::model_of<
                ranges::concepts::OutputIterator,
                    T,
                    character_type_t<encoding_type_t<T>>
            >(),
            ranges::concepts::convertible_to<
                encode_status
            >(
                ct<T>().get_error()
            ),
            ranges::concepts::is_true(
                std::integral_constant<bool,
                    noexcept(ct<T>().get_error())
                >{}
            )
        )
    );
};


/*
 * Text input iterator concept
 */
struct TextInputIterator
    : ranges::concepts::refines<
          TextIterator,
          ranges::concepts::InputIterator>
{
    template<typename T>
    const T ct() noexcept;

    template<typename T>
    auto requires_(T &&) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::model_of<
                Character, ranges::value_type_t<T>
            >(),
            ranges::concepts::convertible_to<
                decode_status
            >(
                ct<T>().get_error()
            ),
            ranges::concepts::is_true(
                std::integral_constant<bool,
                    noexcept(ct<T>().get_error())
                >{}
            )
        )
    );
};


/*
 * Text forward iterator concept
 */
struct TextForwardIterator
    : ranges::concepts::refines<
          TextInputIterator,
          ranges::concepts::ForwardIterator>
{
};


/*
 * Text bidirectional iterator concept
 */
struct TextBidirectionalIterator
    : ranges::concepts::refines<
          TextForwardIterator,
          ranges::concepts::BidirectionalIterator>
{
};


/*
 * Text random access iterator concept
 */
struct TextRandomAccessIterator
    : ranges::concepts::refines<
          TextBidirectionalIterator,
          ranges::concepts::RandomAccessIterator>
{
};


/*
 * Text view concept
 */
struct TextView
    : ranges::concepts::refines<ranges::concepts::View>
{
    template<typename T>
    const T ct() noexcept;

    template<typename T>
    auto requires_(T &&t) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::model_of<
                TextIterator, ranges::iterator_t<T>
            >(),
            ranges::concepts::model_of<
                TextEncoding, encoding_type_t<T>
            >(),
            ranges::concepts::model_of<
                TextErrorPolicy, typename T::error_policy
            >(),
            ranges::concepts::model_of<
                ranges::concepts::View, typename T::view_type
            >(),
            ranges::concepts::model_of<
                TextEncodingState, typename T::state_type
            >(),
            ranges::concepts::model_of<
                CodeUnitIterator, typename T::code_unit_iterator
            >(),
            ranges::concepts::convertible_to<const typename T::view_type&>(
                ct<T>().base()
            ),
            ranges::concepts::is_true(
                std::integral_constant<bool,
                    noexcept(ct<T>().base())
                >{}
            ),
            ranges::concepts::convertible_to<const typename T::state_type&>(
                ct<T>().initial_state()
            ),
            ranges::concepts::is_true(
                std::integral_constant<bool,
                    noexcept(ct<T>().initial_state())
                >{}
            )
        )
    );
};


/*
 * Text input view concept
 */
struct TextInputView
    : ranges::concepts::refines<TextView>
{
    template<typename T>
    auto requires_(T &&t) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::model_of<
                TextInputIterator, ranges::iterator_t<T>
            >()
        )
    );
};


/*
 * Text forward view concept
 */
struct TextForwardView
    : ranges::concepts::refines<TextInputView>
{
    template<typename T>
    auto requires_(T &&t) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::model_of<
                TextForwardIterator, ranges::iterator_t<T>
            >()
        )
    );
};


/*
 * Text bidirectional view concept
 */
struct TextBidirectionalView
    : ranges::concepts::refines<TextForwardView>
{
    template<typename T>
    auto requires_(T &&t) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::model_of<
                TextBidirectionalIterator, ranges::iterator_t<T>
            >()
        )
    );
};


/*
 * Text random access view concept
 */
struct TextRandomAccessView
    : ranges::concepts::refines<TextBidirectionalView>
{
    template<typename T>
    auto requires_(T &&t) -> decltype(
        ranges::concepts::valid_expr(
            ranges::concepts::model_of<
                TextRandomAccessIterator, ranges::iterator_t<T>
            >()
        )
    );
};

} // namespace concepts


template<typename T>
using CodeUnit = ranges::concepts::models<concepts::CodeUnit, T>;

template<typename T>
using CodePoint = ranges::concepts::models<concepts::CodePoint, T>;

template<typename T>
using CharacterSet = ranges::concepts::models<concepts::CharacterSet, T>;

template<typename T>
using Character = ranges::concepts::models<concepts::Character, T>;

template<typename T>
using CodeUnitIterator = ranges::concepts::models<concepts::CodeUnitIterator, T>;

template<typename T, typename V>
using CodeUnitOutputIterator = ranges::concepts::models<concepts::CodeUnitOutputIterator, T, V>;

template<typename T>
using TextEncodingState = ranges::concepts::models<concepts::TextEncodingState, T>;

template<typename T>
using TextEncodingStateTransition = ranges::concepts::models<concepts::TextEncodingStateTransition, T>;

template<typename T>
using TextErrorPolicy = ranges::concepts::models<concepts::TextErrorPolicy, T>;

template<typename T>
using TextEncoding = ranges::concepts::models<concepts::TextEncoding, T>;

template<typename T, typename CUIT>
using TextEncoder = ranges::concepts::models<concepts::TextEncoder, T, CUIT>;

template<typename T, typename CUIT>
using TextForwardDecoder = ranges::concepts::models<concepts::TextForwardDecoder, T, CUIT>;

template<typename T, typename CUIT>
using TextBidirectionalDecoder = ranges::concepts::models<concepts::TextBidirectionalDecoder, T, CUIT>;

template<typename T, typename CUIT>
using TextRandomAccessDecoder = ranges::concepts::models<concepts::TextRandomAccessDecoder, T, CUIT>;

template<typename T>
using TextIterator = ranges::concepts::models<concepts::TextIterator, T>;

template<typename T, typename I>
using TextSentinel = ranges::concepts::models<concepts::TextSentinel, T, I>;

template<typename T>
using TextOutputIterator = ranges::concepts::models<concepts::TextOutputIterator, T>;

template<typename T>
using TextInputIterator = ranges::concepts::models<concepts::TextInputIterator, T>;

template<typename T>
using TextForwardIterator = ranges::concepts::models<concepts::TextForwardIterator, T>;

template<typename T>
using TextBidirectionalIterator = ranges::concepts::models<concepts::TextBidirectionalIterator, T>;

template<typename T>
using TextRandomAccessIterator = ranges::concepts::models<concepts::TextRandomAccessIterator, T>;

template<typename T>
using TextView = ranges::concepts::models<concepts::TextView, T>;

template<typename T>
using TextInputView = ranges::concepts::models<concepts::TextInputView, T>;

template<typename T>
using TextForwardView = ranges::concepts::models<concepts::TextForwardView, T>;

template<typename T>
using TextBidirectionalView = ranges::concepts::models<concepts::TextBidirectionalView, T>;

template<typename T>
using TextRandomAccessView = ranges::concepts::models<concepts::TextRandomAccessView, T>;


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_CONCEPTS_HPP
