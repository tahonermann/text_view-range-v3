# Copyright (c) 2016, Tristan Brindle, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

cmake_minimum_required(VERSION 3.0.2)

project(text_view-range-v3-examples CXX)

include(CTest)

add_subdirectory(tv-dump)
add_subdirectory(tv-enumerate-utf8-code-points)
add_subdirectory(tv-find-utf8-multi-code-unit-code-point)
