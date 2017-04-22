# Copyright (c) 2017, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

cmake_minimum_required(VERSION 3.0.2)

project(text_view-range-v3_include CXX)

add_library(
  text-view-range-v3
  INTERFACE)
target_compile_options(
  text-view-range-v3
  INTERFACE ${text_view_range_v3_COMPILE_OPTIONS})
target_compile_definitions(
  text-view-range-v3
  INTERFACE ${text_view_range_v3_DEFINITIONS})
target_include_directories(
  text-view-range-v3
  INTERFACE
    $<BUILD_INTERFACE:${text_view_range_v3_INCLUDE_DIRS}>
    $<INSTALL_INTERFACE:${TEXT_VIEW_RANGE_V3_DESTINATION_INCLUDE}>)
target_link_libraries(
  text-view-range-v3
  INTERFACE range-v3)

install(
  TARGETS text-view-range-v3
  EXPORT text_view-range-v3-targets
  INCLUDES DESTINATION ${TEXT_VIEW_RANGE_V3_DESTINATION_INCLUDE})
