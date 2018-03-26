#
# Dockerfile
#
# @author gani.budicahyonog@bitmain.com copied from btcpool repositories
# @copyright btc.com
# @since 2018-03-26
#
#
FROM phusion/baseimage:0.9.22
MAINTAINER Gani Budi Cahyono <gani.budicahyonog@bitmain.com>

ENV HOME /root
ENV TERM xterm
CMD ["/sbin/my_init"]

# use aliyun source
#ADD sources-aliyun.com.list /etc/apt/sources.list

RUN apt-get update
RUN apt-get install -y build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils yasm
RUN apt-get install -y libboost-all-dev libzmq3-dev curl wget
RUN apt-get install -y git
RUN apt-get install -y cmake

RUN mkdir ~/source
# build protobuf
RUN cd ~/source && wget https://github.com/google/protobuf/releases/download/v3.5.1/protobuf-all-3.5.1.tar.gz \
  && tar zxf protobuf-all-3.5.1.tar.gz \
  && cd protobuf-3.5.1 \
  && ./autogen.sh \
  && ./configure \
  && make && make install

RUN ldconfig -v | grep protobuf

# build prometheus-cpp client
RUN cd ~/source && git clone https://github.com/jupp0r/prometheus-cpp.git
RUN cd ~/source/prometheus-cpp \
  && pwd \
  && git checkout tags/v0.2 \
  && git submodule init \
  && git submodule update
RUN cd ~/source/prometheus-cpp && cmake . -DBUILD_SHARED_LIBS=ON -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
  && make \ 
  && make install
  #&& mkdir -p deploy && make DESTDIR=`pwd`/deploy install
RUN ls /usr/local/lib
RUN ldconfig -v | grep prometheus
# build bitcoind

RUN cd ~/source && git clone https://github.com/ganibc/bitcoin-abc-prometheus.git
RUN cd ~/source \
  && cd bitcoin-abc-prometheus \
  && ./autogen.sh \
  && ./configure --disable-wallet --disable-tests --with-prometheus \
  && make && make install
  #&& make -j$(nproc) && make install

# mkdir bitcoind data dir
RUN mkdir -p /root/.bitcoin
RUN mkdir -p /root/scripts

# scripts
ADD opsgenie-monitor-bitcoind.sh   /root/scripts/opsgenie-monitor-bitcoind.sh

# crontab shell
ADD crontab.txt /etc/cron.d/bitcoind

# logrotate
ADD logrotate-bitcoind /etc/logrotate.d/bitcoind

#
# services
#

# service for mainnet
RUN mkdir    /etc/service/bitcoind
ADD run      /etc/service/bitcoind/run
RUN chmod +x /etc/service/bitcoind/run

#service for testnet
#RUN mkdir        /etc/service/bitcoind_testnet
#ADD run_testnet /etc/service/bitcoind_testnet/run
#RUN chmod +x     /etc/service/bitcoind_testnet/run

# remove source & build files
RUN rm -rf ~/source

# clean
RUN apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
