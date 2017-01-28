// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_TEXT_OITERATOR_HPP // {
#define TEXT_VIEW_TEXT_OITERATOR_HPP


#include <iterator>
#include <text_view_detail/concepts.hpp>
#include <text_view_detail/default_encoding.hpp>
#include <text_view_detail/iterator_preserve.hpp>
#include <text_view_detail/subobject.hpp>


namespace std {
namespace experimental {
inline namespace text {


/*
 * otext_iterator
 */
template<typename ET, typename CUIT,
CONCEPT_REQUIRES_(
    TextEncoding<ET>(),
    CodeUnitOutputIterator<CUIT, code_unit_type_t<ET>>())>
class otext_iterator
    : private text_detail::subobject<typename ET::state_type>
{
    using base_type = text_detail::subobject<typename ET::state_type>;

public:
    using encoding_type = ET;
    using state_type = typename ET::state_type;
    using state_transition_type = typename ET::state_transition_type;
    using iterator = CUIT;
    using iterator_category = std::output_iterator_tag;
    using value_type = character_type_t<encoding_type>;
    using reference = value_type&;
    using pointer = value_type*;
    using difference_type = ranges::iterator_difference_t<iterator>;

public:
    otext_iterator() = default;

    otext_iterator(
        state_type state,
        iterator current)
    :
        base_type{std::move(state)},
        current(std::move(current))
    {}

    const state_type& state() const noexcept {
        return base_type::get();
    }

    const iterator& base() const noexcept {
        current;
    }

    otext_iterator& operator*() const noexcept {
        return *const_cast<otext_iterator*>(this);
    }

    otext_iterator& operator++() noexcept {
        return *this;
    }
    otext_iterator& operator++(int) noexcept {
        return *this;
    }

    otext_iterator& operator=(
        const state_transition_type &stt)
    {
        // Preserve current so that it remains unmutated in the event that
        // encode_state_transition() throws an exception (unless it is an
        // actual output iterator).
        auto preserved_current = text_detail::make_iterator_preserve(current);
        int encoded_code_units = 0;
        encoding_type::encode_state_transition(
            mutable_state(), preserved_current.get(), stt, encoded_code_units);
        preserved_current.update();
        return *this;
    }

    otext_iterator& operator=(
        const character_type_t<encoding_type> &value)
    {
        // Preserve current so that it remains unmutated in the event that
        // encode_state_transition() throws an exception (unless it is an
        // actual output iterator).
        auto preserved_current = text_detail::make_iterator_preserve(current);
        int encoded_code_units = 0;
        encoding_type::encode(
            mutable_state(), preserved_current.get(), value, encoded_code_units);
        preserved_current.update();
        return *this;
    }

private:
    state_type& mutable_state() noexcept {
        return base_type::get();
    }

    mutable iterator current;
};


/*
 * make_otext_iterator
 */
// Overload to construct an output text iterator for an explicitly specified
// encoding from an output iterator and and an explicitly specified initial
// encoding state.
template<typename ET, typename IT,
CONCEPT_REQUIRES_(
    TextEncoding<ET>(),
    CodeUnitOutputIterator<IT, code_unit_type_t<ET>>())>
otext_iterator<ET, IT> make_otext_iterator(
    typename ET::state_type state,
    IT out)
{
    return otext_iterator<ET, IT>{std::move(state), std::move(out)};
}

// Overload to construct an output text iterator for an explicitly specified
// encoding from an output iterator and and an implicit initial encoding state.
template<typename ET, typename IT,
CONCEPT_REQUIRES_(
    TextEncoding<ET>(),
    CodeUnitOutputIterator<IT, code_unit_type_t<ET>>())>
otext_iterator<ET, IT> make_otext_iterator(
    IT out)
{
    return otext_iterator<ET, IT>{ET::initial_state(), std::move(out)};
}


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_OTEXT_ITERATOR_HPP
