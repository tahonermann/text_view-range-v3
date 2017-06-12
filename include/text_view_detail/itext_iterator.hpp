// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_ITEXT_ITERATOR_HPP // {
#define TEXT_VIEW_ITEXT_ITERATOR_HPP


#include <cassert>
#include <iterator>
#include <range/v3/range_traits.hpp>
#include <range/v3/utility/iterator_traits.hpp>
#include <text_view_detail/adl_customization.hpp>
#include <text_view_detail/caching_iterator.hpp>
#include <text_view_detail/concepts.hpp>
#include <text_view_detail/error_policy.hpp>
#include <text_view_detail/exceptions.hpp>
#include <text_view_detail/subobject.hpp>


namespace std {
namespace experimental {
inline namespace text {


namespace text_detail {

template<typename ET,
CONCEPT_REQUIRES_(
    TextEncoding<ET>())>
class character_or_error {
    using character_type = character_type_t<ET>;

public:
    bool has_character() const noexcept {
        return have_character;
    }

    void set_character(character_type c) noexcept {
        u.c = c;
        have_character = true;
    }
    character_type& get_character() {
        assert(have_character);
        return u.c;
    }
    const character_type& get_character() const {
        return const_cast<character_or_error*>(this)->get_character();
    }

    void set_error(decode_status ds) noexcept {
        u.ds = ds;
        have_character = false;
    }
    decode_status get_error() const noexcept {
        return have_character ? decode_status::no_error : u.ds;
    }

private:
    union {
        decode_status ds;
        character_type c;
    } u = { decode_status::no_error };
    bool have_character = false;
};


template<typename I, typename Enable = void>
struct itext_current_iterator_type;
template<typename I>
struct itext_current_iterator_type<
           I,
           typename std::enable_if<
               (bool)ranges::InputIterator<I>() &&
               ! (bool)ranges::ForwardIterator<I>()>::type>
{
    using type = caching_iterator<I>;
};
template<typename I>
struct itext_current_iterator_type<
           I,
           typename std::enable_if<
               (bool)ranges::ForwardIterator<I>()>::type>
{
    using type = I;
};

template<typename I>
using itext_current_iterator_type_t =
    typename itext_current_iterator_type<I>::type;


template<typename ET, typename CUIT, typename Enable = void>
struct itext_iterator_category;

template<typename ET, typename CUIT>
struct itext_iterator_category<
           ET, CUIT,
           typename std::enable_if<
               (bool)ranges::InputIterator<CUIT>() &&
               ! (bool)ranges::ForwardIterator<CUIT>()>::type>
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
              (bool)ranges::InputIterator<
                  ranges::iterator_t<
                      typename std::add_const<VT>::type>>() &&
              ! (bool)ranges::ForwardIterator<
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

    const view_type* underlying_view() const noexcept {
        return view;
    }

public:
    const iterator& base() const noexcept {
        return current.base();
    }

    auto base_range() const noexcept
    -> decltype(std::declval<const caching_iterator<iterator>>().cached_range())
    {
        return current.cached_range();
    }

    auto look_ahead_range() const noexcept
    -> decltype(std::declval<const caching_iterator<iterator>>().look_ahead_range())
    {
        return current.look_ahead_range();
    }

private:
    const view_type *view;

protected:
    caching_iterator<iterator> current;
};

// Specialization for forward views.
template<typename ET, typename VT>
class itext_iterator_data<
          ET,
          VT,
          typename std::enable_if<
              (bool)TextEncoding<ET>() &&
              (bool)ranges::View<VT>() &&
              (bool)ranges::ForwardIterator<
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

protected:
    current_view_type current_view;
};

} // namespace text_detail


/*
 * itext_iterator
 */
template<typename ET, typename VT, typename TEP = text_default_error_policy,
CONCEPT_REQUIRES_(
    TextEncoding<ET>(),
    ranges::View<VT>(),
    TextErrorPolicy<TEP>(),
    TextForwardDecoder<
        ET,
        text_detail::itext_current_iterator_type_t<
            ranges::iterator_t<
                typename std::add_const<VT>::type>>>())>
class itext_iterator
    : public text_detail::itext_iterator_data<ET, VT>
{
    using base_type = text_detail::itext_iterator_data<ET, VT>;

public:
    using encoding_type = typename itext_iterator::encoding_type;
    using view_type = typename itext_iterator::view_type;
    using error_policy = TEP;
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

        reference operator*() const {
            return self.dereference(value);
        }
        pointer operator->() const {
            return &self.dereference(value);
        }

    private:
        itext_iterator& self;
        text_detail::character_or_error<encoding_type> value;
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

    bool error_occurred() const noexcept {
        return text::error_occurred(value.get_error());
    }

    decode_status get_error() const noexcept {
        return value.get_error();
    }

    // Iterators that are not ok include:
    // - Singular iterators.
    // - Past the end iterators.
    bool is_ok() const noexcept {
        return ok;
    }

    reference operator*() const {
        return dereference(value);
    }
    pointer operator->() const {
        return &dereference(value);
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
        this->current.clear_cache();
        auto end = text_detail::make_caching_iterator_sentinel(
                       text_detail::adl_end(*this->underlying_view()));
        while (this->current != end) {
            value_type tmp_value;
            int decoded_code_units = 0;
            decode_status ds = encoding_type::decode(
                this->mutable_state(),
                this->current,
                end,
                tmp_value,
                decoded_code_units);
            if (text::error_occurred(ds)) {
                value.set_error(ds);
                ok = true;
                break;
            }
            else if (ds == decode_status::no_error) {
                value.set_character(tmp_value);
                ok = true;
                break;
            }
            // Clear the cache prior to looping to maintain consistency with
            // forward iterators; non-character encoding sequences are not
            // reflected in base_range().
            this->current.clear_cache();
        }
        return *this;
    }

    CONCEPT_REQUIRES(
        TextForwardDecoder<encoding_type, iterator>())
    itext_iterator& operator++() {
        ok = false;
        this->current_view.first = this->current_view.last;
        iterator tmp_iterator{this->current_view.first};
        auto end(text_detail::adl_end(*this->underlying_view()));
        while (tmp_iterator != end) {
            value_type tmp_value;
            int decoded_code_units = 0;
            decode_status ds = encoding_type::decode(
                this->mutable_state(),
                tmp_iterator,
                end,
                tmp_value,
                decoded_code_units);
            this->current_view.last = tmp_iterator;
            if (text::error_occurred(ds)) {
                value.set_error(ds);
                ok = true;
                break;
            }
            else if (ds == decode_status::no_error) {
                value.set_character(tmp_value);
                ok = true;
                break;
            }
            this->current_view.first = this->current_view.last;
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
        this->current_view.last = this->current_view.first;
        std::reverse_iterator<iterator> rcurrent{this->current_view.last};
        std::reverse_iterator<iterator> rend{
            text_detail::adl_begin(*this->underlying_view())};
        while (rcurrent != rend) {
            value_type tmp_value;
            int decoded_code_units = 0;
            decode_status ds = encoding_type::rdecode(
                this->mutable_state(),
                rcurrent,
                rend,
                tmp_value,
                decoded_code_units);
            this->current_view.first = rcurrent.base();
            if (text::error_occurred(ds)) {
                value.set_error(ds);
                ok = true;
                break;
            }
            else if (ds == decode_status::no_error) {
                value.set_character(tmp_value);
                ok = true;
                break;
            }
            this->current_view.last = this->current_view.first;
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
            this->current_view.first +=
                ((n+1) * encoding_type::max_code_units);
            --*this;
        } else if (n > 0) {
            this->current_view.last +=
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
        return (l.current_view.first - r.current_view.first) /
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
    static const value_type& dereference(
        const text_detail::character_or_error<encoding_type> &coe)
    {
        if (coe.has_character()) {
            return coe.get_character();
        }
        if (std::is_base_of<
                text_permissive_error_policy,
                error_policy
            >::value)
        {
            // Permissive error policy: return the substitution
            // character.
            using CT = character_type_t<encoding_type>;
            using CST = character_set_type_t<CT>;
            static CT c{CST::get_substitution_code_point()};
            return c;
        } else {
            // Strict error policy: throw an exception.
            throw text_decode_error{coe.get_error()};
        }
    }

    text_detail::character_or_error<encoding_type> value;
    bool ok = false;
};


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_ITEXT_ITERATOR_HPP
