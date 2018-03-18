# xtd_uc
Extended Standard Template Library for micro controllers that currently don't have a C++ standard library.

The xtd µc library provides implementations of parts of the C++ standard with some additions and modifications that are suitable for micro controller applications. The code provided focuses on small code size and low RAM usage, to achieve this some sacrifices are made:

* Locales are not a thing, you get LOCALE_C, this simplifies formatting code.
* Error checking and rounding modes on some math functions is ignored.
* Sizes of containers may be limited to significantly less than you're used to (255 elements on AVR).

# Usage
Get the source, add the `xtd_uc/include/` folder to your include path and add the `.cpp` files from `xtd_uc/src` to your build.
