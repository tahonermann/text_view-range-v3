// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_VOID_T_HPP // {
#define TEXT_VIEW_VOID_T_HPP


namespace std {
namespace experimental {
inline namespace text {
namespace text_detail {

// Prior to CWG1558, it is necessary to use alias template parameters in order
// for SFINAE to be applied.
template<typename ...Ts>
struct make_void { using type = void; };
template<typename ...Ts>
using void_t = typename make_void<Ts...>::type;

} // namespace text_detail
} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_VOID_T_HPP
