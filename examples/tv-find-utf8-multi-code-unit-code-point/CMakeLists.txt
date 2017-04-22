# Copyright (c) 2016, Tristan Brindle, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

cmake_minimum_required(VERSION 3.0.2)

project(text_view-range-v3_examples_tv-find-utf8-multi-code-unit-code-point CXX)

find_package(text_view_range_v3 REQUIRED)

add_executable(
  tv-find-utf8-multi-code-unit-code-point
  tv-find-utf8-multi-code-unit-code-point.cpp)
set_target_properties(
  tv-find-utf8-multi-code-unit-code-point
  PROPERTIES ${text_view_range_v3_PROPERTIES})
target_compile_options(
  tv-find-utf8-multi-code-unit-code-point
  PRIVATE ${text_view_range_v3_COMPILE_OPTIONS})
target_compile_definitions(
  tv-find-utf8-multi-code-unit-code-point
  PRIVATE ${text_view_range_v3_DEFINITIONS})
target_include_directories(
  tv-find-utf8-multi-code-unit-code-point
  PRIVATE ${text_view_range_v3_INCLUDE_DIRS})

include(CTest)
add_test(
  NAME tv-find-utf8-multi-code-unit-code-point
  COMMAND tv-find-utf8-multi-code-unit-code-point)
