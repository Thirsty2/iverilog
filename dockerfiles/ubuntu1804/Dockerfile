ARG IMAGE="ubuntu:18.04"
# Note that some commands such as 'apk add' won't work in arbitrary images.  For instance,
# 'apt-get' works fine under ubuntu, but not under alpine (at least not by default.)

FROM $IMAGE as base

# The following will be cached by the docker layer caching mechanism the first time this
# dockerfile is built, and these commands will never execute again unless you force them
# to with the --no-cache option:
# docker build --no-cache . -t iverilog:latest

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get -y update && \
    apt-get install -y \
        make && \
    rm -rf /var/lib/apt/lists/*

FROM base as builder

ARG DEBIAN_FRONTEND=noninteractive

ARG IVERILOG_REPO_URL=https://github.com/steveicarus/iverilog.git

RUN apt-get -y update && \
    apt-get install -y \
        autoconf \
        automake \
        bison \
        build-essential \
        flex \
        git \
        gperf 

# This clone command results in a bare repo that will remain at the state it
# is at when this Dockerfile is first built and cached by docker. 
# This leverages the caching to avoid repeated full clones, saving bandwidth.
RUN git clone --mirror ${IVERILOG_REPO_URL} iverilog.git

# Bust the cache here or the following 'git remote update' will not execute.
# Pass a random value to the BUST_THE_CAST ENV declared below for 
# every build where you want to bust the cache.  As an alternative to a 
# random variable, you could use a version string or git hash or similar
# to bust the cache only when your (branch, source, whatever) is updated.
# The following build command is a good way to bust the cache here.
# docker build --build-arg IVERILOG_BRANCH=master --build-arg BUST_THE_CACHE=$(date +%s) . -t iverilog:latest
ARG BUST_THE_CACHE="cache not busted"
RUN echo "${BUST_THE_CACHE}" > bust_the_cache.txt
RUN cd iverilog.git && \
    git remote update && \
    cd ..

# The branch to build may be overridden to any git branch or tag 
# using --build-arg on the command line:  
# docker build --build-arg IVERILOG_BRANCH=v10_3 . -t iverilog:10.3
ARG IVERILOG_BRANCH=master

RUN git clone --branch=${IVERILOG_BRANCH} iverilog.git iverilog && \
    cd iverilog && \
    bash autoconf.sh && \
    ./configure && \
    make && \
    make install

FROM builder as test-builder

WORKDIR /iverilog
RUN make check

FROM base as release-candidate

COPY --from=builder /usr/local /usr/local/

RUN adduser --disabled-password ic
USER ic
WORKDIR /home/ic

FROM release-candidate as release-candidate-entrypoint-make

ENTRYPOINT [ "make" ]

# The make and make clean lines below fail if iverilog/dockerfiles/test isn't available
# and older release branches like v10_3 do not have this directory, so we need a
# layer that does have it to copy from.  We create that layer as test-branch here:
FROM builder as test-branch
WORKDIR /iverilog
ARG TEST_BRANCH=master
RUN git checkout ${TEST_BRANCH}

FROM release-candidate-entrypoint-make as test-release-candidate-entrypoint-make
COPY --from=test-branch /iverilog/dockerfiles/test /home/ic/

RUN make
RUN make clean

FROM release-candidate-entrypoint-make as iverilog-make

FROM release-candidate as iverilog

# Below are some sample commands to build docker images.
# 
# The following builds the v10_3 tag from the Thirsty2/iverilog fork 
# docker build --build-arg IVERILOG_REPO_URL=https://github.com/Thirsty2/iverilog.git --build-arg IVERILOG_BRANCH=v10_3 --build-arg TEST_BRANCH=add-dockerfile . -t iverilog:10.3
#
# The following builds the latest code from the Thirsty2/iverilog fork master branch
# docker build --build-arg IVERILOG_REPO_URL=https://github.com/Thirsty2/iverilog.git --build-arg TEST_BRANCH=add-dockerfile . -t iverilog
#
# The following won't work until the pull request is accepted, but should work afterwards to build the master branch of steveicarus/iverilog
# docker build . -t iverilog