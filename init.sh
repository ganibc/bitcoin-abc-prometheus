#!/bin/bash

add-apt-repository ppa:bitcoin/bitcoin
apt-get update
apt-get install -y build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils yasm libboost-all-dev libzmq3-dev curl wget libdb4.8-dev libdb4.8++-dev cmake