# syntax=docker/dockerfile:1.6
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC

# =========================================================
# FAST APT CONFIG (INDIA MIRROR + AGGRESSIVE CACHING)
# =========================================================
RUN sed -i 's|http://archive.ubuntu.com/ubuntu|http://in.archive.ubuntu.com/ubuntu|g' /etc/apt/sources.list \
    && echo 'Acquire::ForceIPv4 "true";' > /etc/apt/apt.conf.d/99force-ipv4 \
    && echo 'Acquire::http::Pipeline-Depth "5";' >> /etc/apt/apt.conf.d/99force-ipv4 \
    && echo 'Acquire::http::Timeout "30";' >> /etc/apt/apt.conf.d/99force-ipv4 \
    && echo 'Acquire::Retries "3";' >> /etc/apt/apt.conf.d/99force-ipv4 \
    && echo 'APT::Install-Recommends "false";' >> /etc/apt/apt.conf.d/99force-ipv4 \
    && echo 'APT::Install-Suggests "false";' >> /etc/apt/apt.conf.d/99force-ipv4

# =========================================================
# CORE SYSTEM TOOLS
# =========================================================
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    rm -f /etc/apt/apt.conf.d/docker-clean && \
    apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    curl \
    wget \
    gnupg \
    git \
    && rm -rf /var/lib/apt/lists/*

# =========================================================
# C/C++ TOOLCHAIN
# =========================================================
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    gcc-12 \
    g++-12 \
    gdb \
    ccache \
    && rm -rf /var/lib/apt/lists/*

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 100

# =========================================================
# BUILD TOOLS
# =========================================================
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt-get update && apt-get install -y --no-install-recommends \
    cmake \
    ninja-build \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# =========================================================
# CLANG TOOLCHAIN
# =========================================================
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt-get update && apt-get install -y --no-install-recommends \
    clang-15 \
    clang-format-15 \
    clang-tidy-15 \
    && rm -rf /var/lib/apt/lists/*

# =========================================================
# DEVELOPMENT LIBRARIES
# =========================================================
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt-get update && apt-get install -y --no-install-recommends \
    libssl-dev \
    libprotobuf-dev \
    protobuf-compiler \
    libgrpc++-dev \
    protobuf-compiler-grpc \
    libyaml-cpp-dev \
    liburing-dev \
    && rm -rf /var/lib/apt/lists/*

# =========================================================
# PYTHON
# =========================================================
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt-get update && apt-get install -y --no-install-recommends \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# =========================================================
# UTILITIES
# =========================================================
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt-get update && apt-get install -y --no-install-recommends \
    jq \
    unzip \
    vim \
    && rm -rf /var/lib/apt/lists/*

# =========================================================
# NODE.JS 20 LTS
# =========================================================
ENV NODE_VERSION=20.11.1
ENV PATH=/usr/local/node/bin:$PATH

RUN --mount=type=cache,target=/tmp/downloads \
    curl -fsSL https://nodejs.org/dist/v${NODE_VERSION}/node-v${NODE_VERSION}-linux-x64.tar.xz \
    -o /tmp/downloads/node.tar.xz && \
    mkdir -p /usr/local/node && \
    tar -xJf /tmp/downloads/node.tar.xz -C /usr/local/node --strip-components=1

RUN --mount=type=cache,target=/root/.npm \
    npm config set registry https://registry.npmjs.org/ && \
    npm install -g --loglevel=error \
    yarn@1.22.19 \
    pnpm@8.15.0

# =========================================================
# GO TOOLCHAIN
# =========================================================
ENV GO_VERSION=1.22.4
ENV GOROOT=/usr/local/go
ENV GOPATH=/go
ENV PATH=$PATH:$GOROOT/bin:$GOPATH/bin
ENV GOPROXY=https://proxy.golang.org,direct
ENV GOSUMDB=sum.golang.org

RUN --mount=type=cache,target=/tmp/downloads \
    curl -fsSL https://go.dev/dl/go${GO_VERSION}.linux-amd64.tar.gz \
    -o /tmp/downloads/go.tar.gz && \
    tar -C /usr/local -xzf /tmp/downloads/go.tar.gz && \
    mkdir -p $GOPATH/bin

RUN --mount=type=cache,target=/go/pkg/mod \
    --mount=type=cache,target=/root/.cache/go-build \
    go install github.com/go-delve/delve/cmd/dlv@latest && \
    go install golang.org/x/tools/gopls@latest && \
    go install golang.org/x/tools/cmd/goimports@latest && \
    go install github.com/golangci/golangci-lint/cmd/golangci-lint@v1.55.2

# =========================================================
# SSH SERVER (FOR VS CODE REMOTE SSH)
# =========================================================
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt-get update && apt-get install -y --no-install-recommends \
    openssh-server \
    sudo \
    && rm -rf /var/lib/apt/lists/*

RUN mkdir /var/run/sshd && \
    sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config && \
    sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config && \
    sed -i 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' /etc/pam.d/sshd

# Root password (dev only)
RUN echo "root:root" | chpasswd

# =========================================================
# VS CODE USER (RECOMMENDED)
# =========================================================
RUN useradd -ms /bin/bash vscode && \
    echo "vscode:vscode" | chpasswd && \
    adduser vscode sudo

# =========================================================
# COMPILER ENV
# =========================================================
ENV CC=gcc
ENV CXX=g++

# =========================================================
# WORKSPACE
# =========================================================
WORKDIR /workspaces/network-library

RUN mkdir -p /workspaces/network-library/.ccache \
    /go/pkg \
    /root/.npm

# =========================================================
# SSH PORT
# =========================================================
EXPOSE 22

# =========================================================
# START SSHD (REQUIRED FOR VS CODE)
# =========================================================
CMD ["/usr/sbin/sshd", "-D"]
