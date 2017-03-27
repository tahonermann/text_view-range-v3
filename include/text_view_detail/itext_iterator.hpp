// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_ITEXT_ITERATOR_HPP // {
#define TEXT_VIEW_ITEXT_ITERATOR_HPP


#include <iterator>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <text_view_detail/adl_customization.hpp>
#include <text_view_detail/concepts.hpp>
#include <text_view_detail/iterator_preserve.hpp>
#include <text_view_detail/subobject.hpp>


namespace std {
namespace experimental {
inline namespace text {


namespace text_detail {

template<typename ET, typename CUIT, typename Enable = void>
struct itext_iterator_category;

template<typename ET, typename CUIT>
struct itext_iterator_category<
           ET, CUIT,
           typename std::enable_if<
               (bool)TextDecoder<ET, CUIT>() &&
               ! (bool)TextForwardDecoder<ET, CUIT>()>::type>
{
    using type = std::input_iterator_tag;
};

template<typename ET, typename CUIT>
struct itext_iterator_category<
           ET, CUIT,
           typename std::enable_if<
               (bool)TextForwardDecoder<ET, CUIT>() &&
               ! (bool)TextBidirectionalDecoder<ET, CUIT>()>::type>
{
    using type = std::forward_iterator_tag;
};

template<typename ET, typename CUIT>
struct itext_iterator_category<
           ET, CUIT,
           typename std::enable_if<
               (bool)TextBidirectionalDecoder<ET, CUIT>() &&
               ! (bool)TextRandomAccessDecoder<ET, CUIT>()>::type>
{
    using type = std::bidirectional_iterator_tag;
};

template<typename ET, typename CUIT>
struct itext_iterator_category<
           ET, CUIT,
           typename std::enable_if<
               (bool)TextRandomAccessDecoder<ET, CUIT>()>::type>
{
    using type = std::random_access_iterator_tag;
};

template<typename ET, typename CUIT>
using itext_iterator_category_t =
    typename itext_iterator_category<ET, CUIT>::type;


template<typename ET, typename VT,
CONCEPT_REQUIRES_(
    TextEncoding<ET>(),
    ranges::View<VT>())>
class itext_iterator_base
    : private subobject<typename ET::state_type>
{
    using base_type = subobject<typename ET::state_type>;

public:
    using encoding_type = ET;
    using view_type = VT;
    using state_type = typename encoding_type::state_type;
    using iterator =
        ranges::iterator_t<
            typename std::add_const<view_type>::type>;
    using iterator_category =
              text_detail::itext_iterator_category_t<
                  encoding_type,
                  iterator>;
    using value_type = character_type_t<encoding_type>;
    using reference = value_type;
    using pointer = typename std::add_const<value_type>::type*;
    using difference_type = ranges::difference_type_t<iterator>;

protected:
    itext_iterator_base() = default;

    itext_iterator_base(
        state_type state)
    :
        base_type{std::move(state)}
    {}

    state_type& mutable_state() noexcept {
        return base_type::get();
    }

public:
    const state_type& state() const noexcept {
        return base_type::get();
    }
};


// Primary class template is incomplete.
template<typename ET, typename VT, typename Enable = void>
class itext_iterator_data;

// Specialization for input views.
template<typename ET, typename VT>
class itext_iterator_data<
          ET,
          VT,
          typename std::enable_if<
              (bool)TextEncoding<ET>() &&
              (bool)ranges::View<VT>() &&
              (bool)TextDecoder<
                  ET,
                  ranges::iterator_t<
                      typename std::add_const<VT>::type>>() &&
              ! (bool)TextForwardDecoder<
                          ET,
                          ranges::iterator_t<
                              typename std::add_const<VT>::type>>()>::type>
: public itext_iterator_base<ET, VT>
{
public:
    using encoding_type = typename itext_iterator_data::encoding_type;
    using view_type = typename itext_iterator_data::view_type;
    using state_type = typename itext_iterator_data::state_type;
    using iterator = typename itext_iterator_data::iterator;

protected:
    itext_iterator_data() = default;

    itext_iterator_data(
        state_type state,
        const view_type *view,
        iterator current)
    :
        itext_iterator_base<ET, VT>{std::move(state)},
        view(view),
        current(std::move(current))
    {}

    iterator& mutable_base() noexcept {
        return current;
    }

    const view_type* underlying_view() const noexcept {
        return view;
    }

public:
    const iterator& base() const noexcept {
        return current;
    }

private:
    const view_type *view;
    iterator current;
};

// Specialization for forward views.
template<typename ET, typename VT>
class itext_iterator_data<
          ET,
          VT,
          typename std::enable_if<
              (bool)TextEncoding<ET>() &&
              (bool)ranges::View<VT>() &&
              (bool)TextForwardDecoder<
                  ET,
                  ranges::iterator_t<
                      typename std::add_const<VT>::type>>()>::type>
: public itext_iterator_base<ET, VT>
{
public:
    using encoding_type = typename itext_iterator_data::encoding_type;
    using view_type = typename itext_iterator_data::view_type;
    using state_type = typename itext_iterator_data::state_type;
    using iterator = typename itext_iterator_data::iterator;

protected:
    itext_iterator_data() = default;

    itext_iterator_data(
        state_type state,
        const view_type *view,
        iterator first)
    :
        itext_iterator_base<ET, VT>{std::move(state)},
        view(view),
        current_view{first, first}
    {}

    struct current_view_type {
        current_view_type()
            : first{}, last{} {}
        current_view_type(iterator first, iterator last)
            : first{first}, last{last} {}

        iterator begin() const { return first; }
        iterator end() const { return last; }

        iterator first;
        iterator last;
    };

    iterator& mutable_base() noexcept {
        return current_view.first;
    }

    current_view_type& mutable_base_range() noexcept {
        return current_view;
    }

    const view_type* underlying_view() const noexcept {
        return view;
    }

public:
    const iterator& base() const noexcept {
        return current_view.first;
    }

    const current_view_type& base_range() const noexcept {
        return current_view;
    }

private:
    const view_type *view;
    current_view_type current_view;
};

} // namespace text_detail


/*
 * itext_iterator
 */
template<typename ET, typename VT,
CONCEPT_REQUIRES_(
    TextEncoding<ET>(),
    ranges::View<VT>(),
    TextDecoder<
        ET,
        ranges::iterator_t<
            typename std::add_const<VT>::type>>())>
class itext_iterator
    : public text_detail::itext_iterator_data<ET, VT>
{
    using base_type = text_detail::itext_iterator_data<ET, VT>;

public:
    using encoding_type = typename itext_iterator::encoding_type;
    using view_type = typename itext_iterator::view_type;
    using state_type = typename itext_iterator::state_type;
    using value_type = typename itext_iterator::value_type;
    using iterator_category = typename itext_iterator::iterator_category;
    using iterator = typename itext_iterator::iterator;
    using pointer = typename itext_iterator::pointer;
    using reference = typename itext_iterator::reference;
    using difference_type = typename itext_iterator::difference_type;

private:
    class post_increment_proxy {
        friend class itext_iterator;
    public:
        post_increment_proxy(itext_iterator& self) noexcept
            : self(self), value(self.value)
        {}

        reference operator*() const noexcept {
            return value;
        }

    private:
        itext_iterator& self;
        value_type value;
    };

public:
    itext_iterator() = default;

    itext_iterator(
        state_type state,
        const view_type *view,
        iterator first)
    :
        base_type{std::move(state), view, std::move(first)}
    {
        ++*this;
    }

    // Iterators that are not ok include:
    // - Singular iterators.
    // - Past the end iterators.
    // - Iterators for which a decoding error occurred during increment or
    //   decrement operations.
    bool is_ok() const noexcept {
        return ok;
    }

    reference operator*() const noexcept {
        return { value };
    }
    pointer operator->() const noexcept {
        return &value;
    }

    friend bool operator==(
        const itext_iterator &l,
        const itext_iterator &r)
    {
        return l.equal(r);
    }
    friend bool operator!=(
        const itext_iterator &l,
        const itext_iterator &r)
    {
        return !(l == r);
    }

    CONCEPT_REQUIRES(
        TextRandomAccessDecoder<encoding_type, iterator>())
    friend bool operator<(
        const itext_iterator &l,
        const itext_iterator &r)
    {
        return (r - l) > 0;
    }
    CONCEPT_REQUIRES(
        TextRandomAccessDecoder<encoding_type, iterator>())
    friend bool operator>(
        const itext_iterator &l,
        const itext_iterator &r)
    {
        return r < l;
    }
    CONCEPT_REQUIRES(
        TextRandomAccessDecoder<encoding_type, iterator>())
    friend bool operator<=(
        const itext_iterator &l,
        const itext_iterator &r)
    {
        return !(r < l);
    }
    CONCEPT_REQUIRES(
        TextRandomAccessDecoder<encoding_type, iterator>())
    friend bool operator>=(
        const itext_iterator &l,
        const itext_iterator &r)
    {
        return !(l < r);
    }

    CONCEPT_REQUIRES(
        ! TextForwardDecoder<encoding_type, iterator>())
    itext_iterator& operator++() {
        ok = false;
        auto preserved_base =
            text_detail::make_iterator_preserve(this->mutable_base());
        auto end(text_detail::adl_end(*this->underlying_view()));
        while (preserved_base.get() != end) {
            value_type tmp_value;
            int decoded_code_units = 0;
            bool decoded_code_point = encoding_type::decode(
                this->mutable_state(),
                preserved_base.get(),
                end,
                tmp_value,
                decoded_code_units);
            preserved_base.update();
            if (decoded_code_point) {
                value = tmp_value;
                ok = true;
                break;
            }
        }
        return *this;
    }

    CONCEPT_REQUIRES(
        TextForwardDecoder<encoding_type, iterator>())
    itext_iterator& operator++() {
        ok = false;
        this->mutable_base_range().first = this->base_range().last;
        iterator tmp_iterator{this->base_range().first};
        auto end(text_detail::adl_end(*this->underlying_view()));
        while (tmp_iterator != end) {
            value_type tmp_value;
            int decoded_code_units = 0;
            bool decoded_code_point = encoding_type::decode(
                this->mutable_state(),
                tmp_iterator,
                end,
                tmp_value,
                decoded_code_units);
            this->mutable_base_range().last = tmp_iterator;
            if (decoded_code_point) {
                value = tmp_value;
                ok = true;
                break;
            }
            this->mutable_base_range().first = this->base_range().last;
        }
        return *this;
    }

    CONCEPT_REQUIRES(
        ! TextForwardDecoder<encoding_type, iterator>())
    post_increment_proxy operator++(int) {
        post_increment_proxy proxy{*this};
        ++*this;
        return proxy;
    }

    CONCEPT_REQUIRES(
        TextForwardDecoder<encoding_type, iterator>())
    itext_iterator operator++(int) {
        itext_iterator it{*this};
        ++*this;
        return it;
    }

    CONCEPT_REQUIRES(
        TextBidirectionalDecoder<encoding_type, iterator>())
    itext_iterator& operator--() {
        ok = false;
        this->mutable_base_range().last = this->base_range().first;
        std::reverse_iterator<iterator> rcurrent{this->base_range().last};
        std::reverse_iterator<iterator> rend{
            text_detail::adl_begin(*this->underlying_view())};
        while (rcurrent != rend) {
            value_type tmp_value;
            int decoded_code_units = 0;
            bool decoded_code_point = encoding_type::rdecode(
                this->mutable_state(),
                rcurrent,
                rend,
                tmp_value,
                decoded_code_units);
            this->mutable_base_range().first = rcurrent.base();
            if (decoded_code_point) {
                value = tmp_value;
                ok = true;
                break;
            }
            this->mutable_base_range().last = this->base_range().first;
        }
        return *this;
    }

    CONCEPT_REQUIRES(
        TextBidirectionalDecoder<encoding_type, iterator>())
    itext_iterator operator--(int) {
        itext_iterator it{*this};
        --*this;
        return it;
    }

    CONCEPT_REQUIRES(
        TextRandomAccessDecoder<encoding_type, iterator>())
    itext_iterator& operator+=(difference_type n) {
        if (n < 0) {
            this->mutable_base_range().first +=
                ((n+1) * encoding_type::max_code_units);
            --*this;
        } else if (n > 0) {
            this->mutable_base_range().last +=
                ((n-1) * encoding_type::max_code_units);
            ++*this;
        }
        return *this;
    }

    CONCEPT_REQUIRES(
        TextRandomAccessDecoder<encoding_type, iterator>())
    friend itext_iterator operator+(
        itext_iterator l,
        difference_type n)
    {
        return l += n;
    }

    CONCEPT_REQUIRES(
        TextRandomAccessDecoder<encoding_type, iterator>())
    friend itext_iterator operator+(
        difference_type n,
        itext_iterator r)
    {
        return r += n;
    }

    CONCEPT_REQUIRES(
        TextRandomAccessDecoder<encoding_type, iterator>())
    itext_iterator& operator-=(difference_type n)
    {
        return *this += -n;
    }

    CONCEPT_REQUIRES(
        TextRandomAccessDecoder<encoding_type, iterator>())
    friend itext_iterator operator-(
        itext_iterator l,
        difference_type n)
    {
        return l -= n;
    }

    CONCEPT_REQUIRES(
        TextRandomAccessDecoder<encoding_type, iterator>())
    friend difference_type operator-(
        const itext_iterator &l,
        const itext_iterator &r)
    {
        return (l.base_range().first - r.base_range().first) /
               encoding_type::max_code_units;
    }

    CONCEPT_REQUIRES(
        TextRandomAccessDecoder<encoding_type, iterator>())
    reference operator[](
        difference_type n) const
    {
        return { *(*this + n) };
    }

private:
    CONCEPT_REQUIRES(
        ! ranges::ForwardIterator<iterator>())
    bool equal(const itext_iterator &other) const {
        // For input iterators, the base iterator corresponds to the next input
        // to be decoded.  Naively checking for base comparison only therefore
        // results in premature matches when the last code point in the input
        // is consumed.  For this reason, base equality is only considered a
        // match if the iterator is not ok.  It is assumed that the reason the
        // base iterator is not ok is that an attempt was made to decode a
        // code point after the last one in the input.  This heuristic can be
        // defeated when an iterator is not ok for other reasons.
        return ok == other.ok
            && (!ok || this->base() == other.base());
    }
    CONCEPT_REQUIRES(
        ranges::ForwardIterator<iterator>())
    bool equal(const itext_iterator &other) const {
        return this->base() == other.base();
    }

private:
    value_type value = {};
    bool ok = false;
};


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_ITEXT_ITERATOR_HPP
