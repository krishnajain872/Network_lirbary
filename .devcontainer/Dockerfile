# syntax=docker/dockerfile:1.6
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC

# =========================================================
# FAST APT CONFIG (CRITICAL FOR WINDOWS + INDIA)
# =========================================================
RUN sed -i 's|http://archive.ubuntu.com/ubuntu|http://in.archive.ubuntu.com/ubuntu|g' /etc/apt/sources.list \
    && echo 'Acquire::ForceIPv4 "true";' > /etc/apt/apt.conf.d/99force-ipv4 \
    && echo 'Acquire::http::Pipeline-Depth "5";' >> /etc/apt/apt.conf.d/99force-ipv4 \
    && echo 'Acquire::http::No-Cache "true";' >> /etc/apt/apt.conf.d/99force-ipv4 \
    && echo 'APT::Install-Recommends "false";' >> /etc/apt/apt.conf.d/99force-ipv4 \
    && echo 'APT::Install-Suggests "false";' >> /etc/apt/apt.conf.d/99force-ipv4

# =========================================================
# BASE SYSTEM + C/C++ TOOLCHAIN (PARALLEL + CACHED)
# =========================================================
RUN --mount=type=cache,target=/var/cache/apt,sharing=locked \
    --mount=type=cache,target=/var/lib/apt,sharing=locked \
    apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    git \
    curl \
    pkg-config \
    gcc-12 \
    g++-12 \
    clang-15 \
    clang-format-15 \
    clang-tidy-15 \
    gdb \
    libssl-dev \
    libprotobuf-dev \
    protobuf-compiler \
    libgrpc++-dev \
    protobuf-compiler-grpc \
    libyaml-cpp-dev \
    liburing-dev \
    python3 \
    python3-dev \
    python3-pip \
    ccache \
    ca-certificates \
    jq \
    unzip

# =========================================================
# GCC 12 AS DEFAULT
# =========================================================
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 100

ENV CC=gcc
ENV CXX=g++

# =========================================================
# NODE.JS 20 LTS (PARALLEL DOWNLOAD)
# =========================================================
ENV NODE_VERSION=20.11.1

RUN --mount=type=cache,target=/tmp/node-cache \
    curl -fsSL https://nodejs.org/dist/v${NODE_VERSION}/node-v${NODE_VERSION}-linux-x64.tar.xz \
    -o /tmp/node-cache/node.tar.xz \
    && tar -xJf /tmp/node-cache/node.tar.xz -C /usr/local --strip-components=1

RUN --mount=type=cache,target=/root/.npm \
    npm install -g --prefer-offline \
    yarn \
    pnpm \
    eslint \
    prettier

# =========================================================
# GO TOOLCHAIN (CACHED DOWNLOAD + PARALLEL INSTALLS)
# =========================================================
ENV GO_VERSION=1.22.4
ENV GOROOT=/usr/local/go
ENV GOPATH=/go
ENV PATH=$PATH:/usr/local/go/bin:/go/bin

RUN --mount=type=cache,target=/tmp/go-cache \
    curl -fsSL https://go.dev/dl/go${GO_VERSION}.linux-amd64.tar.gz \
    -o /tmp/go-cache/go.tar.gz \
    && tar -C /usr/local -xzf /tmp/go-cache/go.tar.gz

RUN mkdir -p /go/bin

# Install Go tools in parallel with caching
RUN --mount=type=cache,target=/go/pkg/mod \
    --mount=type=cache,target=/root/.cache/go-build \
    go install github.com/go-delve/delve/cmd/dlv@latest & \
    go install golang.org/x/tools/gopls@latest & \
    go install golang.org/x/tools/cmd/goimports@latest & \
    go install github.com/golangci/golangci-lint/cmd/golangci-lint@latest & \
    go install honnef.co/go/tools/cmd/staticcheck@latest & \
    wait

# =========================================================
# WORKSPACE
# =========================================================
WORKDIR /workspaces/network-library

CMD ["bash"]