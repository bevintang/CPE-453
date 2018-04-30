#!/bin/bash
# Script to build avr-gcc compiler
#

INSTALL_PREFIX="$HOME/avr_files"
export LD_LIBRARY_PATH=$INSTALL_PREFIX/lib:$LD_LIBRARY_PATH
export PATH=$INSTALL_PREFIX/bin:$PATH

if [[ "$OSTYPE" == "linux-gnu" ]]; then
    sudo apt install m4 byacc flex make gcc g++ bison unxz
fi

#build avr binutils
wget https://mirrors.tripadvisor.com/gnu/binutils/binutils-2.30.tar.xz
unxz binutils-2.30.tar.xz
tar -xf binutils-2.30.tar
rm binutils-2.30.tar
cd binutils-2.30
./configure --prefix=$INSTALL_PREFIX --target=avr --disable-nls
make
make install
cd ..
rm -f -r binutils-2.30

#get libgmp source
wget https://gmplib.org/download/gmp/gmp-6.1.2.tar.xz
unxz gmp-6.1.2.tar.xz
tar -xf gmp-6.1.2.tar
rm gmp-6.1.2.tar
cd gmp-6.1.2
./configure --prefix=$INSTALL_PREFIX
make
make install
cd ..
rm -f -r gmp-6.1.2

#get libmpfr source
wget http://www.mpfr.org/mpfr-current/mpfr-4.0.1.tar.xz
unxz mpfr-4.0.1.tar.xz
tar -xf mpfr-4.0.1.tar
rm mpfr-4.0.1.tar
cd mpfr-4.0.1
./configure --prefix=$INSTALL_PREFIX --with-gmp-dir=$INSTALL_PREFIX --with-gmp-include=$INSTALL_PREFIX/include --with-gmp-lib=$INSTALL_PREFIX/lib
make
make install
cd ..
rm -f -r mpfr-4.0.1

#get libmpc
wget https://ftp.gnu.org/gnu/mpc/mpc-1.1.0.tar.gz
gunzip mpc-1.1.0.tar.gz
tar -xf mpc-1.1.0.tar
rm mpc-1.1.0.tar
cd mpc-1.1.0
./configure --prefix=$INSTALL_PREFIX --with-gmp-dir=$INSTALL_PREFIX --with-gmp-include=$INSTALL_PREFIX/include --with-gmp-lib=$INSTALL_PREFIX/lib
make
make install
cd ..
rm -f -r mpc-1.1.0

#get gcc source
wget http://www.netgull.com/gcc/releases/gcc-7.3.0/gcc-7.3.0.tar.xz
unxz gcc-7.3.0.tar.xz
tar -xf gcc-7.3.0.tar
rm gcc-7.3.0.tar
cd gcc-7.3.0
mkdir build
cd build
../configure --prefix=$INSTALL_PREFIX --target=avr --enable-languages=c,c++ --disable-nls --disable-libssp --with-dwarf2 --with-gmp=$INSTALL_PREFIX --with-mpc=$INSTALL_PREFIX --with-mpfr=$INSTALL_PREFIX
make
make install
cd ../..
rm -f -r gcc-7.3.0

#get avr-libc source
wget http://download.savannah.gnu.org/releases/avr-libc/avr-libc-2.0.0.tar.bz2
bunzip2 avr-libc-2.0.0.tar.bz2
tar -xf avr-libc-2.0.0.tar
rm avr-libc-2.0.0.tar
cd avr-libc-2.0.0
./configure --prefix=$INSTALL_PREFIX --build=`./config.guess` --host=avr
make
make install
cd ..
rm -f -r avr-libc-2.0.0

#avrdude
wget http://download.savannah.gnu.org/releases/avrdude/avrdude-6.3.tar.gz
gunzip -f avrdude-6.3.tar.gz
tar -xf avrdude-6.3.tar
rm avrdude-6.3.tar
cd avrdude-6.3
mkdir build
cd build
../configure --prefix=$INSTALL_PREFIX
make
make install
cd ../..
rm -f -r avrdude-6.3

#remove all directories