// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_SUBOBJECT_HPP // {
#define TEXT_VIEW_SUBOBJECT_HPP


#include <range/v3/utility/concepts.hpp>
#include <type_traits>


namespace std {
namespace experimental {
inline namespace text {
namespace text_detail {


enum class subobject_specialization {
    base,
    member
};

template<typename T>
constexpr subobject_specialization
select_subobject_specialization() noexcept {
    return std::is_class<T>::value
#if __cplusplus >= 201402L
               && ! std::is_final<T>::value
#endif
           ? subobject_specialization::base
           : subobject_specialization::member;
}


// Primary class template is incomplete.
template<typename T,
subobject_specialization = select_subobject_specialization<T>(),
CONCEPT_REQUIRES_(ranges::SemiRegular<T>())>
class subobject;

// Specialization for embedding as a data member.
template<typename T>
class subobject<T, subobject_specialization::member> {
public:
    constexpr subobject() = default;

    constexpr subobject(const T& t)
        noexcept(std::is_nothrow_copy_constructible<T>::value)
    : t(t) {}

    constexpr subobject(T&& t)
        noexcept(std::is_nothrow_move_constructible<T>::value)
    : t(std::move(t)) {}

#if __cplusplus >= 201402L
    constexpr
#endif
    T& get() noexcept {
        return t;
    }
    constexpr const T& get() const noexcept {
        return t;
    }

private:
    T t;
};

// Specialization for embedding as a base class.
template<typename T>
class subobject<T, subobject_specialization::base>
    : public T
{
public:
    constexpr subobject() = default;

    constexpr subobject(const T& t)
        noexcept(std::is_nothrow_copy_constructible<T>::value)
    : T(t) {}

    constexpr subobject(T&& t)
        noexcept(std::is_nothrow_move_constructible<T>::value)
    : T(std::move(t)) {}

#if __cplusplus >= 201402L
    constexpr
#endif
    T& get() noexcept {
        return *this;
    }
    constexpr const T& get() const noexcept {
        return *this;
    }
};


} // namespace text_detail
} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_SUBOBJECT_HPP
