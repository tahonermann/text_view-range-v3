# Copyright (c) 2016, Tristan Brindle, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

cmake_minimum_required(VERSION 3.0.2)

project(test-text-view-range-v3 CXX)

find_package(text_view_range_v3 REQUIRED)

add_executable(
  test-text-view
  test-text_view.cpp)
set_target_properties(
  test-text-view
  PROPERTIES ${text_view_range_v3_PROPERTIES})
target_compile_options(
  test-text-view
  PRIVATE ${text_view_range_v3_COMPILE_OPTIONS})
target_compile_definitions(
  test-text-view
  PRIVATE ${text_view_range_v3_DEFINITIONS})
target_include_directories(
  test-text-view
  PRIVATE ${text_view_range_v3_INCLUDE_DIRS})

include(CTest)
add_test(
  NAME test-text-view
  COMMAND test-text-view)

add_executable(
  test-subobject
  test-subobject.cpp)
set_target_properties(
  test-subobject
  PROPERTIES ${text_view_range_v3_PROPERTIES})
target_compile_options(
  test-subobject
  PRIVATE ${text_view_range_v3_COMPILE_OPTIONS})
target_compile_definitions(
  test-subobject
  PRIVATE ${text_view_range_v3_DEFINITIONS})
target_include_directories(
  test-subobject
  PRIVATE ${text_view_range_v3_INCLUDE_DIRS})

include(CTest)
add_test(
  NAME test-subobject
  COMMAND test-subobject)
