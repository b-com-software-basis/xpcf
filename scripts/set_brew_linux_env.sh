#!/bin/bash

export REMAKENROOT=$HOME/.remaken/packages/linux-gcc
export PKG_CONFIG_PATH=/home/linuxbrew/.linuxbrew/opt/openssl/lib/pkgconfig:/home/linuxbrew/.linuxbrew/lib/pkgconfig/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/linuxbrew/.linuxbrew/lib:.

if [[ "$OSTYPE" == "darwin"* ]]; then
# overload for mac values
	export PKG_CONFIG_PATH=/usr/local/opt/openssl@1.1/lib/pkgconfig:/usr/local/lib/pkgconfig
fi
