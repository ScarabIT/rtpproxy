#!/bin/sh

set -e

BASEDIR="`dirname "${0}"`/.."
. "${BASEDIR}/scripts/functions.sub"

TTYPE="${1}"
BCG729_VER=1.1.1
SNDFILE_VER=1.0.28

uname -a
which ${CC}
${CC} --version
python3 --version
automake --version
autoconf --version
autoreconf --version

sudo iptables -L OUTPUT
sudo iptables -L INPUT
sudo sh -c 'echo 0 > /proc/sys/net/ipv6/conf/all/disable_ipv6'
echo -n "/proc/sys/kernel/core_pattern: "
cat /proc/sys/kernel/core_pattern

ALLCLEAN_TGT="all clean distclean"

if [ "${TTYPE}" = "cleanbuild" ]
then
  ./configure ${CONFIGURE_ARGS}
  exec make ${ALLCLEAN_TGT}
fi

${APT_GET} install -y libgsm1-dev
mkdir deps
cd deps
wget -O bcg729-${BCG729_VER}.tar.gz \
  https://github.com/BelledonneCommunications/bcg729/archive/${BCG729_VER}.tar.gz
tar xfz bcg729-${BCG729_VER}.tar.gz
cd bcg729-${BCG729_VER}
touch ChangeLog NEWS AUTHORS
perl -pi -e 's|bcg729.spec||g' configure.ac
./autogen.sh
./configure
make
sudo make install
cd ..
git clone https://github.com/sippy/libg722 libg722
cd libg722
make
sudo make install
cd ..
git clone https://github.com/cisco/libsrtp.git
cd libsrtp
./configure
make
sudo make install
cd ..
wget http://www.mega-nerd.com/libsndfile/files/libsndfile-${SNDFILE_VER}.tar.gz
tar xfz libsndfile-${SNDFILE_VER}.tar.gz
cd libsndfile-${SNDFILE_VER}
./configure
make
sudo make install
cd ../..

sudo ldconfig

autoreconf --force --install --verbose

if [ "${TTYPE}" = "depsbuild" ]
then
  ./configure ${CONFIGURE_ARGS}
  make ${ALLCLEAN_TGT}
  ${APT_GET} install -y libsrtp0-dev
  ./configure ${CONFIGURE_ARGS}
  exec make ${ALLCLEAN_TGT}
fi

CONFIGURE_ARGS="${CONFIGURE_ARGS} --enable-coverage"
case ${TTYPE} in
basic)
  CONFIGURE_ARGS="${CONFIGURE_ARGS} --enable-basic-tests"
  ;;

glitching)
  CONFIGURE_ARGS="${CONFIGURE_ARGS} --disable-basic-tests --enable-memglitching"
  ;;

*)
  echo "Unknown or undefined TTYPE" >&2
  exit 1
  ;;
esac

./configure ${CONFIGURE_ARGS}
make clean all

ELP_BRANCH="${ELP_BRANCH:-"master"}"
cd deps
git clone --branch ${ELP_BRANCH} https://github.com/sobomax/libelperiodic.git
cd libelperiodic
./configure
make all
sudo make install
python3 setup.py build
sudo python3 setup.py install
cd ../..

sudo ldconfig

${APT_GET} install -y libpcap-dev cmake
git clone -b master https://github.com/sippy/udpreplay.git dist/udpreplay
mkdir dist/udpreplay/build
cmake -Bdist/udpreplay/build -Hdist/udpreplay
make -C dist/udpreplay/build all
sudo make -C dist/udpreplay/build install

${APT_GET} install -y tcpdump curl wireshark-common gdb tcpreplay
tcpdump --version || true
#launchpad fails#${APT_GET} install -y --reinstall ca-certificates
#launchpad fails#sudo add-apt-repository ppa:jonathonf/ffmpeg-4 -y
#launchpad fails#${APT_GET} update
#launchpad fails#${APT_GET} install -y ffmpeg
wget -O dist/ffmpeg.tar.xz https://johnvansickle.com/ffmpeg/releases/ffmpeg-release-i686-static.tar.xz
tar -C dist -xvf dist/ffmpeg.tar.xz
sudo cp dist/ffmpeg-*-i686-static/ffmpeg /usr/bin

TEST_WITNESS_ENABLE=yes make check || (cat tests/test-suite.log; exit 1)
