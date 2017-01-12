# Copyright (c) 2016, Tristan Brindle, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

cmake_minimum_required(VERSION 3.0.2)

project(tv-range-v3-dump CXX)

find_package(text_view_range_v3 REQUIRED)

add_executable(
  tv-dump
  tv-dump.cpp)
set_target_properties(
  tv-dump
  PROPERTIES ${text_view_range_v3_PROPERTIES})
target_compile_options(
  tv-dump
  PRIVATE ${text_view_range_v3_COMPILE_OPTIONS})
target_compile_definitions(
  tv-dump
  PRIVATE ${text_view_range_v3_DEFINITIONS})
target_include_directories(
  tv-dump
  PRIVATE ${text_view_range_v3_INCLUDE_DIRS})
