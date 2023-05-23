APP_NAME="onscripter-insani"
APP_NAME_IDENTIFIER=$(echo $APP_NAME | tr '[:upper:]' '[:lower:]')
APP_NAME_IDENTIFIER=$(echo $APP_NAME_IDENTIFIER | sed 's/[[:space:]]//g')
ARCH=$(arch)

rm -rf "$APP_NAME.app"

mkdir "$APP_NAME.app"
mkdir "$APP_NAME.app/Contents"
mkdir "$APP_NAME.app/Contents/MacOS"
mkdir "$APP_NAME.app/Contents/Resources"
sudo xattr -cr "$APP_NAME.app"

cp onscripter "$APP_NAME.app/Contents/MacOS/$APP_NAME"
cp "$APP_NAME.icns" "$APP_NAME.app/Contents/Resources/$APP_NAME.icns"

dylibbundler -od -b -x "$APP_NAME.app/Contents/MacOS/$APP_NAME" -p @executable_path/../Frameworks/ -d "$APP_NAME.app/Contents/Frameworks/"

# Look for libSDL2-2.0.0.dylib in the right place depending on whether we're on an arm64 or an x86-64 Mac
if [ -d "/opt/local/etc/macports" ]
then
    echo "Macports detected; building Macports-based universal app bundle"
else
    echo "Assuming Homebrew; building Homebrew-based thin app bundle"    
    if [[ $ARCH = "arm64" ]]
    then
        SDL2LOC=$(readlink /opt/homebrew/lib/libSDL2-2.0.0.dylib)
        cp /opt/homebrew/lib/$SDL2LOC "$APP_NAME.app/Contents/Frameworks"
    else
        SDL2LOC=$(readlink /usr/local/lib/libSDL2-2.0.0.dylib)
        cp /usr/local/lib/$SDL2LOC "$APP_NAME.app/Contents/Frameworks"
    fi
fi

echo "<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">
<plist version=\"1.0\">
<dict>
  <key>CFBundleGetInfoString</key>
  <string>$APP_NAME</string>
  <key>CFBundleExecutable</key>
  <string>$APP_NAME</string>
  <key>CFBundleIdentifier</key>
  <string>org.insani.$APP_NAME_IDENTIFIER</string>
  <key>CFBundleName</key>
  <string>$APP_NAME</string>
  <key>CFBundleIconFile</key>
  <string>$APP_NAME.icns</string>
  <key>CFBundleShortVersionString</key>
  <string>20230523</string>
  <key>CFBundleVersion</key>
  <string>FC01</string>
  <key>CFBundleInfoDictionaryVersion</key>
  <string>6.0</string>
  <key>CFBundlePackageType</key>
  <string>APPL</string>
  <key>NSHumanReadableCopyright</key>
  <string>onscripter-insani is (c) 2002-present ogapee and (c) 2005-present insani.  It is licensed under the GPLv2.</string>
</dict>
</plist>" > "$APP_NAME.app/Contents/Info.plist"

if [ -f "ONScripter.o" ]
then
    rm *.o
fi
if [ -f "onscripter" ]
then
    rm onscripter
fi
if [ -f "nsaconv" ]
then
    rm nsaconv
fi
if [ -f "nsadec" ]
then
    rm nsadec
fi
if [ -f "sarconv" ]
then
    rm sarconv
fi
if [ -f "sardec" ]
then
    rm sardec
fi

lipo -info "$APP_NAME.app/Contents/MacOS/$APP_NAME"
