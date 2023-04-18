#!/bin/bash

APP_NAME="onscripter-insani"

rm -rf "$APP_NAME.app"

mkdir "$APP_NAME.app"
mkdir "$APP_NAME.app/Contents"
mkdir "$APP_NAME.app/Contents/MacOS"
mkdir "$APP_NAME.app/Contents/Resources"
sudo xattr -cr "$APP_NAME.app"

cp onscripter "$APP_NAME.app/Contents/MacOS/$APP_NAME"
cp "$APP_NAME.icns" "$APP_NAME.app/Contents/Resources/$APP_NAME.icns"

dylibbundler -od -b -x "$APP_NAME.app/Contents/MacOS/$APP_NAME" -p @executable_path/../Frameworks/ -d "$APP_NAME.app/Contents/Frameworks/"

# Change this to wherever the absolute path of the homebrew lbSDL2-2.0.0.dylib is -- it'll be in /opt or in /usr/local usually
cp /opt/homebrew/Cellar/sdl2/2.26.5/lib/libSDL2-2.0.0.dylib "$APP_NAME.app/Contents/Frameworks"
#cp /usr/local/Cellar/sdl2/2.26.5/lib/libSDL2-2.0.0.dylib "$APP_NAME.app/Contents/Frameworks"

echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">
<plist version=\"1.0\">
<dict>
  <key>CFBundleGetInfoString</key>
  <string>$APP_NAME</string>
  <key>CFBundleExecutable</key>
  <string>$APP_NAME</string>
  <key>CFBundleIdentifier</key>
  <string>org.insani.$APP_NAME</string>
  <key>CFBundleName</key>
  <string>$APP_NAME</string>
  <key>CFBundleIconFile</key>
  <string>$APP_NAME.icns</string>
  <key>CFBundleShortVersionString</key>
  <string>20230420</string>
  <key>CFBundleVersion</key>
  <string>Capcom</string>
  <key>CFBundleInfoDictionaryVersion</key>
  <string>6.0</string>
  <key>CFBundlePackageType</key>
  <string>APPL</string>
  <key>NSHumanReadableCopyright</key>
  <string>onscripter-insani is (c) 2002-present ogapee and (c) 2005-present insani.  It is licensed under the GPLv2.</string>
</dict>
</plist>" > "$APP_NAME.app/Contents/Info.plist"

rm *.o
rm onscripter
rm nsaconv
rm nsadec
rm sarconv
rm sardec
