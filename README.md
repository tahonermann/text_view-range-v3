Travis CI Build Status: [![Travis Build Status](https://travis-ci.org/tahonermann/text_view-range-v3.svg?branch=master)](https://travis-ci.org/tahonermann/text_view-range-v3)

# The port of [Text_view] to [range-v3] is a work in progress!

This documentation is preliminary.  [Text_view] does not yet build with
[range-v3].  Please check back later!

# [Text_view for range-v3][Text_view-range-v3]
A [range-v3] based character encoding and code point enumeration library.

[Text_view for range-v3][Text_view-range-v3] is a port of the [text_view]
project to [range-v3].  See the
[text_view documentation](https://github.com/tahonermann/text_view/blob/master/README.md)
for more details.

For discussion of this project, please post and/or subscribe to the
`text_view@googlegroups.com` group hosted at
https://groups.google.com/d/forum/text_view

- [Overview](#overview)
- [Current features and limitations](#current-features-and-limitations)
- [Requirements](#requirements)
- [Build and installation](#build-and-installation)
  - [Building and installing range-v3]
    (#building-and-installing-range-v3)
  - [Building and installing Text_view for range-v3]
    (#building-and-installing-text_view-for-range-v3)
- [Usage](#usage)
- [Supported Encodings](#supported-encodings)
- [Terminology](#terminology)
- [References](#references)

# Overview
See the [Text_view documentation](https://github.com/tahonermann/text_view/blob/master/README.md#overview).

# Current features and limitations
See the [Text_view documentation](https://github.com/tahonermann/text_view/blob/master/README.md#current-features-and-limitations).

# Requirements
[Text_view for range-v3][Text_view-range-v3] depends on the [range-v3] library
and is expected to build and run correctly with any compiler supported by
[range-v3].

# Build and installation
This section provides instructions for building
[Text_view for range-v3][Text_view-range-v3] and suitable versions of its
dependencies.

## Building and installing [range-v3]
[Text_view for range-v3][Text_view-range-v3] only depends on headers provided
by [range-v3] and no build or installation is required.
[Text_view for range-v3][Text_view-range-v3] is known to build successfully with
[range-v3] git revision `FIXME`.  The
following commands can be used to checkout a known good revision.

```sh
$ git clone https://github.com/ericniebler/range-v3.git range-v3
$ cd range-v3
$ git checkout FIXME
```

## Building and installing [Text_view for range-v3][Text_view-range-v3]
[Text_view for range-v3][Text_view-range-v3] is currently a header-only
library, so no build or installation is currently required to use it.  It is
unlikely that [Text_view for range-v3][Text_view-range-v3] will remain a
header-only library in the future, so at some point, a build and installation
step will be required.

[Text_view for range-v3][Text_view-range-v3] has a [CMake] based build system
sufficient to build and run its tests, to validate example code, and to perform
a minimal installation following established operating system conventions.  By
default, files will be installed under `/usr/local` on UNIX and UNIX-like
systems, and under `C:\Program Files` on Windows.  The installation location
can be changed by invoking `cmake` with a `-DCMAKE_INSTALL_PREFIX=<path>`
option.  On UNIX and UNIX-like systems, header files will be installed in the
`include` directory of the installation destination, and other files will be
installed under `share/text_view`.  On Windows, header files be installed in
the `text_view\include` directory of the installation destination, and other
files will be installed under `text_view`.

Unless [range-v3] is installed to a common location, it will be necessary to
inform the build where it is installed.  This is typically done by setting the
`RANGE_V3_INSTALL_PATH` environment variable.  As of this writing, [range-v3]
does not provide an installation option, so `RANGE_V3_INSTALL_PATH` should
specify the location where the [range-v3] source resides (the directory that
contains the [range-v3] `include` directory).

The following commands suffice to build and run tests and examples, and perform
an installation.  If the build succeeds, built test and example programs will
be present in the `test` and `examples` subdirectories of the build directory
(the built test and example programs are not installed), and header files,
example code, cmake package configuration modules, and other miscellaneous files
will be present in the installation directory.

```sh
$ vi setenv.sh  # Update RANGE_V3_INSTALL_PATH.
$ . ./setenv.sh
$ mkdir build
$ cd build
$ cmake .. [-DCMAKE_INSTALL_PREFIX=/path/to/install/to]
$ cmake --build . --target install
$ ctest
```

`check` and `check-install` [CMake] targets are also available for automating
build and test.  The `check` target performs a build without installation and
then runs the tests.  The `check-install` target performs a build, runs tests,
installs to a location within the build directory, and then performs tests
(verifying that example code builds) on the installation.

The installation includes a [CMake] based build system for building the example
code.  To build all of the examples, run `cmake` specifying the `examples`
directory of the installation as the source directory.  Alternatively, each
example can be built independently by specifying its source directory as the
source directory in a `cmake` invocation.  If the installation was to a
non-default installation location (`-DCMAKE_INSTALL_PREFIX` was specified),
then it may be necessary to set `CMAKE_PREFIX_PATH` to the
[Text_view for range-v3][Text_view-range-v3]
installation location (the location `CMAKE_INSTALL_PREFIX` was set to) or
`text_view_range_v3_DIR` to the directory containing the installed
`text_view_range_v3-config.cmake` file, so that the
[Text_view for range-v3][Text_view-range-v3] package configuration
file is found.  See the [CMake] documentation for more details.

The following commands suffice to build all of the installed examples.

```sh
$ cd /path/to/installation/text_view-range-v3/examples
$ mkdir build
$ cd build
$ cmake .. [-DCMAKE_PREFIX_PATH=/path/to/installation]
$ cmake --build .
$ ctest
```

# Usage
See the [Text_view documentation](https://github.com/tahonermann/text_view/blob/master/README.md#usage).

# Supported Encodings
See the [Text_view documentation](https://github.com/tahonermann/text_view/blob/master/README.md#supported-encodings).

# Terminology
See the [Text_view documentation](https://github.com/tahonermann/text_view/blob/master/README.md#terminology).

# References
- [Text_view]  
  https://github.com/tahonermann/text_view
- [Text_view-range-v3]  
  https://github.com/tahonermann/text_view-range-v3
- [range-v3]  
  https://github.com/ericniebler/range-v3
- [CMake]  
  https://cmake.org

[Text_view]:
https://github.com/tahonermann/text_view
(Text_view library)
[Text_view-range-v3]:
https://github.com/tahonermann/text_view-range-v3
(Text_view for range-v3)
[range-v3]:
https://github.com/ericniebler/range-v3
(Experimental range library for C++11/14/17)
[cmake]:
https://cmake.org
(The CMake build system)
