# onscripter-insani
*A successor to the [onscripter-insani](http://nscripter.insani.org/) of old*

# Languages
- [English](#English)
- [Français](#Français)
- [Italiano](#Italiano)

# English

## Last Updated
2023-04-23

## Introduction and History
onscripter-insani is a branch of [ONScripter](https://onscripter.osdn.jp/onscripter.html), which is an active project as of 2022.  ONScripter is a clean-room open-source implementation of NScripter -- a novel game creation engine that arguably helped to bootstrap the novel game boom in Japan.  Many companies used NScripter in order to create some of the classics of the genre, notable examples being みずいろ (*Mizuiro*) by Nekonekosoft and 月姫 (*Tsukihime*) by TYPE-MOON.

In 2005, a programmer named chendo contributed some patches back to ONScripter which, for the first time, allowed for one-byte characters to be used directly in ONScripter.  We at insani.org took on maintenance of a separate branch of ONScripter for the purposes of novel game localization.  This branch eventually became a full-on fork called ONScripter-EN (ons-en), which was maintained by Haeleth and Uncle Mion.  The last commits to this fork date back to 2011, and the project has not been actively maintained ever since.

As of February 2023, the binaries for ONScripter-EN had the following characteristics:

- They no longer function in modern macOS, likely related to security changes 
  brought about in Catalina and beyond
- Many sound files do not play at all
- They contained a serious bug in which you could not access the NScripter system menu (and therefore you were unable to save) when you reached a choice point

Furthermore, the source trees for ONScripter-EN and ONScripter are now very different -- which is to be expected when there is a 12 year gap between the two projects.  Over the course of these 12 years, ONScripter has ported in many of the enhancements that were in ONScripter-EN, and has had at least one complete reorganization of its files and filenames.  This means that direct  comparisons between the code of ONScripter-EN and ONScripter are now very difficult.

That being said, as of April 2023, the ONScripter-EN project is once again officially maintained, and indeed we are involved with helping to provide a build system on Windows (x86-64 and i686).  The project can be found at:

- https://github.com/Galladite27/ONScripter-en
- https://github.com/insani-org/onscripter-en-msys2-configure-makefile

This iteration of ONScripter-EN largely focuses on advanced functionality not currently in ONScripter, as well as legacy Win32 (one of the stated
targets for ONScripter-EN is 32-bit Windows XP virtual machines, for instance).  We have chosen to re-activate our onscripter-insani branch, as the 
goals of ONScripter-EN and our goals are slightly different.  We consider ONScripter-EN to be a sister project to onscripter-insani and we collaborate closely with the maintainers of said project (for instance, we maintain the Windows Makefiles for MSYS2-based compilation).  Our goals for onscripter-insani are as follows:

- Stay as close to upstream ONScripter as possible
- Write in functionality that truly allows legacy English mode (one-byte, 
  monospaced, using ``` ` ```) to work
- Provide builds for modern architectures, most notably for Apple Silicon
  macOS
- Focus on portability and portable compilation, rather than on replicating
  ONScripter-EN functionality
- Support the insani-localized novel game library for posterity

All changes made by onscripter-insani are wrapped in ```#if defined(INSANI) ... #endif``` blocks and this is done for the following reasons:

- If you compile without the ```-DINSANI``` flag, you'll get baseline ONScripter
- Since this is a branch, this allows us to rapidly adapt to any changes 
  Ogapee might make in the future

Should you wish to contribute code to onscripter-insani, we'd ask that you follow that convention.
  
## Required and Recommended Libraries/Utilities
- For macOS, our build system assumes that you are using [Homebrew](https://brew.sh)
- For Windows, our build system assumes that you are using [MSYS2](https://msys2.org)

### Required Libraries
- libjpeg
- bzip2
- One of:
  - SDL-2.0 **and** SDL1.2-compat
  - SDL-1.2
- SDL_image
- SDL_mixer
  - For MP3 support on Homebrew and MSYS2, you will need to recompile this library, as detailed [here](#sdl_mixer-and-mp3-playback)
- SDL_ttf

### Recommended Libraries
- SMPEG
- libogg
- libvorbis
- libmad

### Required Utilities (macOS)
- dylibbundler
- GNU make

## Preparing Your Development and Build Environment

### GNU/Linux
Every Linux distribution will have its own package manager.  Simply make sure you get the required libraries using that package manager.  If you are on a decently modern Linux distribution version, then make sure that you go the SDL2 + SDL1.2-compat route, as opposed to the pure SDL1.2 route.  You will get breakage otherwise.

### macOS
After you have installed [Homebrew](https://brew.sh), simply do:

```
brew install jpeg jpeg-turbo bzip2 sdl2 sdl12-compat sdl_image sdl_mixer sdl_ttf smpeg libogg libvorbis mad make dylibbundler
```

You will want to make sure that Homebrew's libraries directory:

- *Apple Silicon*: ```/opt/homebrew/lib```
- *Intel*: ```/usr/local/lib```

is added to your PATH variable.  It should be added by default after Homebrew is installed.

All binaries that you create through this methodology are specific to both the architecture (ARM64/x86-64) as well as the macOS version that you're compiling on.  We recommend maintaining virtual machines simply so you can build to certain targets:

- *Apple Silicon*: Ventura
- *Intel*: Catalina

We recommend these two respective macOS versions because they are major breakpoints that brought about significant security changes -- binaries from
prior versions of macOS often don't work in these versions, whereas binaries that do work in these versions will work in all subsequent versions to date.

### Windows

Install [MSYS2](https://www.msys2.org) and accept the default install location:

```C:\msys64```

After you have installed MSYS2, make sure that you are logging into the MINGW64 environment.  By default MSYS2 launches into the UCRT64 environment, so be careful.  Once you are in, update MSYS2:

```
pacman -Syuu
```

This will likely cause MSYS2 to close.  Relaunch into the MINGW64 environment and run that command again until nothing further happens.  Then run:

```
pacman -S mingw-w64-x86_64-SDL mingw-w64-x86_64-SDL_ttf mingw-w64-x86_64-SDL_mixer mingw-w64-x86_64-SDL_image mingw-w64-x86_64-bzip2 mingw-w64-x86_64-libogg mingw-w64-x86_64-libvorbis mingw-w64-x86_64-freetype mingw-w64-x86_64-smpeg mingw-w64-x86_64-iconv mingw-w64-x86_64-zlib mingw-w64-x86_64-toolchain
```

## Compilation Instructions

### GNU/Linux
```
make -f Makefile.Linux.insani
```

On Linux and gcc, you can fairly easily make a static build of your binary by passing in the ```-static``` option to gcc in the right place(s) in the Makefile.  You might be interested in [this](https://github.com/insani-org/onscripter-en-msys2-configure-makefile) for a working example of a Makefile  (for our sister project) that results in a statically-compiled portable binary (albeit for Windows; and no, onscripter-insani will not compile with this Makefile so please do not try).

### macOS
```
gmake -f Makefile.MacOSX.insani
```

If you wish to make a game-specific app bundle, you can do so by running ```./makedist.MacOSX.sh``` -- however, if you wish to actually distribute your 
builds, you are going to need an Apple developer account and you will have to fork over $99 a year.  Much more detailed instructions and notes can be found in the comments section of ```Makefile.MacOSX.insani```.

### Windows
Within the MINGW64 environment of MSYS2:

```
make -f Makefile.Win.insani
```

The DLL dependencies for the resultant onscripter.exe are:

```
libbrotlicommon.dll
libbrotlidec.dll
libbz2-1.dll
libdeflate.dll
libfreetype-6.dll
libgcc_s_seh1.dll
libglib-2.0.0.dll
libgraphite2.dll
libharfbuzz-0.dll
libiconv-2.dll
libintl-8.dll
libjbig-0.dll
libjpeg-8.dll
libLerc.dll
liblzma-5.dll
libmad-0.dll
libogg-0.dll
libpcre2-8-0.dll
libpng16-16.dll
libSDL_image-1-2-0.dll
libSDL_mixer-1-2-0.dll
libsharpyuv0.dll
libstdc++-6.dll
libtiff-6.dll
libvorbis-0.dll
libvorbisfile-3.dll
libwebp-7.dll
libwinpthread-1.dll
libzstd.dll
SDL_ttf.dll
SDL_mixer.dll
SDL.dll
zlib1.dll
```

This list is exhaustive and likely many of the DLLs are not needed for normal running.  All DLLs can be found at:

```C:\msys64\mingw64\bin\```

If you are distributing your build, you'll want to package up all of these DLLs alongside your onscripter.exe.

### All Other Platforms
We only provide builds for macOS and Windows, and we only do testing on Linux, macOS, and Windows; however, ONScripter itself has been successfully compiled for platforms as diverse as Android, iOS, Symbian, MacOS Classic, PSP, *BSD, and the list goes on.  However we have not modified the makefiles for these platforms, and we can't guarantee a successful compilation on any platform other than Linux, macOS, and Windows.  If you are compiling on any of those platforms, you will minimally want to make sure the ```-DINSANI``` define is present.  If you succeed on one of our non-supported platforms, please get in touch with us and contribute your modified Makefiles back.

## Why Dynamic Libraries?
Traditionally, there was a push to make sure that onscripter-insani and ONScripter-EN builds were built with static libraries, for ease of distribution among other things.  The problem is, bitrot sets in quite quickly -- and the presence of those static libraries can sometimes lead to crashes as time goes on and there are major changes in the underlying OS.

Our macOS build system depends on Homebrew, and you should never attempt to statically link Homebrew libraries as they tend to be truly OS version
dependent in a way that the dylibs are not.  Furthermore, for the purposes of macOS, you're canonically supposed to distribute the required dylibs inside the app bundle as our ```makedist.MacOSX.sh``` does.

For Windows, the MINGW64 DLLs are widely compatible with any modern version of x86-64 Windows, and in the case that they become incompatible, replacement with updated versions will be easy.

## Features and Errata

### Features

#### UTF8 Mode
onscripter-insani has a fully-supported, backward-compatible UTF8 Mode.  It will automatically enter UTF8 mode if you have a ```0.utf``` or a ```0.utf.txt``` instead of a ```0.txt```.

#### Fonts
onscripter-insani relies upon having a default font file available to it.  Historically, this had to be named ```default.ttf``` and placed in the same directory as the onscripter-insani executable or, in the case of macOS App Bundles, the Contents/Resources subdirectory of the App Bundle in question.  We now provide support for OpenType and for OpenType/TrueType font collections, so onscripter-insani now looks for, in order:

- ```default.ttf```
- ```default.ttc```
- ```default.otf```
- ```default.otc```

If none of these are found, onscripter-insani will attempt the following fallback fonts:

- *macOS:* [Hiragino Maru Gothic](https://en.wikipedia.org/wiki/Hiragino)
- *Windows:* [MS Gothic](https://learn.microsoft.com/en-us/typography/font-list/ms-gothic)

These two fonts are pre-installed on their respective operating systems.  MS Gothic has come pre-installed on Windows since Vista, and Hiragino Maru Gothic has come pre-installed on macOS since macOS was a wee little thing named MacOS X, and also code-named after big cats at that.

For *maximum compatibility with Japanese language games*, you will want a default font file that is (a) monospaced and (b) capable of CJK glyphs.  Our traditional recommendations are:

- [Genjyuu Gothic X Monospace/Monospace Bold](http://jikasei.me/font/genjyuu/)
- [Migu 2M/2M Bold](http://mix-mplus-ipa.osdn.jp/)
- [Sazanami Gothic](https://osdn.net/projects/efont/releases/)

However, onscripter-insani has production UTF8 and proportional font support -- meaning that you can use just about any font with your translation project so long as it has the necessary glyphs.  For instance, an English-language translation project will generally only need a font that covers the Latin-1 spectrum (which is most fonts in existence).  A sampling of proportional fonts known to work well with onscripter-insani in English includes:

- [The DejaVu Family](https://dejavu-fonts.github.io/)
- [The Noto Family](https://fonts.google.com/noto)
- [Comic Sans](https://en.wikipedia.org/wiki/Comic_Sans)

##### Bold and Italics
If you wish to enable these text styles, you may use the following syntax within text:

```
`This is ~b~bold~b~; this is ~i~italics~i~; this is ~bi~bold italics~bi~.
```

Font style resets at the end of every script line; that is to say, whether a line terminates with an ```@```, a ```\```, a ```/```, or even nothing at all, the font style will reset.  If you want to have multiple full pages that are styled bold, for instance, you will have to put in the ```~b~``` style tag at the beginning of every line.  That being said, we encourage that you treat these style tags the same way you would treat HTML tags -- if you opened one, make sure you close it at the end of the line.  While a line that goes:

```
`~i~This line is entirely in italics!\
```

will work just fine, it is better to write:

```
`~i~This line is entirely in italics!~i~\
```

as this will help give your script maximum compatibility across NScripter-derived engines that utilize this font styling syntax.

All other instances of ```~``` are treated like normal printing characters.  This is explicitly only enabled for UTF8 mode at this time.  Faux bold and faux italics styles are provided out-of-the-box if you only specify a single default font file.  We also allow you to specify true bold/italics/bold italics styles as different fonts at startup as follows:

- ```default.ttf/.otf/.ttc/.otc```: normal style
- ```default-b.ttf/.otf/.ttc/.otc```: bold style
- ```default-i.ttf/.otf/.ttc/.otc```: italics style
- ```default-bi.ttf/.otf/.ttc/.otc```: bold italics style

All of the above will have to have the same file extension (all ```.ttf``` for instance).  If you specify a true bold/italics/bold italics font file, the corresponding faux will be turned off accordingly -- for instance, if you have a ```default.ttf``` and a ```default-i.ttf``` but nothing else, faux italics will be off, but faux bold and faux bold italics will remain on.  This is in support of cases where you simply wish to use a different font rather than using the actual bold/italics/bold italics style of the same font.

We have also added experimental support of underlining (```~u~```, predictably), but this support is highly experimental and we do not recommend that you use it at this time.

### Errata

#### UTF8 vs. SHIFT_JIS Encoded Files
Most of the files in this project are encoded as UTF8.  However, there are several files in particular that are encoded as SHIFT_JIS, those being:

- ```ONScripter_rmenu.cpp```
- ```ONScripter_text.cpp```
- ```ScriptParser.h```
- ```ScriptParser.cpp```

That is because these files have several strings that *must be in their original SHIFT_JIS format* unless you want to cause random memory corruption-related crashes whenever you are in UTF8 mode.  The reason behind this issue is a fascinating artifact of a time when ONScripter was in its infancy, and cared about no other character encoding other than SHIFT_JIS (or, more accurately, Windows CP932, a very close relative of SHIFT_JIS).  ONScripter initializes several subsystems, including what is known as the kinsoku process, *during initialization of the engine itself*, before any script file is read.  Despite the fact that UTF8 support has been introduced over the years, ONScripter is fundamentally not a Unicode-aware program by design, and it shows in cases like this.  There is no way, with this design, for the engine to know whether it's in CP932 or UTF8 mode -- and so it assumes CP932, and assumes all strings are CP932-encoded for the purposes of initialization.

All this to say: if you want to contribute to onscripter-insani, **make extra sure that these files are encoded as SHIFT_JIS and not UTF8**.  You will cause random crashes all over the place, usually upon startup in UTF8 mode, if you do not.

#### SDL_mixer and MP3 Playback
The default precompiled libraries for SDL_mixer on both MSYS2 and Homebrew do not enable support for MP3 playback.  If you want MP3 playback, you are going to have to compile your own SDL_mixer as an end result.  Instructions below:

##### macOS
The easiest way to achieve a redistributable SDL_mixer 1.2.12 with MP3 playback enabled on Homebrew is to edit the Formula for SDL_mixer.  Do as follows:

```
brew tap homebrew/core
EDITOR=nano brew edit sdl_mixer
```

This will open up a text editor with the sdl_mixer Homebrew Formula opened up.  A few important things to note:

In the section that reads

```
  depends_on "pkg-config" => :build
  depends_on "flac"
  depends_on "libmikmod"
  depends_on "libogg"
  depends_on "libvorbis"
  depends_on "sdl12-compat"
```

add

```
  depends_on "smpeg"
  depends_on "libmad"
```

under ```depends_on "sdl12-compat"```.

In the section that reads

```
    args = %W[
      --prefix=#{prefix}
      --disable-dependency-tracking
      --enable-music-ogg
      --enable-music-flac
      --disable-music-ogg-shared
      --disable-music-mod-shared
    ]
```

add

```
      --enable-music-mp3
      --disable-music-mp3-shared
      --disable-music-fluidsynth-midi
```

under ```--disable-music-mod-shared``` but before ```]```.

Now that you have made those edits, hit ```CTRL-O``` to save, then hit ```Enter```.  Finally, hit ```CTRL-X``` to finish.  All this done, run the command

```
HOMEBREW_NO_INSTALL_FROM_API=1 brew reinstall --build-from-source sdl_mixer
```

This should rebuild SDL_mixer with MP3 support, and put it in the right place.

As a final note, it's been known for the source code download to fail.  You may have to use [this URL](https://downloads.sourceforge.net/project/libsdl/SDL_mixer/1.2.12/SDL_mixer-1.2.12.tar.gz?ts=gAAAAABkOLOQCC9jEAqgEZajvI2a4ok_TF2WDqrk-rDCCOnmq8w2pI0vwLB3egonw2e-xodO02o2vCytb1anEdOZOVBY1Z7DKg%3D%3D) instead of the default listed URL in the Formula.

##### Windows
Download the [SDL_mixer 1.2.12 source zip](https://sourceforge.net/projects/libsdl/files/SDL_mixer/1.2.12/SDL_mixer-1.2.12.zip/download), and unzip it into your MSYS2 home folder (```C:\msys64\home\username```).  Open up the MINGW64 environment of MSYS2, go to the ```SDL_mixer-1.2.12``` directory, then run:

```
./configure
make
```

Once this is done, you can navigate into the ```build``` subdirectory and find ```SDL_mixer.dll```.  This is the DLL you should be packaging alongside your onscripter-insani redistributable.  There is no need to recompile onscripter-insani; simply use this DLL and MP3 playback will automagically work.

#### The Behavior of ```@```
In NScripter, the presence of an ```@``` in a text block triggers what is known as the clickwait state.  In this state, the program waits for you to click before advancing the text any further.  It's often used to create a deliberate pause for dramatic or comedic effect.  The way ```@``` is supposed to work, if you had a line that goes

```
`I@ am@ waiting@ after@ every@ word.\
```

you would expect it to wait after every word.  Unfortunately, there is currently a bug that causes this to not work.  In the meantime, you can work around this by using the ```/``` ignore newline text block command:

```
`I @/
` am@/
` waiting@/
` after@/
` every@/
` word.\
```

## Contacts
The original creator of ONScripter is Ogapee:

- https://onscripter.osdn.jp/onscripter.html

Please refrain from contacting Ogapee about anything you find in onscripter-insani, though, as this branch does have differences from mainline 
ONScripter.  Contact us through Github instead:

- [http://nscripter.insani.org/](http://nscripter.insani.org/)
- https://github.com/insani-org/onscripter-insani/

## Changelog
*For more detailed release notes, please go [here](https://github.com/insani-org/onscripter-insani/releases).*

### 20230420 "Capcom"
#### All
- New UTF8 mode
  - onscripter-insani now accepts UTF8-encoded script files; the script file must be named ```0.utf``` (not ```0.utf.txt```) for autodetection
- Proportional font support in UTF8 mode
- Completely new pixel-based text wrapping system
  - Appropriately ignores the length of inline text commands, unlike the previous system (```!s```, ```!sd```, ```!w```, ```!d```, ```#nnnnnn```, ```~i~```, ```~b~```, ```~bi~```)
  - Truly universal; works with both monospaced and proportional fonts, in both UTF8 and SHIFT_JIS modes
  - Has special rules for emdashes, horizontal rules, and French spacing for guillemets in UTF8 mode
  - Nonbreaking spaces (U+00A0) honored in UTF8 mode
- Faux styles (bold and italics) for fonts
  - Use ```~i~```, ```~b~```, and ```~bi~``` as inline text commands; the first instance enables that style, and the next disables it
    - ```This is ~i~italics~i~ and this is ~b~bold~b~ and this is ~bi~bold italics~bi~```
- Expanded font file compatibility; onscripter-insani now accepts OpenType fonts, TrueType collections, and OpenType collections as follows:
  - ```default.ttf```
  - ```default.ttc```
  - ```default.otf```
  - ```default.otc```
#### macOS
- If no ```default.ttf/.ttc/.otf/.otc``` is found, fallback to system Hiragino Maru Gothic
- Recompiled SDL_mixer with ```--disable-music-mp3-shared```, making MP3 playback work in macOS app bundles *for real this time around*
  - Updated ```README.md``` section on [SDL_mixer and MP3 playback](https://github.com/insani-org/onscripter-insani#sdl_mixer-and-mp3-playback) with updated build instructions
- ```makedist.MacOSX.sh``` updated to 20230420
#### Windows
- If no ```default.ttf/.ttc/.otf/.otc``` is found, fallback to system MS Gothic

### 20230413-1 "BaseSon"
#### All
- Now with 100% more MP3 playback capability
  - ```README.md``` now has information about compiling your own SDL_mixer with MP3 support
#### macOS
- ```makedist.MacOSX.sh``` updated to 20230413-1

### 20230413 "Baldr"
#### All
- Now with 100% more insanity spirit
- Corrections to line-break system
- Corrections to behavior of ```@```, ```\```, and ```/``` in ```legacy_english_mode```
- ```!s```, ```!w```, and ```#``` now work in ```english_mode``` and ```legacy_english_mode```
- Initial support for UTF8 (```0.utf```) script files
- French and Italian documentation

### 20230308 "August"
#### ONScripter Upstream Version
- [20220816](https://onscripter.osdn.jp/onscripter-20220816.tar.gz)
#### All
- Added ```legacy_english_mode``` flag
- Set ```english_mode``` and ```legacy_english_mode``` to ```TRUE``` when a line that begins with ``` ` ``` is detected
- Created line-break system for ```legacy_english_mode```
- Fixed a bug in which lines that begin with ``` ` ``` cannot be advanced from in any circumstance
- Minor tweaks to allow for usage of SDL2 + SDL12-compat
#### macOS
- ```-DAPPBUNDLE``` build flag which by default allows for saving of ```.sav``` and ```envdata``` files in ```~/Documents/ONScripter/gamename``` instead of in the same  directory, and to allow for game asset detection within the app bundle itself
- Modified makefile for macOS
- Created macOS app bundle creation script
#### Windows
- All-new makefile for Windows, based on MSYS2 rather than VC++
- Edits to several files to allow proper MSYS2-based compilation
- Backported in window icon feature (from 2005!)

### All Previous Versions
- [http://nscripter.insani.org/changelog.html](http://nscripter.insani.org/changelog.html)

## License
onscripter-insani is distributed under the terms of the GNU Public  License (GPL) v2.  This license does not extend to the assets for any game run
in onscripter-insani -- those rights remain with the original creators.

A copy of the GPLv2 can be found under ```COPYING``` in this source directory.

# Français

## Dernière mise à jour
2023-04-13

## Introduction et histoire
onscripter-insani est une branche d'[ONScripter](https://onscripter.osdn.jp/onscripter.html), qui est un projet actif en date de 2022.  ONScripter est une implémentation libre, clone de NScripter -- un moteur de création de romans vidéoludiques qui a sans doute contribué à l'essor de ceux-ci au Japon.  De nombreuses entreprises ont utilisé NScripter pour créer certains des classiques du genre, comme par exemple *Mizuiro* de Nekonekosoft et *Tsukihime* de TYPE-MOON.

En 2005, un programmeur nommé chendo a apporté quelques correctifs à ONScripter qui, pour la première fois, permettait d'utiliser directement des caractères d'un octet dans ONScripter.  Nous, membres d'insani.org, avons pris en charge la maintenance d'une branche séparée d'ONScripter pour les besoins de la localisation de nouveaux jeux.  Cette branche est finalement devenue une branche à part entière appelée ONScripter-EN (ons-en), qui a été maintenue par Haeleth et Uncle Mion.  Les dernières modifications de cette branche remontent à 2011, et le projet n'a pas été activement maintenu depuis.

En février 2023, les binaires existants pour ONScripter-EN avaient les caractéristiques suivantes :

- Ils ne fonctionnent plus avec les versions modernes de macOS, probablement en raison des changements de sécurité apportés par Catalina et les versions supérieures
- De nombreux fichiers sonores ne sont pas joués du tout
- Ils contenaient un bogue sérieux dans lequel vous ne pouviez pas accéder au menu système (et donc vous n'étiez pas en mesure d'enregistrer votre progression) lorsque vous atteigniez un choix

De plus, les arborescences des sources d'ONScripter-EN et ONScripter sont maintenant très différents -- ce qui est normal puisqu'il y a 12 ans d'écart entre les deux projets.  Au cours de ces 12 années, ONScripter a porté en son sein de nombreuses améliorations qui se trouvaient dans ONScripter-EN, et a subi au moins une réorganisation complète de ses fichiers et de ses noms de fichiers.  Cela signifie que les comparaisons directes entre le code d'ONScripter-EN et celui d'ONScripter sont désormais très difficiles.

Ceci étant dit, depuis avril 2023, le projet ONScripter-EN est de nouveau officiellement maintenu, et nous sommes en effet impliqués dans la mise en place du système de compilation sur Windows (x86-64 et i686).  Le projet peut être trouvé ici :

- https://github.com/Galladite27/ONScripter-en
- https://github.com/insani-org/onscripter-en-msys2-configure-makefile

Cette itération d'ONScripter-EN se concentre principalement sur les fonctionnalités avancées qui ne sont pas actuellement dans ONScripter, et sur la prise en charge de Win32 (ONScripter-EN cible les machines virtuelles Windows XP 32 bits, par exemple).  Nous avons choisi de réactiver notre branche onscripter-insani, car les objectifs d'ONScripter-EN et d'onscripter-insani sont légèrement différents.  Nous considérons ONScripter-EN comme un projet frère d'onscripter-insani et collaborons étroitement avec les mainteneurs de ce projet (par exemple, nous maintenons les Makefiles Windows pour la compilation reposant sur MSYS2).  Nos objectifs pour onscripter-insani sont les suivants :

- Rester aussi proche que possible de la version amont d'ONScripter
- Inclure une fonctionalité qui permet vraiment le fonctionnement du mode anglais pré-existant (un octet, chasse fixe, en utilisant ``` ` ```)
- Fournir des binaires pour les architectures modernes, plus particulièrement pour Apple Silicon macOS
- Mettre l'accent sur la portabilité et la compilation portable, plutôt que sur la réplication des fonctionnalités d'ONScripter-EN
- Prendre en charge la ludothèque de l'insani pour la postérité

Tous les changements effectués par onscripter-insani sont enveloppés dans des blocs ```#if defined(INSANI) ... #endif```, pour les raisons suivantes :

- Si vous compilez sans le paramètre ```-DINSANI```, vous obtiendrez la version de  base d'ONScripter.
- Comme il s'agit d'une branche, cela nous permet de nous adapter  rapidement à tous les changements qu'Ogapee pourrait apporter à l'avenir.

Si vous souhaitez contribuer au code d'onscripter-insani, nous vous demandons de suivre cette convention.

## Bibliothèques/Utilitaires requis et recommandés
- Pour macOS, le système de compilation suppose l'utilisation de [Homebrew](https://brew.sh)
- Pour Windows, le système de compilation suppose l'utilisation de [MSYS2](https://msys.org)

### Bibliothèques requises
- libjpeg
- bzip2
- Au choix :
  - SDL-2.0 **et** SDL1.2-compat
  - SDL-1.2
- SDL_image
- SDL_mixer
  - Pour la prise en charge des MP3 sur Homebrew et MSYS2, vous devrez recompiler cette bibliothèque, comme indiqué [ici](#sdl_mixer-et-lecture-de-mp3)
- SDL_ttf

### Bibliothèques recommandées
- SMPEG
- libogg
- libvorbis
- libmad

### Utilitaires requis (macOS)
- dylibbundler
- GNU make

## Préparation de votre environnement de développement et de compilation

### GNU/Linux
Chaque distribution GNU/Linux dispose de son propre gestionnaire de paquets.  Assurez-vous simplement d'obtenir les bibliothèques nécessaires en utilisant ce gestionnaire de paquets.  Si vous êtes sur une dist. plus ou moins moderne, assurez-vous d'opter pour la SDL2 + SDL1.2-compat, par opposition à SDL1.2 pur.  Vous aurez des pannes dans le cas contraire.

### macOS
Après avoir installé [Homebrew](https://brew.sh), il suffit de faire :

```
brew install jpeg jpeg-turbo bzip2 sdl2 sdl12-compat sdl_image sdl_mixer sdl_ttf smpeg libogg libvorbis make dylibbundler
```

Vous devrez vous assurer que le répertoire des bibliothèques de Homebrew

- *ARM64*: ```/opt/homebrew/lib```
- *x86-64*: ```/usr/local/lib```

soit ajouté à votre variable PATH.  Il doit l'être par défaut après l'installation de Homebrew.

Tous les binaires que vous créez par cette méthode sont spécifiques à la fois à l'architecture architecture (ARM64/x86-64) ainsi qu'à la version de macOS sur laquelle vous compilez.  Nous recommandons de maintenir des machines virtuelles simplement pour pouvoir compiler vers certaines cibles :

- *ARM64* : Ventura
- *x86-64* : Catalina

Nous recommandons ces deux versions respectives de macOS parce qu'il s'agit de versions majeures qui ont apporté des changements importants en matière de sécurité -- les binaires de versions antérieures de macOS ne fonctionnent souvent pas dans ces versions, alors que les binaires qui fonctionnent dans ces versions fonctionneront dans toutes les versions ultérieures à ce jour.

### Windows
Procédez à l'installation de [MSYS2](https://www.msys2.org) et acceptez l'emplacement d'installation par défaut :

- ```C:\msys64```

Après avoir installé MSYS2, assurez-vous que vous êtes dans l'environnement MINGW64.  Par défaut, MSYS2 se lance dans l'environnement UCRT64, soyez donc prudent. Une fois que vous êtes connecté, mettez à jour MSYS2 :

```
pacman -Syuu
```

Cette opération entraînera probablement la fermeture de MSYS2. Relancez dans l'environnement MINGW64 et exécutez à nouveau ceci jusqu'à ce qu'il ne se passe plus rien. Ensuite, exécutez la commande suivante :

```
pacman -S mingw-w64-x86_64-SDL mingw-w64-x86_64-SDL_ttf mingw-w64-x86_64-SDL_mixer mingw-w64-x86_64-SDL_image mingw-w64-x86_64-bzip2 mingw-w64-x86_64- libogg mingw-w64-x86_64-libvorbis mingw-w64-x86_64-freetype mingw-w64-x86_64-smpeg mingw-w64-x86_64-iconv mingw-w64-x86_64-zlib mingw-w64-x86_64-toolchain
```

## Instructions de compilation

### GNU/Linux
```
make -f Makefile.Linux.insani
```

Sous GNU/Linux et gcc, vous pouvez assez facilement faire une compilation statique de votre binaire en passant l'option ```-static``` à gcc au bon endroit dans le Makefile.  Vous pourriez être intéressé par [ceci](https://github.com/insani-org/onscripter-en-msys2-configure-makefile) pour un exemple de Makefile (pour notre projet jumeau) qui résulte en un binaire portable compilé statiquement - bien que pour Windows ; et non, onscripter-insani ne se compilera pas avec ce Makefile donc n'essayez pas.

### macOS
```
gmake -f Makefile.MacOSX.insani
```

Si vous souhaitez créer un app bundle spécifique à un jeu, vous pouvez le faire en exécutant ```./makedist.MacOSX.sh``` -- cependant, si vous souhaitez publier vos compilations, vous aurez besoin d'un compte dév. Apple et vous devrez débourser 99 $ par an.  Des instructions et des notes beaucoup plus détaillées peuvent être trouvées dans la section des commentaires de ```Makefile.MacOSX.insani```.

### Windows
Dans l'environnement MINGW64 de MSYS2 :

```
make -f Makefile.Win.insani
```

Les dépendances DLL pour le fichier onscripter.exe résultant sont :

```
libbrotlicommon.dll
libbrotlidec.dll
libbz2-1.dll
libdeflate.dll
libfreetype-6.dll
libgcc_s_seh1.dll
libglib-2.0.0.dll
libgraphite2.dll
libharfbuzz-0.dll
libiconv-2.dll
libintl-8.dll
libjbig-0.dll
libjpeg-8.dll
libLerc.dll
liblzma-5.dll
libmad-0.dll
libogg-0.dll
libpcre2-8-0.dll
libpng16-16.dll
libSDL_image-1-2-0.dll
libSDL_mixer-1-2-0.dll
libsharpyuv0.dll
libstdc++-6.dll
libtiff-6.dll
libvorbis-0.dll
libvorbisfile-3.dll
libwebp-7.dll
libwinpthread-1.dll
libzstd.dll
SDL_ttf.dll
SDL_mixer.dll
SDL.dll
zlib1.dll
```

Cette liste est exhaustive et il est probable que de nombreuses DLL ne soient pas nécessaires à un fonctionnement normal. Toutes les DLL se trouvent au chemin suivant :

- ```C:\msys64\mingw64\bin\```

Si vous distribuez votre version, vous devriez empaqueter toutes ces DLL avec votre onscripter.exe.

### Toutes les autres plateformes
Nous ne fournissons des compilations que pour macOS et Windows, et nous ne faisons des essais que sur GNU/Linux, macOS et Windows ; cependant, ONScripter a été compilé avec succès pour des plateformes pour des plateformes aussi diverses qu'Android, iOS, Symbian, MacOS Classic, PSP, *BSD, et bien d'autres encore.  Cependant, nous n'avons pas modifié les makefiles pour ces plateformes, et nous ne pouvons pas garantir une compilation réussie sur toute plateforme autres que GNU/Linux, macOS et Windows.  Si vous compilez sur l'une d'elles vous devrez au minimum vous assurer que la définition ```-DINSANI``` est présente.  Si vous réussissez à compiler sur une plateforme non prise en charge, prière de nous joindre et nous renvoyer vos Makefiles modifiés.

## Pourquoi des bibliothèques dynamiques
Traditionnellement, un effort a été fait pour s'assurer que les compilations ONScripter-insani et ONScripter-EN soient compilées avec des bibliothèques statiques, pour faciliter la distribution, entre autres. Le problème est qu'une tendance du logiciel à développer des bogues au fil du temps en raison d'incompatibilités mineures avec des systèmes d'exploitation plus récents pourrait s'installer assez rapidement -- et que la présence de ces bibliothèques statiques peut parfois conduire à des plantages au fil du temps et que le système d'exploitation sous-jacent subit des modifications importantes.

Notre système de compilation pour macOS dépend de Homebrew, et vous ne devriez jamais essayer de lier statiquement les bibliothèques Homebrew, car elles ont tendance à dépendre de la version du système d'exploitation, d'une façon que les bibliothèques dynamiques ('dylibs') ne sont pas. De plus, pour les besoins de macOS, vous êtes canoniquement supposé incorporer les bibliothèques dynamiques nécessaires à l'intérieur du paquet app comme le fait notre ```makedist.MacOSX.sh```.

Pour Windows, les DLL MINGW64 sont largement compatibles avec toutes les versions modernes de Windows x86-64, et dans le cas où elles deviendraient incompatibles, le remplacement par des versions mises à jour sera aisé.

## Errata et curiosités

### Fichiers UTF8 vs. SHIFT_JIS
La plupart des fichiers de ce projet sont en UTF8.  Cependant, trois fichiers en particulier sont en SHIFT_JIS :

- ```ONScripter_text.cpp```
- ```ScriptParser.h```
- ```ScriptParser.cpp```

C'est parce que ces trois fichiers contiennent plusieurs chaînes de caractères qui *doivent être dans leur format SHIFT_JIS* d'origine, à moins que vous ne vouliez provoquer des plantages fortuits liés à la corruption de la mémoire chaque fois que vous êtes en mode UTF8.  La raison derrière ce problème est un artefact fascinant de l'époque où ONScripter en était à ses balbutiements, et ne se souciait d'aucun autre encodage de caractères que SHIFT_JIS (ou, plus précisément, Windows CP932, un parent très proche de SHIFT_JIS).  ONScripter initialise plusieurs sous-systèmes, y compris ce qui est connu sous le nom de processus kinsoku, *pendant l'initialisation du moteur lui-même*, avant que tout fichier script ne soit lu.  Malgré le fait que le support de l'UTF8 ait été introduit au fil des ans, ONScripter n'est fondamentalement pas un programme compatible avec l'Unicode, et cela se voit dans des cas comme celui-ci.  Il n'y a aucun moyen, avec cette conception, pour le moteur de savoir s'il est en mode CP932 ou UTF8 -- et donc il suppose CP932, et suppose que toutes les chaînes sont codées en CP932 pour les besoins de l'initialisation.

Tout ceci pour dire : si vous voulez contribuer à onscripter-insani, **assurez vous que ces fichiers sont encodés en SHIFT_JIS et non en UTF8**.  Si vous ne le faites pas, vous provoquerez des plantages fortuits un peu partout, généralement au démarrage en mode UTF8.

### SDL_mixer et lecture de MP3
Les bibliothèques précompilées par défaut pour SDL_mixer sur MSYS2 et Homebrew ne permettent pas la lecture de MP3.  Si vous souhaitez lire des MP3, vous devrez compiler votre propre SDL_mixer.  Instructions ci-dessous :

#### macOS
La façon la plus simple d'obtenir un SDL_mixer 1.2.12 redistribuable avec la lecture de MP3 possible sur Homebrew est de modifier la formule pour SDL_mixer.  Procédez comme suit :

```
brew tap homebrew/core
EDITOR=nano brew edit sdl_mixer
```

Ceci ouvrira un logiciel de modification de texte avec la formule Homebrew sdl_mixer.  Quelques points importants à noter :

Dans la section suivante

```
  depends_on "pkg-config" => :build
  depends_on "flac"
  depends_on "libmikmod"
  depends_on "libogg"
  depends_on "libvorbis"
  depends_on "sdl12-compat"
```

ajoutez

```
  depends_on "smpeg"
  depends_on "libmad"
```

sous ```depends_on "sdl12-compat"```.

Dans la section suivante

```
    args = %W[
      --prefix=#{prefix}
      --disable-dependency-tracking
      --enable-music-ogg
      --enable-music-flac
      --disable-music-ogg-shared
      --disable-music-mod-shared
    ]
```

ajoutez

```
      --enable-music-mp3
      --disable-music-fluidsynth-midi
```

sous ```--disable-music-mod-shared``` mais avant ```]```.

Maintenant que vous avez fait ces modifications, tapez ```CTRL-O``` pour sauvegarder, puis tapez ```Entrée```.  Enfin, tapez ```CTRL-X``` pour terminer.  Une fois tout cela fait, lancez la commande

```
HOMEBREW_NO_INSTALL_FROM_API=1 brew reinstall --build-from-source sdl_mixer
```

Cela devrait permettre de recompiler SDL_mixer avec la prise en charge des MP3, et de le placer au bon endroit.

Enfin, il est arrivé que le téléchargement du code source échoue.  Il se peut que vous deviez utiliser [cette URL](https://downloads.sourceforge.net/project/libsdl/SDL_mixer/1.2.12/SDL_mixer-1.2.12.tar.gz?ts=gAAAAABkOLOQCC9jEAqgEZajvI2a4ok_TF2WDqrk-rDCCOnmq8w2pI0vwLB3egonw2e-xodO02o2vCytb1anEdOZOVBY1Z7DKg%3D%3D) au lieu de l'URL par défaut dans la formule.

#### Windows
Téléchargez l'[archive zip source de SDL_mixer 1.2.12](https://sourceforge.net/projects/libsdl/files/SDL_mixer/1.2.12/SDL_mixer-1.2.12.zip/download), et décompressez-la dans votre répertoire personnel de MSYS2 (```C:\msys64\home\votrenom```).  Ouvrez l'environnement MINGW64 de MSYS2, allez dans le répertoire ```SDL_mixer-1.2.12```, puis lancez :

```
./configure
make
```

Une fois cela fait, vous pouvez naviguer dans le sous-répertoire ```build``` et trouver ```SDL_mixer.dll```.  C'est la DLL que vous devez empaqueter avec votre redistribuable onscripter-insani.  Il n'est pas nécessaire de recompiler onscripter-insani ; utilisez simplement cette DLL et la lecture des MP3 fonctionnera automatiquement.

### Le comportement de ```@```
Dans NScripter, la présence d'un ```@``` dans un bloc de texte déclenche ce que l'on appelle l'état clickwait - attente de clic.  Dans cet état, le programme attend que vous cliquiez avant de faire avancer le texte.  Il est souvent utilisé pour créer une pause délibérée à des fins dramatiques ou comiques.  De la manière dont ```@``` devrait fonctionner, si vous avez une ligne ressemblant à

```
`Je@ suis@ en@ train@ d'attendre@ après@ chaque@ mot.\
```

on pourrait s'attendre à ce que le logiciel attende après chaque mot.  Malheureusement, il y a actuellement un bogue qui fait que cela ne fonctionne pas.  En attendant, vous pouvez contourner le problème en utilisant la commande ```/``` ignorant le saut de ligne :

```
`Je @/
` suis@/
` en@/
` train@/
` d'attendre@/
` après@/
` chaque@/
` mot.\
```

## Joignez-nous
Le créateur original d'ONScripter est Ogapee :

- https://onscripter.osdn.jp/onscripter.html

Veuillez vous abstenir de contacter Ogapee à propos de tout ce que vous trouverez dans onscripter-insani, car cette branche présente des différences par rapport à la version principale d'ONScripter. À la place, joignez-nous - de préférence via GitHub :

- http://nscripter.insani.org/
- https://github.com/insani-org/onscripter-insani/

## Journal des modifications
*Pour des notes de mise à jour plus détaillées, veuillez vous rendre [ici](https://github.com/insani-org/onscripter-insani/releases).*

### 20230413-1 'BaseSon'
#### Tous
- Avec 100 % plus de capacité à lire des MP3
  - ```README.md``` contient maintenant des informations sur la compilation de votre propre SDL_mixer avec prise en charge des MP3
#### macOS
- ```makedist.MacOSX.sh``` mis à jour vers 20230413-1

### 20230413 'Baldr'
#### Tous
- Avec 100 % plus d’esprit d'insanité
- Corrections apportées au système de sauts de ligne
- Corrections du comportement de ```@```, ```\``` et ```/``` en ``legacy_english_mode```
- ```!s```, ```!w```, et ```#``` fonctionnent maintenant en ```english_mode``` et ```legacy_english_mode```
- Prise en charge expérimentale initiale pour UTF8 (```0.utf```)
- Documentation française et italienne

### 20230308 'August'
#### Tous
- Version amont d'ONScripter : 20220816
- Ajout du paramètre ```legacy_english_mode```
- Passe ```english_mode``` et ```legacy_english_mode``` à ```VRAI``` lorsqu'une ligne commençant par ` est détectée.
- Création d'un système de saut de ligne pour ```legacy_english_mode```
- Correction d'un bogue dans lequel les lignes commençant par ``` ` ``` ne peuvent être avancées dans aucune circonstance
- Ajustements mineurs pour permettre l'utilisation de SDL2 + SDL12-compat
#### macOS
- Paramètre de compilation ```-DAPPBUNDLE``` qui permet par défaut d'enregistrer les fichiers ```.sav``` et ```envdata``` dans ```~/Documents/ONScripter/nomdujeu``` plutôt que dans le même répertoire, et de permettre la détection des ressources du jeu dans l'app bundle lui-même
- Modification du fichier makefile pour macOS
- Création d'un script de création d'app bundle pour macOS
#### Windows
- Tout nouveau makefile pour Windows, reposant sur MSYS2 plutôt que VC++
- Modifications de plusieurs fichiers pour permettre une compilation correcte avec MSYS2
- Fonctionnalité de l'icône de la fenêtre rétroportée (datant de 2005 !)

### Toutes les versions précédentes
- http://nscripter.insani.org/fr/changelog.html

## Licence
onscripter-insani est distribué sous les termes de la licence publique GNU
(GPL) v2.  Cette licence ne s'étend pas aux ressources d'un jeu exécuté
dans onscripter-insani -- ces droits restent la propriété des créateurs
d'origine.

Une copie de la GPLv2 ('```COPYING```') est présente dans ce répertoire source.

# Italiano

## Ultimo aggiornamento
2023-04-13

## Sulla traduzione

Questa traduzione non è stata effettuata da una persona di lingua natale italiana ma da un amatore francofono.  Tuttavia, è stata corretta da >zero< persone italofone con maggiore esperienza. Questa traduzione si rivolge a più persone e usa il pronome personale «voi».  Vedere la sezione RAGGIUNGERCI per l'indirizzo della persona responsabile del progetto in caso di errore.

## Introduzione e storia
onscripter-insani è una diramazione di [ONScripter](https://onscripter.osdn.jp/onscripter.html), che è un progetto attivo ancora nel 2022.  ONScripter è un'implementazione libera di NScripter, un motore per la creazione di romanzi videoludici che probabilmente ha contribuito a dare il via al fenomeno dei romanzi videoludici in Giappone.  Molte aziende hanno utilizzato NScripter per creare alcuni dei classici del genere, notevoli esempi sono *Mizuiro* di Nekonekosoft e *Tsukihime* di TYPE-MOON.

Nel 2005, un programmatore chiamato chendo contribuì con alcune modifiche a ONScripter che, per la prima volta, permettevano di utilizzare direttamente i caratteri a un byte in ONScripter.  Noi, membri di insani.org, ci siamo occupati della manutenzione di una diramazione separata di ONScripter per la localizzazione di nuovi giochi.  Questa diramazione è stata poi trasformata in una vera e propria versione derivata chiamata ONScripter-EN (ons-en), mantenuta da Haeleth e Uncle Mion.  Gli ultimi commit di questa versione sono risalgono al 2011 e da allora il progetto non è più stato mantenuto attivamente.

Alla data del febbraio 2023, i binari per ONScripter-EN avevano le seguenti caratteristiche:

- Non funzionano più nei macOS moderni, probabilmente a causa di modifiche in materia di sicurezza apportate da Catalina e versioni successive
- Molti file audio non vengono riprodotti
- Contenevano un grave baco per cui non era possibile accedere al menu di sistema (e quindi non era possibile il salvataggio dei progressi) quando la scelta veniva raggiunta

Inoltre, gli alberi dei sorgenti di ONScripter-EN e ONScripter sono ora molto diversi -- il che è normale dato che c'è un divario di 12 anni tra i due progetti.  In questi 12 anni, ONScripter ha apportato molti dei miglioramenti che erano presenti in ONScripter-EN, e ha subito almeno una riorganizzazione completa dei file e dei nomi dei file.  Ciò significa che il confronto diretto tra il codice di ONScripter-EN e quello di ONScripter è ora molto difficile.

Detto questo, a partire dall'aprile 2023, il progetto ONScripter-EN è di nuovo ufficialmente mantenuto e siamo coinvolti nel fornire un sistema di compilazione su Windows (x86-64 e i686).  Il progetto è disponibile qui:

- https://github.com/Galladite27/ONScripter-en
- https://github.com/insani-org/onscripter-en-msys2-configure-makefile

Questa iterazione di ONScripter-EN si concentra principalmente su funzionalità avanzate non presenti attualmente in ONScripter e sul supporto Win32 (ONScripter-EN mira alle macchine virtuali Windows XP a 32 bit, per esempio). Abbiamo scelto di riattivare la nostra diramazione ONScripter-insani, poiché gli obiettivi di ONScripter-EN e ONScripter-insani sono leggermente diversi. Consideriamo ONScripter-EN come un progetto gemello di ONScripter-insani e lavoriamo a stretto contatto con i manutentori di tale progetto (per esempio, manteniamo i Makefile di Windows per la compilazione tramite MSYS2).  I nostri obiettivi per ONScripter-insani sono i seguenti:

- Rimanere il più vicino possibile all'ONScripter ascendente
- Includere una funzionalità che permetta davvero la modalità inglese preesistente (un byte, spaziatura fissa, usando ``` ` ```) per funzionare
- Fornire binari per le architetture moderne, in particolare per Apple Silicon macOS
- Concentrarsi sulla portabilità e sulla compilazione portatile, piuttosto che sulla replica delle funzionalità di ONScripter-EN
- Supportare la ludoteca dell'insani per posterità

Tutte le modifiche apportate da ONScripter-insani sono racchiuse in blocchi ```#if defined(INSANI) ... #endif```, per i seguenti motivi:

- Se compilate senza il parametro ```-DINSANI```, otterrete la versione di base di ONScripter
- Poiché si tratta di una diramazione, questo ci permette di adattarci rapidamente a qualsiasi cambiamento che Ogapee potrebbe apportare in futuro.

Se volete contribuire al codice di onscripter-insani, vi chiediamo di seguire questa convenzione.

## Biblioteche/Utilità richieste e raccomandate
- Per macOS, il sistema di compilazione utilizza [homebrew](https://brew.sh)
- Per Windows, il sistema di compilazione utilizza [MSYS2](https://msys.org)

### Biblioteche richieste
- libjpeg
- bzip2
- Scelta tra:
  - SDL-2.0 **e** SDL1.2-compat
  - SDL-1.2
- SDL_image
- SDL_mixer
- SDL_ttf

### Biblioteche raccomandate
- SMPEG
- libogg
- libvorbis
- libmad

### Utilità richieste (macOS)
- dylibbundler
- GNU make

## Preparazione dell'ambiente di sviluppo e di compilazione

### GNU/Linux
Ogni distribuzione GNU/Linux ha il proprio gestore di pacchetti.  Assicuratevi semplicemente di ottenere le biblioteche necessarie usando quel gestore di pacchetti.  Se si utilizza una distribuzione GNU/Linux più o meno moderna, assicuratevi di utilizzare SDL2 + SDL1.2-compat, invece di utilizzare SDL1.2 puro.  Altrimenti otterrete delle rotture.

### macOS
Dopo aver installato [Homebrew](https://brew.sh), è sufficiente fare:

```
brew install jpeg jpeg-turbo bzip2 sdl2 sdl12-compat sdl_image sdl_mixer sdl_ttf smpeg libogg libvorbis make dylibbundler
```

È necessario assicurarsi che la cartella delle biblioteche di Homebrew

- *ARM64*: ```/opt/homebrew/lib```
- *x86-64*: ```/usr/local/lib```

sia aggiunta alla variabile PATH.  Dovrebbe essere aggiunta per impostazione predefinita dopo l'installazione di Homebrew.

Tutti i binari creati con questa metodologia sono specifici sia per l'architettura (ARM64/x86-64) che per la versione di macOS in cui vengono compilati. Si consiglia di mantenere delle macchine virtuali semplicemente per poter compilare su determinate piattaforme:

- *ARM64* : Ventura
- *x86-64* : Catalina

Raccomandiamo queste due rispettive versioni di macOS perché sono versioni importanti che hanno apportato modifiche significative in materia di sicurezza -- i binari delle versioni precedenti di macOS spesso non funzionano in queste versioni, mentre i binari che funzionano in queste versioni funzioneranno in tutte le versioni successive fino a oggi.

### Windows
Procedete all'installazione di [MSYS2](https://www.msys2.org) e accettate il percorso di installazione predefinito:

- ```C:\msys64```

Dopo aver installato MSYS2, assicurarsi di essere accedere all'ambiente MINGW64.  Per impostazione predefinita, MSYS2 viene lanciato nell'ambiente UCRT64, quindi fate attenzione. Una volta entrati, aggiornate MSYS2:

```
pacman -Syuu
```

Questo probabilmente causerà la chiusura di MSYS2.  Rilanciare l'ambiente MINGW64 ed eseguire di nuovo il comando finché non succede più nulla. Quindi eseguire:

```
pacman -S mingw-w64-x86_64-SDL mingw-w64-x86_64-SDL_ttf mingw-w64-x86_64-SDL_mixer mingw-w64-x86_64-SDL_image mingw-w64-x86_64-bzip2 mingw-w64-x86_64- libogg mingw-w64-x86_64-libvorbis mingw-w64-x86_64-freetype mingw-w64-x86_64-smpeg mingw-w64-x86_64-iconv mingw-w64-x86_64-zlib mingw-w64-x86_64-toolchain
```

## Istruzioni di compilazione

### GNU/Linux
```
make -f Makefile.Linux.insani
```

Su Linux e gcc, è possibile creare abbastanza facilmente una compilazione statica del proprio binario passando l'opzione ```-static``` a gcc nel posto giusto del Makefile.

### macOS
```
gmake -f Makefile.MacOSX.insani
```

Se volete creare un app bundle specifico per un gioco, potete farlo eseguendo ```./makedist.MacOSX.sh```; tuttavia, se volete diffondere effettivamente le vostre compilazioni, avrete bisogno di un conto sviluppatore Apple e dovrete sborsare 99 dollari all'anno.  Istruzioni e note molto più dettagliate si trovano nella sezione commenti di ```Makefile.MacOSX.insani```.

### Windows
Nell'ambiente MINGW64 di MSYS2:

```
make -f Makefile.Win.insani
```

Le dipendenze DLL per il programma onscripter.exe risultante sono:

```
libbrotlicommon.dll
libbrotlidec.dll
libbz2-1.dll
libdeflate.dll
libfreetype-6.dll
libgcc_s_seh1.dll
libglib-2.0.0.dll
libgraphite2.dll
libharfbuzz-0.dll
libiconv-2.dll
libintl-8.dll
libjbig-0.dll
libjpeg-8.dll
libLerc.dll
liblzma-5.dll
libmad-0.dll
libogg-0.dll
libpcre2-8-0.dll
libpng16-16.dll
libSDL_image-1-2-0.dll
libSDL_mixer-1-2-0.dll
libsharpyuv0.dll
libstdc++-6.dll
libtiff-6.dll
libvorbis-0.dll
libvorbisfile-3.dll
libwebp-7.dll
libwinpthread-1.dll
libzstd.dll
SDL_ttf.dll
SDL_mixer.dll
SDL.dll
zlib1.dll
```

Questa lista è esaustiva e probabilmente molte DLL non sono necessarie per il funzionamento normale.  Tutte le DLL si trovano in:

- ```C:\msys64\mingw64\bin\```

Se distribuite la propria versione, è consigliabile impacchettare tutte queste DLL insieme al programma onscripter.exe.

### Tutte le altre piattaforme
Forniamo solo versioni per macOS e Windows e facciamo prove solo su GNU/Linux, macOS e Windows; tuttavia, ONScripter stesso è stato compilato con successo per piattaforme diverse come Android, iOS, Symbian, MacOS Classic, PSP, *BSD e la lista continua.  Tuttavia, non abbiamo modificato i makefile per queste piattaforme e non possiamo garantire il successo della compilazione su piattaforme diverse da GNU/Linux, macOS e Windows.  Se compilate su una di queste piattaforme, dovete assicurarvi che la definizione ```-DINSANI``` sia presente. Se riuscite a compilare su una delle nostre piattaforme non supportate, vi preghiamo di mettervi in comunicazione con noi e di contribuire con i vostri Makefile modificati.

## Perché delle biblioteche dinamiche
Tradizionalmente, si è cercato di garantire che le compilazioni di ONScripter-insani e ONScripter-EN fossero compilate con biblioteche statiche, tra l'altro per facilitare la distribuzione.  Il problema è che la tendenza del programma a sviluppare bachi nel corso del tempo a causa di piccole incompatibilità con i sistemi operativi più recenti potrebbe insorgere abbastanza rapidamente, e che la presenza di queste librerie statiche può talvolta portare a blocchi nel corso del tempo, quando il sistema operativo sottostante subisce cambiamenti significativi.

Il nostro sistema di compilazione per macOS dipende da Homebrew e non dovreste mai cercare di collegare staticamente le biblioteche di Homebrew, in quanto tendono a dipendere dalla versione del sistema del sistema operativo, come non lo sono i dylibs.  Inoltre, per quanto riguarda macOS, canonicamente dovreste distribuire i dylibs richiesti all'interno del app bundle come fa il nostro ```makedist.MacOSX.sh```.

Per Windows, le DLL MINGW64 sono ampiamente compatibili con qualsiasi versione moderna di Windows x86-64 e, nel caso in cui dovessero diventare incompatibili, la sostituzione con le versioni aggiornate sarà facile.

## Raggiungerci
Il creatore d'ONScripter è Ogapee:

- https://onscripter.osdn.jp/onscripter.html

Vi preghiamo di non contattare Ogapee per qualsiasi cosa troviate in onscripter-insani tuttavia, poiché questa diramazione presenta delle differenze rispetto alla versione principale di ONScripter.  Rivolgetevi invece a noi, preferibilmente tramite GitHub:

- http://nscripter.insani.org/
- https://github.com/insani-org/onscripter-insani/

## Registro delle modifiche

### 20230413
#### Tutti
- Con il 100 % di spirito di insanità in più
- Correzioni al sistema di rottura di riga
- Corretto il comportamento di ```@```, ```\``` e ```/``` in ```legacy_english_mode```
- ```!s```, ```!w```, e ```#``` ora funzionano in ```english_mode``` e ```legacy_english_mode```
- Supporto sperimentale iniziale per UTF8 (```0.utf```)
- Documentazione in francese e italiano

### 20230308
#### Tutti
- Versione ascendente di ONScripter: 20220816
- Aggiunto il parametro ```legacy_english_mode```
- Imposta ```english_mode``` e ```legacy_english_mode``` su ```VERO``` quando viene rilevata una riga che inizia con ``` ` ```
- Creato sistema di rottura di riga per ```legacy_english_mode```
- È stato risolto un baco per il quale le righe che iniziano con ``` ` ``` non possono essere avanzate in qualsiasi circostanza
- Modifiche minori per permettere l'uso di SDL2 + SDL12-compat
#### macOS:
- Parametro di compilazione ```-DAPPBUNDLE``` che, per impostazione predefinita, salva i dati ```.sav``` e ```envdata``` in ```~/Documents/ONScripter/nomedelgioco``` piuttosto che nella stessa cartella, e per rendere possibile il rilevamento delle risorse del gioco nell'app bundle stesso
- Makefile modificato per macOS
- Creato lo script per la creazione di app bundle per macOS
#### Windows:
- Nuovo makefile per Windows, utilizzato per MSYS2 anziché per VC++
- Alcune modifiche per permettere una corretta compilazione mediante MSYS2
- Funzionalità dell'icona della finestra retroportata (dal 2005!)

### Tutte le versioni precedenti
- http://nscripter.insani.org/changelog.html

## Licenza
onscripter-insani è diffuso secondo i termini della licenza pubblica GNU (GPL)
v2.  Questa licenza non si estende alle risorse per qualsiasi gioco eseguito in
ONScripter-insani - tali diritti rimangono ai creatori originali.

Una copia della GPLv2 ('```COPYING```') è presente in questa cartella dei sorgenti.
