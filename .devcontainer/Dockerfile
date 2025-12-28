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
# CORE SYSTEM TOOLS (MINIMAL, FAST LAYER)
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
# C/C++ TOOLCHAIN (SPLIT FOR BETTER CACHING)
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

# Set GCC 12 as default
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 100

# =========================================================
# BUILD TOOLS (SEPARATE LAYER)
# =========================================================
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt-get update && apt-get install -y --no-install-recommends \
    cmake \
    ninja-build \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# =========================================================
# CLANG TOOLCHAIN (OPTIONAL - SEPARATE LAYER)
# =========================================================
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt-get update && apt-get install -y --no-install-recommends \
    clang-15 \
    clang-format-15 \
    clang-tidy-15 \
    && rm -rf /var/lib/apt/lists/*

# =========================================================
# DEVELOPMENT LIBRARIES (SPLIT BY CATEGORY)
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
# PYTHON (MINIMAL INSTALL)
# =========================================================
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt-get update && apt-get install -y --no-install-recommends \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# =========================================================
# UTILITIES (LIGHTWEIGHT)
# =========================================================
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt-get update && apt-get install -y --no-install-recommends \
    jq \
    unzip \
    vim \
    && rm -rf /var/lib/apt/lists/*

# =========================================================
# NODE.JS 20 LTS (CACHED DOWNLOAD)
# =========================================================
ENV NODE_VERSION=20.11.1
ENV PATH=/usr/local/node/bin:$PATH

RUN --mount=type=cache,target=/tmp/downloads \
    curl -fsSL https://nodejs.org/dist/v${NODE_VERSION}/node-v${NODE_VERSION}-linux-x64.tar.xz \
    -o /tmp/downloads/node.tar.xz && \
    mkdir -p /usr/local/node && \
    tar -xJf /tmp/downloads/node.tar.xz -C /usr/local/node --strip-components=1

# Install npm packages globally with cache
RUN --mount=type=cache,target=/root/.npm \
    npm config set registry https://registry.npmjs.org/ && \
    npm install -g --loglevel=error \
    yarn@1.22.19 \
    pnpm@8.15.0

# =========================================================
# GO TOOLCHAIN (CACHED + OPTIMIZED)
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

# Install Go tools with module cache
RUN --mount=type=cache,target=/go/pkg/mod \
    --mount=type=cache,target=/root/.cache/go-build \
    go install -v github.com/go-delve/delve/cmd/dlv@latest && \
    go install -v golang.org/x/tools/gopls@latest && \
    go install -v golang.org/x/tools/cmd/goimports@latest && \
    go install -v github.com/golangci/golangci-lint/cmd/golangci-lint@v1.55.2

# =========================================================
# COMPILER ENVIRONMENT
# =========================================================
ENV CC=gcc
ENV CXX=g++

# =========================================================
# WORKSPACE SETUP
# =========================================================
WORKDIR /workspaces/network-library

# Create necessary directories
RUN mkdir -p /workspaces/network-library/.ccache && \
    mkdir -p /go/pkg && \
    mkdir -p /root/.npm

# Keep container running
CMD ["/bin/bash", "-c", "trap 'exit 0' SIGTERM; tail -f /dev/null & wait"]