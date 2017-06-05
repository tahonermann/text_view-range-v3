// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_TEXT_VIEW_HPP // {
#define TEXT_VIEW_TEXT_VIEW_HPP


#include <iterator>
#include <string>
#include <utility>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/iterator_concepts.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <text_view_detail/adl_customization.hpp>
#include <text_view_detail/advance_to.hpp>
#include <text_view_detail/basic_view.hpp>
#include <text_view_detail/concepts.hpp>
#include <text_view_detail/default_encoding.hpp>
#include <text_view_detail/error_policy.hpp>
#include <text_view_detail/itext_iterator.hpp>
#include <text_view_detail/itext_sentinel.hpp>
#include <text_view_detail/subobject.hpp>


namespace std {
namespace experimental {
inline namespace text {


template<typename ET, typename VT, typename TEP = text_default_error_policy,
CONCEPT_REQUIRES_(
    TextEncoding<ET>(),
    ranges::View<VT>(),
    TextErrorPolicy<TEP>())>
class basic_text_view
    : private text_detail::subobject<typename ET::state_type>,
      public ranges::view_base
{
    using base_type = text_detail::subobject<typename ET::state_type>;

public:
    using encoding_type = ET;
    using view_type = VT;
    using error_policy = TEP;
    using state_type = typename ET::state_type;
    using code_unit_iterator =
        ranges::iterator_t<
            typename std::add_const<view_type>::type>;
    using code_unit_sentinel =
        ranges::sentinel_t<
            typename std::add_const<view_type>::type>;
    using iterator = itext_iterator<ET, VT, TEP>;
    using sentinel = itext_sentinel<ET, VT, TEP>;

    // The default constructor produces a text view with a singular range.  An
    // object produced with this constructor may only be assigned to or
    // destroyed.
    basic_text_view() = default;

    // Overload to initialize a text view from an object of the underlying code
    // unit view type and an explicitly specified initial encoding state.
    basic_text_view(
        state_type state,
        view_type view)
    :
        base_type{std::move(state)},
        view{std::move(view)}
    {}

    // Overload to initialize a text view from an object of the underlying code
    // unit view type and an implicit initial encoding state.
    basic_text_view(
        view_type view)
    :
        base_type{encoding_type::initial_state()},
        view{std::move(view)}
    {}

    // Overload to initialize a text view from an InputIterator and Sentinel
    // pair, and an explicitly specified initial encoding state.  This overload
    // requires that view_type be constructible from a code_unit_iterator pair.
    CONCEPT_REQUIRES(
        ranges::Constructible<
            view_type,
            code_unit_iterator&&,
            code_unit_sentinel&&>())
    basic_text_view(
        state_type state,
        code_unit_iterator first,
        code_unit_sentinel last)
    :
        base_type{std::move(state)},
        view{std::move(first), std::move(last)}
    {}

    // Overload to initialize a text view from an InputIterator and Sentinel
    // pair, and an implicit initial encoding state.  This overload requires
    // that view_type be constructible from a code_unit_iterator pair.
    CONCEPT_REQUIRES(
        ranges::Constructible<
            view_type,
            code_unit_iterator&&,
            code_unit_sentinel&&>())
    basic_text_view(
        code_unit_iterator first,
        code_unit_sentinel last)
    :
        base_type{encoding_type::initial_state()},
        view{std::move(first), std::move(last)}
    {}

    // Overload to initialize a text view from an InputIterator and count pair,
    // and an explicitly specified initial encoding state.  This overload
    // requires that view_type be constructible from a code_unit_iterator pair.
    CONCEPT_REQUIRES(
        ranges::Constructible<
            view_type,
            code_unit_iterator,
            code_unit_iterator>())
    basic_text_view(
        state_type state,
        code_unit_iterator first,
        ranges::difference_type_t<code_unit_iterator> n)
    :
        base_type{std::move(state)},
        view{first, std::next(first, n)}
    {}

    // Overload to initialize a text view from an InputIterator and count pair,
    // and an implicit initial encoding state.  This overload requires that
    // view_type be constructible from a code_unit_iterator pair.
    CONCEPT_REQUIRES(
        ranges::Constructible<
            view_type,
            code_unit_iterator,
            code_unit_iterator>())
    basic_text_view(
        code_unit_iterator first,
        ranges::difference_type_t<code_unit_iterator> n)
    :
        base_type{encoding_type::initial_state()},
        view{first, std::next(first, n)}
    {}

    // Overload to initialize a text view with an explicitly specified initial
    // encoding state and a basic_string specialization.  This overload requires
    // that view_type be constructible from a code_unit_iterator pair, that
    // code_unit_iterator be constructible from a const code unit pointer, and
    // that the difference type (likely ptrdiff_t) for code_unit_iterator be
    // constructible from the basic_string size_type.  This overload enables
    // construction with basic_string specializations when view_type requires
    // pointers to contiguous memory (as is the case for text_view, wtext_view,
    // etc...).  When this overload is not enabled, construction with a
    // basic_string specialization will be handled by the constructor below
    // and the basic_string specialization will be treated as any other type
    // that models Range (in which case, code_unit_iterator must be
    // constructible from the basic_string const_iterator type).  Since
    // iterators into the provided basic_string object will be held, the
    // basic_string reference must reference an object with a lifetime that
    // exceeds the lifetime of the basic_text_view object being constructed.
    template<typename charT, typename traits, typename Allocator,
    CONCEPT_REQUIRES_(
        ranges::Constructible<code_unit_iterator, const charT *>(),
        ranges::ConvertibleTo< // Allow narrowing conversions.
            ranges::difference_type_t<code_unit_iterator>,
            typename basic_string<charT, traits, Allocator>::size_type>(),
        ranges::Constructible<
            view_type, code_unit_iterator, code_unit_sentinel>())>
    basic_text_view(
        state_type state,
        const basic_string<charT, traits, Allocator> &str)
    :
        basic_text_view{std::move(state),
                        str.c_str(),
                        ranges::difference_type_t<
                            code_unit_iterator>(str.size())}
    {}

    // Overload to initialize a text view with an implicitly specified initial
    // encoding state and a basic_string specialization.  See the above comments
    // for the similar overload that accepts an explicitly specified initial
    // state.
    template<typename charT, typename traits, typename Allocator,
    CONCEPT_REQUIRES_(
        ranges::Constructible<code_unit_iterator, const charT *>(),
        ranges::ConvertibleTo< // Allow narrowing conversions.
            ranges::difference_type_t<code_unit_iterator>,
            typename basic_string<charT, traits, Allocator>::size_type>(),
        ranges::Constructible<
            view_type, code_unit_iterator, code_unit_sentinel>())>
    basic_text_view(
        const basic_string<charT, traits, Allocator> &str)
    :
        basic_text_view{str.c_str(),
                        ranges::difference_type_t<
                            code_unit_iterator>(str.size())}
    {}

    // Overload to initialize a text view with an explicitly specified initial
    // encoding state and an InputIterator and Sentinel extracted from a
    // supplied InputRange.  The underlying view type must be constructible from
    // the iterator type of the range type.  Since iterators into the provided
    // InputRange object will be held, the specified range must be a reference
    // to an object with a lifetime that exceeds the lifetime of the
    // basic_text_view object being constructed.
    template<typename RT,
    CONCEPT_REQUIRES_(
        ranges::InputRange<RT>(),
        ranges::Constructible<
            code_unit_iterator,
            ranges::iterator_t<const RT>>(),
        ranges::Constructible<
            view_type,
            code_unit_iterator,
            code_unit_sentinel>())>
    basic_text_view(
        state_type state,
        const RT &range)
    :
        basic_text_view(std::move(state),
                        text_detail::adl_begin(range),
                        text_detail::adl_end(range))
    {}

    // Overload to initialize a text view with an implicitly specified initial
    // encoding state and an InputIterator and Sentinel extracted from a
    // supplied InputRange.  See the above comments for the similar overload
    // that accepts an explicitly specified initial state.
    template<typename RT,
    CONCEPT_REQUIRES_(
        ranges::InputRange<RT>(),
        ranges::Constructible<
            code_unit_iterator,
            ranges::iterator_t<const RT>>(),
        ranges::Constructible<
            view_type,
            code_unit_iterator,
            code_unit_sentinel>())>
    basic_text_view(
        const RT &range)
    :
        basic_text_view(text_detail::adl_begin(range),
                        text_detail::adl_end(range))
    {}

    // Overload to initialize a text view from a text iterator pair.
    // The initial encoding state is inferred from the first iterator.
    CONCEPT_REQUIRES(
        ranges::Constructible<
            code_unit_iterator,
            decltype(std::declval<iterator>().base())>(),
        ranges::Constructible<
            view_type,
            code_unit_iterator,
            code_unit_iterator>())
    basic_text_view(
        iterator first,
        iterator last)
    :
        basic_text_view{first.state(), first.base(), last.base()}
    {}

    // Overload to initialize a text view from a text iterator/sentinel pair.
    // The initial encoding state is inferred from the first iterator.
    CONCEPT_REQUIRES(
        ranges::Constructible<
            code_unit_iterator,
            decltype(std::declval<iterator>().base())>(),
        ranges::Constructible<
            view_type,
            code_unit_iterator,
            code_unit_sentinel>())
    basic_text_view(
        iterator first,
        sentinel last)
    :
        basic_text_view{first.state(), first.base(), last.base()}
    {}

    const view_type& base() const noexcept {
        return view;
    }

    const state_type& initial_state() const noexcept {
        return base_type::get();
    }

    iterator begin() const {
        return iterator{initial_state(), &view, text_detail::adl_begin(view)};
    }
    CONCEPT_REQUIRES(
        std::is_empty<state_type>::value &&
        ranges::Iterator<code_unit_sentinel>())
    iterator end() const
    {
        // Use the (empty) initial state to construct the end iterator.
        return iterator{ET::initial_state(), &view, text_detail::adl_end(view)};
    }
    CONCEPT_REQUIRES(
        ! std::is_empty<state_type>::value ||
        ! ranges::Iterator<code_unit_sentinel>())
    sentinel end() const
    {
        return sentinel{text_detail::adl_end(view)};
    }

private:
    view_type view;
};


using text_view = basic_text_view<
          execution_character_encoding,
          text_detail::basic_view<const char*>>;
using wtext_view = basic_text_view<
          execution_wide_character_encoding,
          text_detail::basic_view<const wchar_t*>>;
// FIXME: If P0482 were to be adopted, replace char with char8_t.
using u8text_view = basic_text_view<
          char8_character_encoding,
          text_detail::basic_view<const char*>>;
using u16text_view = basic_text_view<
          char16_character_encoding,
          text_detail::basic_view<const char16_t*>>;
using u32text_view = basic_text_view<
          char32_character_encoding,
          text_detail::basic_view<const char32_t*>>;


/*
 * make_text_view
 */
// Overload to construct a text view for an explicitly specified encoding type
// and error policy from an InputIterator and Sentinel pair, and an explicitly
// specified initial encoding state.
template<typename ET, typename TEP, typename IT, typename ST,
         typename VT = text_detail::basic_view<IT, ST>,
CONCEPT_REQUIRES_(
    TextEncoding<ET>(),
    TextErrorPolicy<TEP>(),
    ranges::InputIterator<IT>(),
    ranges::Sentinel<ST, IT>())>
auto make_text_view(
    typename ET::state_type state,
    IT first,
    ST last)
-> basic_text_view<ET, VT, TEP>
{
    return basic_text_view<ET, VT, TEP>{std::move(state),
                                        std::move(first),
                                        std::move(last)};
}

// Overload to construct a text view for an explicitly specified encoding type
// and an implicitly specified error policy from an InputIterator and Sentinel
// pair, and an explicitly specified initial encoding state.
template<typename ET, typename IT, typename ST,
         typename VT = text_detail::basic_view<IT, ST>,
CONCEPT_REQUIRES_(
    TextEncoding<ET>(),
    ranges::InputIterator<IT>(),
    ranges::Sentinel<ST, IT>())>
auto make_text_view(
    typename ET::state_type state,
    IT first,
    ST last)
-> basic_text_view<ET, VT>
{
    return basic_text_view<ET, VT>{std::move(state),
                                   std::move(first),
                                   std::move(last)};
}

// Overload to construct a text view for an implicitly assumed encoding type
// and explicitly specified error policy from an InputIterator and Sentinel
// pair, and an explicitly specified initial encoding state.
template<typename TEP, typename IT, typename ST, typename ET =
         default_encoding_type_t<ranges::value_type_t<IT>>,
CONCEPT_REQUIRES_(
    TextErrorPolicy<TEP>(),
    ranges::InputIterator<IT>(),
    ranges::Sentinel<ST, IT>())>
auto make_text_view(
    typename ET::state_type state,
    IT first,
    ST last)
-> decltype(text::make_text_view<ET, TEP>(std::move(state),
                                          std::move(first),
                                          std::move(last)))
{
    return text::make_text_view<ET, TEP>(std::move(state),
                                         std::move(first),
                                         std::move(last));
}

// Overload to construct a text view for an implicitly assumed encoding type
// and error policy from an InputIterator and Sentinel pair, and an explicitly
// specified initial encoding state.
template<typename IT, typename ST, typename ET =
         default_encoding_type_t<ranges::value_type_t<IT>>,
CONCEPT_REQUIRES_(
    ranges::InputIterator<IT>(),
    ranges::Sentinel<ST, IT>())>
auto make_text_view(
    typename ET::state_type state,
    IT first,
    ST last)
-> decltype(text::make_text_view<ET>(std::move(state),
                                     std::move(first),
                                     std::move(last)))
{
    return text::make_text_view<ET>(std::move(state),
                                    std::move(first),
                                    std::move(last));
}

// Overload to construct a text view for an explicitly specified encoding type
// and error policy from an InputIterator and Sentinel pair, and an implicit
// initial encoding state.
template<typename ET, typename TEP, typename IT, typename ST,
         typename VT = text_detail::basic_view<IT, ST>,
CONCEPT_REQUIRES_(
    TextEncoding<ET>(),
    TextErrorPolicy<TEP>(),
    ranges::InputIterator<IT>(),
    ranges::Sentinel<ST, IT>())>
auto make_text_view(
    IT first,
    ST last)
-> basic_text_view<ET, VT, TEP>
{
    return basic_text_view<ET, VT, TEP>{std::move(first),
                                        std::move(last)};
}

// Overload to construct a text view for an explicitly specified encoding type
// and an implicitly specified error policy from an InputIterator and Sentinel
// pair, and an implicit initial encoding state.
template<typename ET, typename IT, typename ST,
         typename VT = text_detail::basic_view<IT, ST>,
CONCEPT_REQUIRES_(
    TextEncoding<ET>(),
    ranges::InputIterator<IT>(),
    ranges::Sentinel<ST, IT>())>
auto make_text_view(
    IT first,
    ST last)
-> basic_text_view<ET, VT>
{
    return basic_text_view<ET, VT>{std::move(first),
                                   std::move(last)};
}

// Overload to construct a text view for an implicitly assumed encoding type
// and explicitly specified error policy from an InputIterator and Sentinel
// pair, and an implicit initial encoding state.
template<typename TEP, typename IT, typename ST, typename ET =
         default_encoding_type_t<ranges::value_type_t<IT>>,
CONCEPT_REQUIRES_(
    TextErrorPolicy<TEP>(),
    ranges::InputIterator<IT>(),
    ranges::Sentinel<ST, IT>())>
auto make_text_view(
    IT first,
    ST last)
-> decltype(text::make_text_view<ET, TEP>(std::move(first),
                                          std::move(last)))
{
    return text::make_text_view<ET, TEP>(std::move(first),
                                         std::move(last));
}

// Overload to construct a text view for an implicitly assumed encoding type
// and error policy from an InputIterator and Sentinel pair, and an implicit
// initial encoding state.
template<typename IT, typename ST, typename ET =
         default_encoding_type_t<ranges::value_type_t<IT>>,
CONCEPT_REQUIRES_(
    ranges::InputIterator<IT>(),
    ranges::Sentinel<ST, IT>())>
auto make_text_view(
    IT first,
    ST last)
-> decltype(text::make_text_view<ET>(std::move(first),
                                     std::move(last)))
{
    return text::make_text_view<ET>(std::move(first),
                                    std::move(last));
}

// Overload to construct a text view for an explicitly specified encoding type
// and error policy from a ForwardIterator and count pair, and an explicitly
// specified initial encoding state.
template<typename ET, typename TEP, typename IT,
CONCEPT_REQUIRES_(
    TextEncoding<ET>(),
    TextErrorPolicy<TEP>(),
    ranges::ForwardIterator<IT>())>
auto make_text_view(
    typename ET::state_type state,
    IT first,
    ranges::difference_type_t<IT> n)
-> decltype(text::make_text_view<ET, TEP>(std::move(state),
                                          std::move(first),
                                          std::move(std::next(first, n))))
{
    auto last = std::next(first, n);
    return text::make_text_view<ET, TEP>(std::move(state),
                                         std::move(first),
                                         std::move(last));
}

// Overload to construct a text view for an explicitly specified encoding type
// and implicitly assumed error policy from a ForwardIterator and count pair,
// and an explicitly specified initial encoding state.
template<typename ET, typename IT,
CONCEPT_REQUIRES_(
    TextEncoding<ET>(),
    ranges::ForwardIterator<IT>())>
auto make_text_view(
    typename ET::state_type state,
    IT first,
    ranges::difference_type_t<IT> n)
-> decltype(text::make_text_view<ET>(std::move(state),
                                     std::move(first),
                                     std::move(std::next(first, n))))
{
    auto last = std::next(first, n);
    return text::make_text_view<ET>(std::move(state),
                                    std::move(first),
                                    std::move(last));
}

// Overload to construct a text view for an implicitly assumed encoding type
// and explicitly specified error policy from a ForwardIterator and count pair,
// and an explicitly specified initial encoding state.
template<typename TEP, typename IT, typename ET =
         default_encoding_type_t<ranges::value_type_t<IT>>,
CONCEPT_REQUIRES_(
    TextErrorPolicy<TEP>(),
    ranges::ForwardIterator<IT>())>
auto make_text_view(
    typename ET::state_type state,
    IT first,
    ranges::difference_type_t<IT> n)
-> decltype(text::make_text_view<ET, TEP>(std::move(state),
                                          std::move(first),
                                          std::move(std::next(first, n))))
{
    auto last = std::next(first, n);
    return text::make_text_view<ET, TEP>(std::move(state),
                                         std::move(first),
                                         std::move(last));
}

// Overload to construct a text view for an implicitly assumed encoding type
// and error policy from a ForwardIterator and count pair, and an explicitly
// specified initial encoding state.
template<typename IT, typename ET =
         default_encoding_type_t<ranges::value_type_t<IT>>,
CONCEPT_REQUIRES_(
    ranges::ForwardIterator<IT>())>
auto make_text_view(
    typename ET::state_type state,
    IT first,
    ranges::difference_type_t<IT> n)
-> decltype(text::make_text_view<ET>(std::move(state),
                                     std::move(first),
                                     std::move(std::next(first, n))))
{
    auto last = std::next(first, n);
    return text::make_text_view<ET>(std::move(state),
                                    std::move(first),
                                    std::move(last));
}

// Overload to construct a text view for an explicitly specified encoding type
// and error policy from a ForwardIterator and count pair, and an implicit
// initial encoding state.
template<typename ET, typename TEP, typename IT,
CONCEPT_REQUIRES_(
    TextEncoding<ET>(),
    TextErrorPolicy<TEP>(),
    ranges::ForwardIterator<IT>())>
auto make_text_view(
    IT first,
    ranges::difference_type_t<IT> n)
-> decltype(text::make_text_view<ET, TEP>(std::move(first),
                                          std::move(std::next(first, n))))
{
    auto last = std::next(first, n);
    return text::make_text_view<ET, TEP>(std::move(first),
                                         std::move(last));
}

// Overload to construct a text view for an explicitly specified encoding type
// and implicitly assumed error policy from a ForwardIterator and count pair,
// and an implicit initial encoding state.
template<typename ET, typename IT,
CONCEPT_REQUIRES_(
    TextEncoding<ET>(),
    ranges::ForwardIterator<IT>())>
auto make_text_view(
    IT first,
    ranges::difference_type_t<IT> n)
-> decltype(text::make_text_view<ET>(std::move(first),
                                     std::move(std::next(first, n))))
{
    auto last = std::next(first, n);
    return text::make_text_view<ET>(std::move(first),
                                    std::move(last));
}

// Overload to construct a text view for an implicitly assumed encoding type
// and explicitly specified error policy from a ForwardIterator and count pair,
// and an implicit initial encoding state.
template<typename TEP, typename IT, typename ET =
         default_encoding_type_t<ranges::value_type_t<IT>>,
CONCEPT_REQUIRES_(
    TextErrorPolicy<TEP>(),
    ranges::ForwardIterator<IT>())>
auto make_text_view(
    IT first,
    ranges::difference_type_t<IT> n)
-> decltype(text::make_text_view<ET, TEP>(std::move(first),
                                          std::move(std::next(first, n))))
{
    auto last = std::next(first, n);
    return text::make_text_view<ET, TEP>(std::move(first),
                                         std::move(last));
}

// Overload to construct a text view for an implicitly assumed encoding type
// and error policy from a ForwardIterator and count pair, and an implicit
// initial encoding state.
template<typename IT, typename ET =
         default_encoding_type_t<ranges::value_type_t<IT>>,
CONCEPT_REQUIRES_(
    ranges::ForwardIterator<IT>())>
auto make_text_view(
    IT first,
    ranges::difference_type_t<IT> n)
-> decltype(text::make_text_view<ET>(std::move(first),
                                     std::move(std::next(first, n))))
{
    auto last = std::next(first, n);
    return text::make_text_view<ET>(std::move(first),
                                    std::move(last));
}

// Overload to construct a text view for an explicitly specified encoding type
// and error policy from an InputRange const reference and an explicitly
// specified initial encoding state.
template<typename ET, typename TEP, typename RT,
CONCEPT_REQUIRES_(
    TextEncoding<ET>(),
    TextErrorPolicy<TEP>(),
    ranges::InputRange<RT>())>
auto make_text_view(
    typename ET::state_type state,
    const RT &range)
-> decltype(text::make_text_view<ET, TEP>(std::move(state),
                                          text_detail::adl_begin(range),
                                          text_detail::adl_end(range)))
{
    return text::make_text_view<ET, TEP>(std::move(state),
                                         text_detail::adl_begin(range),
                                         text_detail::adl_end(range));
}

// Overload to construct a text view for an explicitly specified encoding type
// and implicitly assumed error policy from an InputRange const reference and
// an explicitly specified initial encoding state.
template<typename ET, typename RT,
CONCEPT_REQUIRES_(
    TextEncoding<ET>(),
    ranges::InputRange<RT>())>
auto make_text_view(
    typename ET::state_type state,
    const RT &range)
-> decltype(text::make_text_view<ET>(std::move(state),
                                     text_detail::adl_begin(range),
                                     text_detail::adl_end(range)))
{
    return text::make_text_view<ET>(std::move(state),
                                    text_detail::adl_begin(range),
                                    text_detail::adl_end(range));
}

// Overload to construct a text view for an implicitly assumed encoding type
// and explicitly specified error policy from an InputRange const reference
// and an explicitly specified initial encoding state.
template<typename TEP, typename RT, typename ET =
         default_encoding_type_t<ranges::range_value_type_t<RT>>,
CONCEPT_REQUIRES_(
    TextErrorPolicy<TEP>(),
    ranges::InputRange<RT>())>
auto make_text_view(
    typename ET::state_type state,
    const RT &range)
-> decltype(text::make_text_view<ET, TEP>(std::move(state),
                                          text_detail::adl_begin(range),
                                          text_detail::adl_end(range)))
{
    return text::make_text_view<ET, TEP>(std::move(state),
                                         text_detail::adl_begin(range),
                                         text_detail::adl_end(range));
}

// Overload to construct a text view for an implicitly assumed encoding type
// and error policy from an InputRange const reference and an explicitly
// specified initial encoding state.
template<typename RT, typename ET =
         default_encoding_type_t<ranges::range_value_type_t<RT>>,
CONCEPT_REQUIRES_(
    ranges::InputRange<RT>())>
auto make_text_view(
    typename ET::state_type state,
    const RT &range)
-> decltype(text::make_text_view<ET>(std::move(state),
                                     text_detail::adl_begin(range),
                                     text_detail::adl_end(range)))
{
    return text::make_text_view<ET>(std::move(state),
                                    text_detail::adl_begin(range),
                                    text_detail::adl_end(range));
}

// Overload to construct a text view for an explicitly specified encoding type
// and error policy from an InputRange const reference and an implicit initial
// encoding state.
template<typename ET, typename TEP, typename RT,
CONCEPT_REQUIRES_(
    TextEncoding<ET>(),
    TextErrorPolicy<TEP>(),
    ranges::InputRange<RT>())>
auto make_text_view(
    const RT &range)
-> decltype(text::make_text_view<ET, TEP>(text_detail::adl_begin(range),
                                          text_detail::adl_end(range)))
{
    return text::make_text_view<ET, TEP>(text_detail::adl_begin(range),
                                         text_detail::adl_end(range));
}

// Overload to construct a text view for an explicitly specified encoding type
// and implicitly assumed error policy from an InputRange const reference and
// an implicit initial encoding state.
template<typename ET, typename RT,
CONCEPT_REQUIRES_(
    TextEncoding<ET>(),
    ranges::InputRange<RT>())>
auto make_text_view(
    const RT &range)
-> decltype(text::make_text_view<ET>(text_detail::adl_begin(range),
                                     text_detail::adl_end(range)))
{
    return text::make_text_view<ET>(text_detail::adl_begin(range),
                                    text_detail::adl_end(range));
}

// Overload to construct a text view for an implicitly assumed encoding type
// and explicitly specified error policy from an InputRange const reference
// and an implicit initial encoding state.
template<typename TEP, typename RT, typename ET =
         default_encoding_type_t<ranges::range_value_type_t<RT>>,
CONCEPT_REQUIRES_(
    TextErrorPolicy<TEP>(),
    ranges::InputRange<RT>())>
auto make_text_view(
    const RT &range)
-> decltype(text::make_text_view<ET, TEP>(text_detail::adl_begin(range),
                                          text_detail::adl_end(range)))
{
    return text::make_text_view<ET, TEP>(text_detail::adl_begin(range),
                                         text_detail::adl_end(range));
}

// Overload to construct a text view for an implicitly assumed encoding type
// and error policy from an InputRange const reference and an implicit initial
// encoding state.
template<typename RT, typename ET =
         default_encoding_type_t<ranges::range_value_type_t<RT>>,
CONCEPT_REQUIRES_(
    ranges::InputRange<RT>())>
auto make_text_view(
    const RT &range)
-> decltype(text::make_text_view<ET>(text_detail::adl_begin(range),
                                     text_detail::adl_end(range)))
{
    return text::make_text_view<ET>(text_detail::adl_begin(range),
                                    text_detail::adl_end(range));
}

// Overload to construct a text view with an explicitly specified error policy
// from a text iterator and sentinel pair.  The initial encoding state is
// inferred from the first iterator.
template<typename TEP, typename TIT, typename TST, typename ET =
         encoding_type_t<TIT>,
CONCEPT_REQUIRES_(
    TextErrorPolicy<TEP>(),
    TextInputIterator<TIT>(),
    TextSentinel<TST, TIT>())>
auto make_text_view(
    TIT first,
    TST last)
-> decltype(text::make_text_view<ET, TEP>(first.state(),
                                          first.base(),
                                          last.base()))
{
    return text::make_text_view<ET, TEP>(first.state(),
                                         first.base(),
                                         last.base());
}

// Overload to construct a text view with an implicitly assumed error policy
// from a text iterator and sentinel pair.  The initial encoding state is
// inferred from the first iterator.
template<typename TIT, typename TST, typename ET = encoding_type_t<TIT>,
CONCEPT_REQUIRES_(
    TextInputIterator<TIT>(),
    TextSentinel<TST, TIT>())>
auto make_text_view(
    TIT first,
    TST last)
-> decltype(text::make_text_view<ET>(first.state(), first.base(), last.base()))
{
    return text::make_text_view<ET>(first.state(), first.base(), last.base());
}

// Overload to construct a text view with an explicitly specified error policy
// from an existing text view.
template<typename TEP, typename TVT,
CONCEPT_REQUIRES_(
    TextErrorPolicy<TEP>(),
    TextView<TVT>())>
auto make_text_view(
    TVT tv)
-> decltype(text::make_text_view<TEP>(text_detail::adl_begin(tv),
                                      text_detail::adl_end(tv)))
{
    return text::make_text_view<TEP>(text_detail::adl_begin(tv),
                                     text_detail::adl_end(tv));
}

// Overload to construct a text view with an implicitly assumed error policy
// from an existing text view.
template<typename TVT,
CONCEPT_REQUIRES_(
    TextView<TVT>())>
TVT make_text_view(
    TVT tv)
{
    return tv;
}


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_TEXT_VIEW_HPP
