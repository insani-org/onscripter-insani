make -f Makefile.macports.x86_64 clean && make -f Makefile.macports.x86_64
mv onscripter onscripter.x86_64
make -f Makefile.macports.arm64 clean && make -f Makefile.macports.arm64
mv onscripter onscripter.arm64
make -f Makefile.macports.arm64 clean
lipo onscripter.x86_64 onscripter.arm64 -create -output onscripter
rm onscripter.x86_64
rm onscripter.arm64
file onscripter
