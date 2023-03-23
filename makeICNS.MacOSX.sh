ICNS="onscripter-insani.iconset"
ICNS_OUT="onscripter-insani.icns"
mkdir $ICNS
cp icon.png icon_512x512@2x.png
convert icon.png -resize 512x512 icon_512x512.png
cp icon_512x512.png icon_256x256@2x.png
convert icon.png -resize 256x256 icon_256x256.png
cp icon_256x256.png icon_128x128@2x.png
convert icon.png -resize 128x128 icon_128x128.png
convert icon.png -resize 64x64 icon_32x32@2x.png
convert icon.png -resize 32x32 icon_32x32.png
cp icon_32x32.png icon_16x16@2x.png
convert icon.png -resize 16x16 icon_16x16.png
mv icon_*.png $ICNS
iconutil -c icns -o $ICNS_OUT $ICNS
rm -rf $ICNS
