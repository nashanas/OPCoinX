FROM debian:jessie
MAINTAINER pavik - https://github.com/pavik

ENV OPCX_USER=opcx

ENV OPCX_URL_LATEST=https://github.com/OPCoinX/OPCoinX/releases/latest \
 OPCX_URL_DOWNLOAD=https://github.com/OPCoinX/OPCoinX/releases/download \
 OPCX_CONF=/home/$OPCX_USER/.OPCoinX/OPCoinX.conf

RUN apt-get -qq update && \
apt-get install -yq wget ca-certificates pwgen curl && \
rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* && \
export OPCX_URL=`curl -Ls -o /dev/null -w %{url_effective} $OPCX_URL_LATEST` && \
export OPCX_VERSION=`echo $OPCX_URL | grep -oP 'tag\/v(.+?)$' | sed -E 's/tag\/v//'` && \
wget $OPCX_URL_DOWNLOAD/v$OPCX_VERSION/opcx-$OPCX_VERSION-x86_64-linux-gnu.tar.gz -O /tmp/opcx.tar.gz && \
mkdir -p /opt && \
cd /opt && \
tar xvzf /tmp/opcx.tar.gz && \
rm /tmp/opcx.tar.gz && \
ln -sf opcx-$OPCX_VERSION opcx && \
ln -sf /opt/opcx/bin/opcxd /usr/local/bin/opcxd && \
ln -sf /opt/opcx/bin/opcx-cli /usr/local/bin/opcx-cli && \
ln -sf /opt/opcx/bin/opcx-tx /usr/local/bin/opcx-tx && \
adduser --uid 1000 --system ${OPCX_USER} && \
mkdir -p /home/${OPCX_USER}/.OPCoinX/ && \
chown -R ${OPCX_USER} /home/${OPCX_USER} && \
echo "success: $OPCX_CONF"

USER opcx
RUN printf "listen=1\nserver=1\ntxindex=1\nrpcuser=*Your\$Username*\nrpcpassword=*Your\$Password*\nrpcport=51473\nport=51572\nrpcallowip=127.0.0.1\n" > $OPCX_CONF && \
    sed -i -e 's/\*Your$Username\*/opcx/g' ${OPCX_CONF} && \
    sed -i -e 's/\*Your$Password\*/'`pwgen 32 1`'/g' ${OPCX_CONF} && \
    echo "success"

EXPOSE 51572
VOLUME ["/home/opcx/.OPCoinX"]
WORKDIR /home/opcx

ENTRYPOINT ["/usr/local/bin/opcxd"]