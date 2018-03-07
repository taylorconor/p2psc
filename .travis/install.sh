DEPS_DIR="${TRAVIS_BUILD_DIR}/deps"
mkdir -p ${DEPS_DIR} && cd ${DEPS_DIR}

function install_boost {
  BOOST_LIBRARIES="chrono,system,test"
  BOOST_VERSION="1.62.0"
  BOOST_URL="https://sourceforge.net/projects/boost/files/boost/${BOOST_VERSION}/boost_${BOOST_VERSION//\./_}.tar.gz"
  BOOST_DIR="${DEPS_DIR}/boost"
  echo "Downloading Boost ${BOOST_VERSION} from ${BOOST_URL}"
  mkdir -p ${BOOST_DIR} && cd ${BOOST_DIR}
  wget -O - ${BOOST_URL} | tar --strip-components=1 -xz -C ${BOOST_DIR} || exit 1
  ./bootstrap.sh --with-libraries=${BOOST_LIBRARIES} && ./b2
  export BOOST_ROOT=${BOOST_DIR}
}

[ $CXX = g++ ] && export CXX=g++-6 || true
[ $CXX = clang++ ] && export CXX=clang++-3.8 || true
install_boost