# Foundation Library  -  Public Domain

This library provides a cross-platform foundation library in C providing basic support data types and
functions to write applications and games in a platform-independent fashion. It provides:

* Abstractions and unification of basic data types
* Pluggable memory management
* Threads and synchronization
* Atomic operations
* Timing and profiling
* Object lifetime management
* Events processing
* File system access
* Dynamic library loading
* Process spawning
* Logging, error reporting and asserts
* String handling in UTF-8 and UTF-16
* Murmur hasing and statically hashed strings
* Math support for 32 and 64 bit floats
* JSON/SJSON parser
* SHA256/SHA512 digest
* Application, environment and system queries and control
* Regular expressions
* Exception utilities (SEH, signals)

It is written with the following API design principles in mind:

* Consistent. All functions, parameters and types should follow a well defined pattern in order to make it easy to remember how function names are constructed and how to pass the expected parameters.
* Orthogonal. A function should not have any side effects, and there should be only one way to perform an operation in the system.
* Specialized. A function in an API should perform a single task. Functions should not do completely different unrelated tasks or change behaviour depending on the contents of the variables passed in.
* Compact. The API needs to be compact, meaning the user can use it without using a manual. Note though that "compact" does not mean "small". A consistent naming scheme makes the API easier to use and remember.
* Contained. Third party dependencies are kept to an absolute minimum and prefer to use primitive or well-defined data types.

Platforms and architectures currently supported:

* Windows (x86, x86-64), Vista or later
* MacOS X (x86-64), 10.7+
* Linux (x86, x86-64, PPC, ARM)
* FreeBSD (x86, x86-64, PPC, ARM)
* iOS (ARMv7, ARMv7s, ARMv8/AArch64), 6.0+
* Android (ARMv6, ARMv7, ARMv8/AArch64, x86, x86-64, MIPS, MIPS64)
* Raspberry Pi (ARMv6)
* PNaCl


Please consider our Patreon - <https://www.patreon.com/rampantpixels>


The latest source code maintained by Rampant Pixels is always available at  
<https://github.com/rampantpixels/foundation_lib>

Pre-generated documentation in HTML format for the latest release can be found at  
<http://rampantpixels.github.io/foundation_lib/doc>

Master branch is used for stable releases. Development is done in feature branches from the develop branch  
<https://github.com/rampantpixels/foundation_lib/tree/develop>

Cross-platform build system uses Ninja  
<http://martine.github.io/ninja>

The PNaCl build of the latest release can be tested at  
<http://www.rampantpixels.com/foundation_lib/pnacl/>

Test suite coverage and scan reports:  
[![codecov.io](http://codecov.io/github/rampantpixels/foundation_lib/coverage.svg?branch=develop)](http://codecov.io/github/rampantpixels/foundation_lib?branch=develop)
[![coverity.com](https://scan.coverity.com/projects/6944/badge.svg)](https://scan.coverity.com/projects/rampantpixels-foundation_lib)

This library is put in the public domain; you can redistribute it and/or modify it without any restrictions.


Created by Mattias Jansson / Rampant Pixels - <http://www.rampantpixels.com>
