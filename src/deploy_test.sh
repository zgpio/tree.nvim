BUILD=~/Documents/nvim.cpp/build
rm -rf $BUILD/test.app
mkdir -p $BUILD/test.app/Contents/Macos
cp $BUILD/test $BUILD/test.app/Contents/Macos/
/usr/local/opt/qt/bin/macdeployqt  $BUILD/test.app/ -no-plugins
otool -L $BUILD/test.app/Contents/Macos/test
tree $BUILD/test.app
du -sh $BUILD/test.app
rm $BUILD/test-release-macos.tar.gz
tar -zcvf $BUILD/test-release-macos.tar.gz -C $BUILD test.app
du -sh $BUILD/test-release-macos.tar.gz
