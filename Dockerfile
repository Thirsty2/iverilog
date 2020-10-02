ARG IMAGE="alpine:3.12"
# Note that some commands such as 'apk add' won't work in arbitrary images.  For instance,
# 'apk add' works fine under alpine, but not under Ubuntu (at least not by default.)
# In other words, the following 'apk add' sections assume an alpine based image or a derivitive

FROM $IMAGE as base

RUN apk add --no-cache \
        bzip2 \
        libbz2 \
        libgcc \
        libhistory \
        libstdc++ \
        make \
        readline \
        zlib

FROM base as builder

RUN apk add --no-cache \
        autoconf \
        build-base \
        bison \
        bzip2-dev \
        flex \
        git \
	gperf \
        readline-dev \
        zlib-dev

COPY . .

RUN sh autoconf.sh && \
    ./configure && \
    make && \
    make install

FROM builder as test-builder

# WORKDIR /iverilog
RUN make check

FROM base as release-candidate

COPY --from=builder /usr/local /usr/local/

RUN adduser --disabled-password ic
USER ic
WORKDIR /home/ic

FROM release-candidate as release-candidate-entrypoint-make

ENTRYPOINT [ "make" ]

# This commented section or something similar may be used to test the release candidate
# image before it is finally released.  A failure here would stop the process so that
# a faulty image is not released.
#
# The make and make clean lines below fail if iverilog/dockerfiles/test isn't available
# and older release branches like v10_3 do not have this directory, so we need a
# layer that does have it to copy from.  We create that layer as test-branch here:
# FROM builder as test-branch
# WORKDIR /iverilog
# ARG TEST_BRANCH=master
# RUN git checkout ${TEST_BRANCH}
#
# FROM release-candidate-entrypoint-make as test-release-candidate-entrypoint-make
# COPY --from=test-branch /iverilog/dockerfiles/test /home/ic/
# 
# RUN make
# RUN make clean

FROM release-candidate-entrypoint-make as iverilog-make

FROM release-candidate as iverilog

# Below are some sample commands to build docker images.
# 
# The following builds iverilog from the source in the current directory
# docker build  . -t iverilog
#
# The following builds the latest code from the Thirsty2/iverilog fork master branch
# docker build --build-arg IVERILOG_REPO_URL=https://github.com/Thirsty2/iverilog.git --build-arg TEST_BRANCH=add-dockerfile . -t iverilog
#
# The following won't work until the pull request is accepted, but should work afterwards to build the master branch of steveicarus/iverilog
# docker build . -t iverilog
