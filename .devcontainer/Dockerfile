FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install core build tools and dependencies
# Matching project dependencies + dev tools
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    wget \
    curl \
    pkg-config \
    gcc-12 \
    g++-12 \
    gdb \
    valgrind \
    libssl-dev \
    libprotobuf-dev \
    protobuf-compiler \
    libgrpc++-dev \
    protobuf-compiler-grpc \
    libyaml-cpp-dev \
    ccache \
    vim \
    openssh-client \
    && rm -rf /var/lib/apt/lists/*

# Set GCC 12 as default
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 100

ENV CC=gcc
ENV CXX=g++

WORKDIR /workspaces/network-library
