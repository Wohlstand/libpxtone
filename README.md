# PXTONE

The library to play and/or edit music in PTTUNE and PTCOP file formats.

Library developed by Pixel: https://pxtone.org/developer/

# What is this?
This library is a modified version 220910a to add some new features and provide
the Big-Endian processors support (for example, PowerPC and MIPS).

# What changes being made?
## Bug fixes
* **[Fixed comparison code at the pxtnPulse_Noise::Compare](https://github.com/Wohlstand/libpxtone/commit/d0c363d1f57ef2318bb91f2f72382d2dfed44e1f)**
  * There was a bug that makes all event lists being "always-equal" because they had been compared with self.
* **[Fixed the object deletion at pxtoneNoise::init()](https://github.com/Wohlstand/libpxtone/commit/3513ff46789a873357557f6dd93f34dcc0c79611)**
  * Any objects created with `new` operator should be removed by `delete` operator, otherwise, there is a possible chance to produce a memory leak.

## Polishing
* **[Removed all trailing whitespaces](https://github.com/Wohlstand/libpxtone/commit/7cc9a465b960abc4b8d44d30c345d452c51437bb)**
  * There are spaces at end of lines: basically a useless ballast that confuses the difference and taking the file size. A lot of IDEs and text editors do have the feature of trailing spaces removal, etc.
* **Fixed warnings**
  * [One](https://github.com/Wohlstand/libpxtone/commit/f30e17df4602df032679d5a3e66d6edc8c772738)
    * Trailing comma at enumerations isn't well met by some compliers, causing warnings
    * Signed and Unsigned comparison
    * Unused arguments
    * Missing default branch at Switch things
  * [GCC gives a misleading indentation warning](https://github.com/Wohlstand/libpxtone/commit/8c71dbccf593905b9024f79024e3c3de0b66934a) when you have multiple statements after the `if` clause without adding brackets.
  * [Using safer way to cast floating-point number as unsigned integer](https://github.com/Wohlstand/libpxtone/commit/775b8e6221a14b08324e142b77da90c0dea54872)
    * Initial way gives the "type-punned pointer will break strict-aliasing rules" warning. New way uses a `union` which allows to do the same safer :smile:.

## Hardware support
* **[Replaced plain "int" with "int32_t"](https://github.com/Wohlstand/libpxtone/commit/4c2b7c2a680a150107d370fb257c6b2265aa80b7)**
  * On some toolchains, the int32_t in fact is the "long int" which leads to complie errors.
* **[Added support for Big Endian processors](https://github.com/Wohlstand/libpxtone/commit/7314fc157ad55940e40bc62ad301eb11e72dd395)**
  * Allows the library to work on processors that has the Big Endian byte order, for example, on old Macs that has PowerPC processor. You will need to specify the `-Dpx_BIG_ENDIAN` macro when compile project on the Big Endian hardware to make library work correctly.

## Features
* **[Added tempo factor parameter](https://github.com/Wohlstand/libpxtone/commit/f0b2118deda068f86edf7076ff60ab3d5fa652f8)**
  * This is a factor that can be used to change the tempo of currently playing song on the fly. This feature is used for various actions.
* **[Added an option to specify the number of loops to play](https://github.com/Wohlstand/libpxtone/commit/314a09d02ea8c7674dff73535a58ea307ba77b01)**
  * Now you can specify how many times the song should loop.
* **[Added an option to retrieve the loop start point (the "repeat")](https://github.com/Wohlstand/libpxtone/commit/e36599684189a0b23a714f936a711a102436b280)**
  * It allows to indicate the loop area at the music file during playback.

## OGG Vorbis support changes
* [Exclude static callbacks at OGG Vorbis](https://github.com/Wohlstand/libpxtone/commit/b3e130dcaf637a6d5b18ce416ed5f357a611be6d)
  * This fixes one another warning given by unused static callbacks at Vorbis headers.
* [Added Tremor support](https://github.com/Wohlstand/libpxtone/commit/d8010e7b5019865dbe0413590a432cd016f88f54)
  * This is the integer-only implementation of OGG Vorbis, designed for hardware without or with a limited FPU.
* [Added use of stb_vorbis](https://github.com/Wohlstand/libpxtone/commit/9d3ff1e96c042050420e02f890fc7e9c39c11d62)
  * This is the header-only implementation of OGG Vorbis. It can be used without having full set of OGG Vorbis libraries.
* [Added missing error code assignment](https://github.com/Wohlstand/libpxtone/commit/29365516f993fc3871aaf83dd6e9bfc1ba38305a)
  * This change fixes the "VOID" error being returned instead of the actual error reason.

Among with the `-DpxINCLUDE_OGGVORBIS`, you also can specify the `-DpxINCLUDE_OGGVORBIS_TREMOR` to use the Tremor (integer-only) implementation, or `-DpxINCLUDE_OGGVORBIS_STB` to use the header-only stb_vorbis implementation which is included with the project here.
