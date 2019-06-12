#!/bin/bash

set -e

cd ../..

# test if we are in the root directory of repository
if [[ ! -e "autogen.sh" ]]
then
    echo "autogen.sh is not found, not root directory of the github repository?"
    exit 1
fi

# test if dependencies was built
if [[ -e depends/work/build ]]
then
    arch=`ls depends/work/build`
else
    arch=""
fi

if [[ -z "$arch" || ! -e "depends/$arch" ]]
then
    echo "Building dependencies..."
    pushd `pwd`
    cd depends
    make
    popd

    # test again
    arch=`ls depends/work/build`
    if [[ -z "$arch" || ! -e "depends/$arch" ]]
    then
        echo "Failed to build dependencies."
        exit 1
    fi
else
    echo "Dependencies are up to date: $arch"
fi

echo "Building sources..."

./autogen.sh
export HOSTS=$arch
export BASEPREFIX=`pwd`/depends
CONFIG_SITE=${BASEPREFIX}/`echo "${HOSTS}" | awk '{print $1;}'`/share/config.site ./configure --prefix=/ --enable-debug --with-utils --with-libs --with-daemon --with-gui=qt5 --enable-tests
make

# test if binaries are here
if [[ ! -e "src/opcx-cli" ]]
then
    echo "opcx-cli was not found"
    exit 1
fi

if [[ ! -e "src/opcx-tx" ]]
then
    echo "opcx-tx was not found"
    exit 1 
fi

if [[ ! -e "src/opcxd" ]]
then
    echo "opcxd was not found"
    exit 1 
fi

if [[ ! -e "src/qt/opcx-qt" ]]
then
    echo "opcx-qt was not found"
    exit 1 
fi

echo "${0} [done]."
