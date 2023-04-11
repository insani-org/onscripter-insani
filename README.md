# onscripter-insani
*A successor to the [onscripter-insani](http://nscripter.insani.org/) of old*

# Last Updated
11 April 2023

# Languages
- [English](#English)
- [Français](#Français)
- [Italiano](Italiano)

# English

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
  monospaced, using `) to work
- Provide builds for modern architectures, most notably for Apple Silicon
  macOS
- Focus on portability and portable compilation, rather than on replicating
  ONScripter-EN functionality
- Support the insani-localized novel game library for posterity

All changes made by onscripter-insani are wrapped in #if defined(INSANI) ... #endif blocks and this is done for the following reasons:

- If you compile without the INSANI flag, you'll get baseline ONScripter
- Since this is a branch, this allows us to rapidly adapt to any changes 
  Ogapee might make in the future

Should you wish to contribute code to onscripter-insani, we'd ask that you follow that convention.
  
## Required and Recommended Libraries/Utilities
- For macOS, our build system assumes that you are using [homebrew](https://brew.sh)
- For Windows, our build system assumes that you are using [MSYS2](https://msys2.org)

### Required Libraries
- libjpeg
- bzip2
- One of:
  - SDL-2.0 **and** SDL1.2-compat
  - SDL-1.2
- SDL_image
- SDL_mixer
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

```brew install jpeg jpeg-turbo bzip2 sdl2 sdl12-compat sdl_image sdl_mixer sdl_ttf smpeg libogg libvorbis make dylibbundler```

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

```pacman -Syuu```

This will likely cause MSYS2 to close.  Relaunch into the MINGW64 environment and run that command again until nothing further happens.  Then run:

```pacman -S mingw-w64-x86_64-SDL mingw-w64-x86_64-SDL_ttf mingw-w64-x86_64-SDL_mixer mingw-w64-x86_64-SDL_image mingw-w64-x86_64-bzip2 mingw-w64-x86_64-libogg mingw-w64-x86_64-libvorbis mingw-w64-x86_64-freetype mingw-w64-x86_64-smpeg mingw-w64-x86_64-iconv mingw-w64-x86_64-zlib mingw-w64-x86_64-toolchain```

## Compilation Instructions

### GNU/Linux
```make -f Makefile.Linux.insani```

On Linux and gcc, you can fairly easily make a static build of your binary by passing in the -static option to gcc in the right place in the Makefile.

### macOS
```gmake -f Makefile.MacOSX.insani```

If you wish to make a game-specific app bundle, you can do so by running ```./makedist.MacOSX.sh``` -- however, if you wish to actually distribute your 
builds, you are going to need an Apple developer account and you will have to fork over $99 a year.  Much more detailed instructions and notes can be found in the comments section of Makefile.MacOSX.insani.

### Windows
Within the MINGW64 environment of MSYS2:

```make -f Makefile.Win.insani```

The DLL dependencies for the resultant onscripter.exe are:

- libbrotlicommon.dll
- libbrotlidec.dll
- libbz2-1.dll
- libdeflate.dll
- libfreetype-6.dll
- libgcc_s_seh1.dll
- libglib-2.0.0.dll
- libgraphite2.dll
- libharfbuzz-0.dll
- libiconv-2.dll
- libintl-8.dll
- libjbig-0.dll
- libjpeg-8.dll
- libLerc.dll
- liblzma-5.dll
- libmad-0.dll
- libogg-0.dll
- libpcre2-8-0.dll
- libpng16-16.dll
- libSDL_image-1-2-0.dll
- libSDL_mixer-1-2-0.dll
- libsharpyuv0.dll
- libstdc++-6.dll
- libtiff-6.dll
- libvorbis-0.dll
- libvorbisfile-3.dll
- libwebp-7.dll
- libwinpthread-1.dll
- libzstd.dll
- SDL_ttf.dll
- SDL.dll
- zlib1.dll

This list is exhaustive and likely many of the DLLs are not needed for normal running.  All DLLs can be found at:

```C:\msys64\mingw64\bin\```

If you are distributing your build, you'll want to package up all of these DLLs alongside your onscripter.exe.

### All Other Platforms
We only provide builds for macOS and Windows, and we only do testing on Linux, macOS, and Windows; however, ONScripter itself has been successfully compiled for platforms as diverse as Android, iOS, Symbian, MacOS Classic, PSP, *BSD, and the list goes on.  However we have not modified the makefiles for these platforms, and we can't guarantee a successful compilation on any platform other than Linux, macOS, and Windows.  If you are compiling on any of those platforms, you will minimally want to make sure the -DINSANI define is present.  If you succeed on one of our non-supported platforms, please get in touch with us and contribute your modified Makefiles back.

## Why Dynamic Libraries?
Traditionally, there was a push to make sure that onscripter-insani and ONScripter-EN builds were built with static libraries, for ease of distribution among other things.  The problem is, bitrot sets in quite quickly -- and the presence of those static libraries can sometimes lead to crashes as time goes on and there are major changes in the underlying OS.

Our macOS build system depends on Homebrew, and you should never attempt to statically link Homebrew libraries as they tend to be truly OS version
dependent in a way that the dylibs are not.  Furthermore, for the purposes of macOS, you're canonically supposed to distribute the required dylibs inside the app bundle as our makedist.MacOSX.sh does.

For Windows, the MINGW64 DLLs are widely compatible with any modern version of x86-64 Windows, and in the case that they become incompatible, replacement with updated versions will be easy.

## Contacts
The original creator of ONScripter is Ogapee:

- https://onscripter.osdn.jp/onscripter.html

Please refrain from contacting Ogapee about anything you find in onscripter-insani, though, as this branch does have differences from mainline 
ONScripter.  Contact us through Github instead:

- http://nscripter.insani.org/
- https://github.com/insani-org/onscripter-insani/

## Changelog

### 20230410
- Now with 100% more insanity spirit
- Corrections to line-break system
- Corrections to behavior of ```@```, ```\```, and ```/``` in legacy english mode
- ```!s```, ```!w```, and ```#``` now work in english mode
- Initial experimental support for UTF8 (0.utf)
- French and Italian documentation

### 20230308
- ONScripter upstream version: 20220816
- Added legacy_english_mode flag
- Set english_mode and legacy_english_mode to TRUE when a line that begins with ` is detected
- Created line-break system for legacy english mode
- Fixed a bug in which lines that begin with ` cannot be advanced from in any circumstance
- Minor tweaks to allow for usage of SDL2 + SDL12-compat
- macOS:
  - APPBUNDLE build flag which by default allows for saving of .sav and envdata files in ```~/Documents/ONScripter/gamename``` instead of in the same  directory, and to allow for game asset detection within the app bundle itself
  - Modified makefile for macOS
  - Created macOS app bundle creation script
- Windows:
  - All-new makefile for Windows, based on MSYS2 rather than VC++
  - Edits to several files to allow proper MSYS2-based compilation
  - Backported in window icon feature (from 2005!)

### All Previous Versions
- http://nscripter.insani.org/changelog.html

## License
onscripter-insani is distributed under the terms of the GNU Public  License (GPL) v2.  This license does not extend to the assets for any game run
in onscripter-insani -- those rights remain with the original creators.

A copy of the GPLv2 can be found under COPYING in this source directory.

# Français

## Introduction et Histoire
onscripter-insani est une branche d'[ONScripter](https://onscripter.osdn.jp/onscripter.html), qui est un projet actif en date de 2022.  ONScripter est une implémentation libre, clone de NScripter -- un moteur de création de romans vidéoludiques qui a sans doute contribué à l'essor de ceux-ci au Japon.  De nombreuses entreprises ont utilisé NScripter pour créer certains des classiques du genre, comme par exemple 'Mizuiro' de Nekonekosoft et 'Tsukihime' de TYPE-MOON.

En 2005, un programmeur nommé chendo a apporté quelques correctifs à ONScripter qui, pour la première fois, permettait d'utiliser directement des caractères d'un octet dans ONScripter.  Nous, membres d'insani.org, avons pris en charge la maintenance d'une branche séparée d'ONScripter pour les besoins de la localisation de nouveaux jeux.  Cette branche est finalement devenue une branche à part entière appelée ONScripter-EN (ons-en), qui a été maintenue par Haeleth et Uncle Mion.  Les dernières modifications de cette branche remontent à 2011, et le projet n'a pas été activement maintenu depuis.

En février 2023, les binaires existants pour ONScripter-EN avaient les caractéristiques suivantes :

- Ils ne fonctionnent plus avec les versions modernes de macOS, probablement en raison des changements de sécurité apportés par Catalina et les versions supérieures
- De nombreux fichiers sonores ne sont pas joués du tout
- Ils contenaient un bogue sérieux dans lequel vous ne pouviez pas accéder au menu système NScripter (et donc vous n’étiez pas en mesure d’enregistrer) lorsque vous atteigniez un point de choix

De plus, les arborescences des sources d'ONScripter-EN et ONScripter sont maintenant très différents -- ce qui est normal puisqu'il y a 12 ans d'écart entre les deux projets.  Au cours de ces 12 années, ONScripter a porté en son sein de nombreuses améliorations qui se trouvaient dans ONScripter-EN, et a subi au moins une réorganisation complète de ses fichiers et de ses noms de fichiers.  Cela signifie que les comparaisons directes entre le code d'ONScripter-EN et celui d'ONScripter sont désormais très difficiles.

Ceci étant dit, depuis avril 2023, le projet ONScripter-EN est de nouveau officiellement maintenu, et nous sommes en effet impliqués dans la mise en place du système de compilation sur Windows (x86-64 et i686).  Le projet peut être trouvé ici :

- https://github.com/Galladite27/ONScripter-en
- https://github.com/insani-org/onscripter-en-msys2-configure-makefile

Cette itération d'ONScripter-EN se concentre principalement sur les fonctionnalités avancées qui ne sont pas actuellement dans ONScripter, et sur la prise en charge de Win32 (ONScripter-EN cible les machines virtuelles Windows XP 32 bits, par exemple).  Nous avons choisi de réactiver notre branche onscripter-insani, car les objectifs d'ONScripter-EN et d'onscripter-insani sont légèrement différents.  Nous considérons ONScripter-EN comme un projet frère d'onscripter-insani et collaborons étroitement avec les mainteneurs de ce projet (par exemple, nous maintenons les Makefiles Windows pour la compilation reposant sur MSYS2).  Nos objectifs pour onscripter-insani sont les suivants :

- Rester aussi proche que possible de la version amont d'ONScripter
- Inclure une fonctionalité qui permet vraiment le fonctionnement du   mode anglais pré-existant (un octet, chasse fixe, en utilisant `)
- Fournir des binaires pour les architectures modernes, plus particulièrement pour Apple Silicon macOS
- Mettre l'accent sur la portabilité et la compilation portable, plutôt que sur la réplication des fonctionnalités d'ONScripter-EN
- Prendre en charge la ludothèque de l'insani pour la postérité

Tous les changements effectués par onscripter-insani sont enveloppés dans des blocs #if defined(INSANI) ... #endif, pour les raisons suivantes :

- Si vous compilez sans le paramètre INSANI, vous obtiendrez la version de   base d'ONScripter.
- Comme il s'agit d'une branche, cela nous permet de nous adapter   rapidement à tous les changements qu'Ogapee pourrait apporter à l'avenir.

Si vous souhaitez contribuer au code d'onscripter-insani, nous vous demandons de suivre cette convention.

## Bibliothèques/Utilitaires Requis et Recommandés
- Pour macOS, le système de compilation suppose l'utilisation de [homebrew](https://brew.sh)
- Pour Windows, le système de compilation suppose l'utilisation de [MSYS2](https://msys.org)

### Bibliothèques requises
- libjpeg
- bzip2
- Votre choix de:
  - SDL-2.0 **et** SDL1.2-compat
  - SDL-1.2
- SDL_image
- SDL_mixer
- SDL_ttf

### Bibliothèques recommandées
- SMPEG
- libogg
- libvorbis
- libmad

### Utilitaires requis (macOS)
- dylibbundler
- GNU make

## Préparation de Votre Environnement de Développement et de Compilation

### GNU/Linux
Chaque distribution GNU/Linux dispose de son propre gestionnaire de paquets.  Assurez-vous simplement d'obtenir les bibliothèques nécessaires en utilisant ce gestionnaire de paquets.  Si vous êtes sur une dist. plus ou moins moderne, assurez-vous d'opter pour la SDL2 + SDL1.2-compat, par opposition à SDL1.2 pur.  Vous aurez des pannes dans le cas contraire.

### macOS
Après avoir installé [Homebrew](https://brew.sh), il suffit de faire :

```brew install jpeg jpeg-turbo bzip2 sdl2 sdl12-compat sdl_image sdl_mixer sdl_ttf smpeg libogg libvorbis make dylibbundler```

Vous devrez vous assurer que le répertoire des bibliothèques de Homebrew

- *ARM64*: ```/opt/homebrew/lib```
- *x86-64*: ```/usr/local/lib```

soit ajouté à votre variable PATH.  Il doit l'être par défaut après l'installation de Homebrew.

Tous les binaires que vous créez par cette méthode sont spécifiques à la fois à l'architecture architecture (ARM64/x86-64) ainsi qu'à la version de macOS sur laquelle vous compilez.  Nous recommandons de maintenir des machines virtuelles simplement pour pouvoir compiler vers certaines cibles :

- *ARM64* : Ventura
- *x86-64* : Catalina

Nous recommandons ces deux versions respectives de macOS parce qu'il s'agit de versions majeures qui ont apporté des changements importants en matière de sécurité -- les binaires de versions antérieures de macOS ne fonctionnent souvent pas dans ces versions, alors que les binaires qui fonctionnent dans ces versions fonctionneront dans toutes les versions ultérieures à ce jour.

### Windows
Installez [MSYS2](https://www.msys2.org) et acceptez l'emplacement d'installation par défaut:

- ```C:\msys64```

Après avoir installé MSYS2, assurez-vous que vous êtes dans l'environnement MINGW64.  Par défaut, MSYS2 se lance dans l'environnement UCRT64, soyez donc prudent. Une fois que vous êtes connecté, mettez à jour MSYS2 :

```pacman -Syuu```

Cette opération entraînera probablement la fermeture de MSYS2. Relancez dans l'environnement MINGW64 et exécutez à nouveau ceci jusqu'à ce qu'il ne se passe plus rien. Ensuite, exécutez la commande suivante :

```pacman -S mingw-w64-x86_64-SDL mingw-w64-x86_64-SDL_ttf mingw-w64-x86_64-SDL_mixer mingw-w64-x86_64-SDL_image mingw-w64-x86_64-bzip2 mingw-w64-x86_64- libogg mingw-w64-x86_64-libvorbis mingw-w64-x86_64-freetype mingw-w64-x86_64-smpeg mingw-w64-x86_64-iconv mingw-w64-x86_64-zlib mingw-w64-x86_64-toolchain```

## Instructions de Compilation

### GNU/Linux
```make -f Makefile.Linux.insani```

Sous GNU/Linux et gcc, vous pouvez assez facilement faire une compilation statique de votre binaire en passant l'option -static à gcc au bon endroit dans le Makefile.

### macOS
```gmake -f Makefile.MacOSX.insani```

Si vous souhaitez créer un paquet d'app spécifique à un jeu, vous pouvez le faire en exécutant ```./makedist.MacOSX.sh``` -- cependant, si vous souhaitez publier vos compilations, vous aurez besoin d'un compte dév. Apple et vous devrez débourser 99 $ par an.  Des instructions et des notes beaucoup plus détaillées peuvent être trouvées dans la section des commentaires de ```Makefile.MacOSX.insani```.

### Windows
Dans l'environnement MINGW64 de MSYS2 :

```make -f Makefile.Win.insani```

Les dépendances DLL pour le fichier onscripter.exe résultant sont :

- libbrotlicommon.dll
- libbrotlidec.dll
- libbz2-1.dll
- libdeflate.dll
- libfreetype-6.dll
- libgcc_s_seh1.dll
- libglib-2.0.0.dll
- libgraphite2.dll
- libharfbuzz-0.dll
- libiconv-2.dll
- libintl-8.dll
- libjbig-0.dll
- libjpeg-8.dll
- libLerc.dll
- liblzma-5.dll
- libmad-0.dll
- libogg-0.dll
- libpcre2-8-0.dll
- libpng16-16.dll
- libSDL_image-1-2-0.dll
- libSDL_mixer-1-2-0.dll
- libsharpyuv0.dll
- libstdc++-6.dll
- libtiff-6.dll
- libvorbis-0.dll
- libvorbisfile-3.dll
- libwebp-7.dll
- libwinpthread-1.dll
- libzstd.dll
- SDL_ttf.dll
- SDL.dll
- zlib1.dll

Cette liste est exhaustive et il est probable que de nombreuses DLL ne soient pas nécessaires à un fonctionnement normal. Toutes les DLL se trouvent à l'adresse suivante :

- ```C:\msys64\mingw64\bin\```

Si vous distribuez votre version, vous devriez empaqueter toutes ces DLL
avec votre onscripter.exe.

### Toutes les autres plateformes
Nous ne fournissons des compilations que pour macOS et Windows, et nous ne faisons des essais que sur GNU/Linux, macOS et Windows ; cependant, ONScripter a été compilé avec succès pour des plateformes pour des plateformes aussi diverses qu'Android, iOS, Symbian, MacOS Classic, PSP, *BSD, et bien d'autres encore.  Cependant, nous n'avons pas modifié les makefiles pour ces plateformes, et nous ne pouvons pas garantir une compilation réussie sur toute plateforme autres que GNU/Linux, macOS et Windows.  Si vous compilez sur l'une d'elles vous devrez au minimum vous assurer que la définition -DINSANI est présente.  Si vous réussissez à compiler sur une plateforme non prise en charge, prière de nous joindre et nous renvoyer vos Makefiles modifiés.

## Pourquoi des Bibliothèques Dynamiques
Traditionnellement, un effort a été fait pour s'assurer que les compilations ONScripter-insani et ONScripter-EN soient compilées avec des bibliothèques statiques, pour faciliter la distribution, entre autres. Le problème est qu'une tendance du logiciel à développer des bogues au fil du temps en raison d'incompatibilités mineures avec des systèmes d'exploitation plus récents pourrait s'installer assez rapidement -- et que la présence de ces bibliothèques statiques peut parfois conduire à des plantages au fil du temps et que le système d'exploitation sous-jacent subit des modifications importantes.

Notre système de compilation pour macOS dépend de Homebrew, et vous ne devriez jamais essayer de lier statiquement les bibliothèques Homebrew, car elles ont tendance à dépendre de la version du système d'exploitation, d'une façon que les bibliothèques dynamiques ('dylibs') ne sont pas. De plus, pour les besoins de macOS, vous êtes canoniquement supposé incorporer les bibliothèques dynamiques nécessaires à l'intérieur du paquet app comme le fait notre ```makedist.MacOSX.sh```.

Pour Windows, les DLL MINGW64 sont largement compatibles avec toutes les versions modernes de Windows x86-64, et dans le cas où elles deviendraient incompatibles, le remplacement par des versions mises à jour sera aisé.

## Joignez-Nous
Le créateur original d'ONScripter est Ogapee :

- https://onscripter.osdn.jp/onscripter.html

Veuillez vous abstenir de contacter Ogapee à propos de tout ce que vous trouverez dans onscripter-insani, car cette branche présente des différences par rapport à la version principale d'ONScripter. À la place, joignez-nous - de préférence via GitHub :

- http://nscripter.insani.org/
- https://github.com/insani-org/onscripter-insani/

## Journal des Modifications

### 20230410
- Avec 100% plus d’esprit d'insanité!
- Corrections apportées au système de sauts de ligne
- Corrections du comportement de ```@```, ```/``` et ```/``` en legacy_english_mode
- ```!s```, ```!w```, et ```#``` fonctionnent maintenant en modes anglais et UTF8
- Support expérimental initial pour UTF8 (0.utf)
- Nouvelle documentation française et italienne

### 20230308
- Version amont d'ONScripter : 20220816
- Ajout du paramètre legacy_english_mode
- Passe english_mode et legacy_english_mode à VRAI lorsqu'une ligne commençant par ` est détectée.
- Création d'un système de saut de ligne pour legacy_english_mode
- Correction d'un bogue dans lequel les lignes commençant par ` ne peuvent   être avancées dans aucune circonstance
- Ajustements mineurs pour permettre l'utilisation de SDL2 + SDL12-compat
- macOS :
  - Paramètre de compilation APPBUNDLE qui permet par défaut d'enregistrer les fichiers .sav et envdata dans ```~/Documents/ONScripter/nomdejeu``` plutôt que dans le même répertoire, et de permettre la détection des ressources du jeu dans l'app bundle lui-même
  - Modification du fichier makefile pour macOS
  - Création d'un script de création d'app bundle pour macOS
- Windows :
  - Tout nouveau makefile pour Windows, reposant sur MSYS2 plutôt que VC++
  - Modifications de plusieurs fichiers pour permettre une compilation correcte basée sur MSYS2
  - Fonctionnalité de l'icône de la fenêtre rétroportée (datant de 2005 !)

### Toutes les versions précédentes
- http://nscripter.insani.org/fr/changelog.html

## License
onscripter-insani est distribué sous les termes de la licence publique GNU
(GPL) v2.  Cette licence ne s'éend pas aux ressources d'un jeu exécuté
dans onscripter-insani -- ces droits restent la propriété des créateurs
d'origine.

Une copie de la GPLv2 ('COPYING') est présente dans ce répertoire source.

# Italiano

