export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

mkdir ~/loxtemp/
cd ~/loxtemp/

wget https://ftp.gnu.org/gnu/binutils/binutils-2.32.tar.gz
tar -xvzf binutils-2.32.tar.gz

wget https://ftp.gnu.org/gnu/gcc/gcc-9.1.0/gcc-9.1.0.tar.gz
tar -xvzf gcc-9.1.0.tar.gz

rm *.tar.gz

currPath=$(pwd)
binDir=$(ls -d binutils*)
gccDir=$(ls -d gcc*)

mkdir build-binutils
cd build-binutils
echo $currPath/$binDir
$currPath/$binDir/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make
make install

cd ../

which -- $TARGET-as || echo $TARGET-as is not in the PATH

mkdir build-gcc
cd build-gcc
$currPath/$gccDir/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc

echo "PATH=\"\$HOME/opt/cross/bin:\$PATH\"" >> ~/.profile

cd ~
rm -rf loxtemp
