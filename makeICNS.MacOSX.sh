# -- makeICNS.MacOSX.sh --
#
# A handy script to generate ICNS icon sets.  Requires the
# XCode command line utilities and ImageMagick.  Assumes
# homebrew and that you've installed ImageMagick via:
#
# brew install ImageMagick
#
# Requires a square PNG of any resolution, although 1024x1024
# is optimal.  Remember that these days, macOS icons are
# rounded rects ... it is likely good to follow that
# convention until the next time Cupertino decides on a major
# UI shift.
#
NAME="onscripter-insani"
ICNS="$NAME.iconset"
ICNS_OUT="$NAME.icns"
mkdir "$ICNS"
convert "$NAME.png" -resize 1024x1024 icon_512x512@2x.png
convert "$NAME.png" -resize 512x512 icon_512x512.png
cp icon_512x512.png icon_256x256@2x.png
convert "$NAME.png" -resize 256x256 icon_256x256.png
cp icon_256x256.png icon_128x128@2x.png
convert "$NAME.png" -resize 128x128 icon_128x128.png
convert "$NAME.png" -resize 64x64 icon_32x32@2x.png
convert "$NAME.png" -resize 32x32 icon_32x32.png
cp icon_32x32.png icon_16x16@2x.png
convert "$NAME.png" -resize 16x16 icon_16x16.png
mv icon_*.png "$ICNS"
iconutil -c icns -o "$ICNS_OUT" "$ICNS"
rm -rf "$ICNS"
