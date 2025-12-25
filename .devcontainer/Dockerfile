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
    clang-15 \
    clang-format-15 \
    clang-tidy-15 \
    gdb \
    valgrind \
    libssl-dev \
    libprotobuf-dev \
    protobuf-compiler \
    libgrpc++-dev \
    protobuf-compiler-grpc \
    libyaml-cpp-dev \
    libgtest-dev \
    liburing-dev \
    pybind11-dev \
    python3-dev \
    python3-pip \
    ccache \
    doxygen \
    graphviz \
    vim \
    openssh-client \
    openssh-server \
    && rm -rf /var/lib/apt/lists/*

# Set GCC 12 as default
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 100

ENV CC=gcc
ENV CXX=g++

# Configure SSH for remote debugging
RUN mkdir /var/run/sshd && \
    echo 'root:root' | chpasswd && \
    sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config && \
    sed -i 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' /etc/pam.d/sshd

EXPOSE 22

WORKDIR /workspaces/network-library

# Start SSH server by default if run manually
CMD ["/usr/sbin/sshd", "-D"]
