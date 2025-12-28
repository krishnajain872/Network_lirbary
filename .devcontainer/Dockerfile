FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC

# =========================================================
# Core system + C/C++ toolchain (single optimized layer)
# =========================================================
RUN apt-get update && apt-get install -y --no-install-recommends \
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
    unzip \
    jq \
    vim \
    && rm -rf /var/lib/apt/lists/*

# =========================================================
# GCC 12 as default
# =========================================================
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 100

ENV CC=gcc
ENV CXX=g++

# =========================================================
# Node.js 20 (binary install, MUCH faster)
# =========================================================
ENV NODE_VERSION=20.11.1

RUN curl -fsSL https://nodejs.org/dist/v${NODE_VERSION}/node-v${NODE_VERSION}-linux-x64.tar.xz \
    | tar -xJ -C /usr/local --strip-components=1

RUN npm install -g yarn pnpm eslint prettier && npm cache clean --force

# =========================================================
# Go (cached + slim install)
# =========================================================
ENV GO_VERSION=1.22.4
ENV GOROOT=/usr/local/go
ENV GOPATH=/go
ENV PATH=$PATH:/usr/local/go/bin:/go/bin

RUN curl -fsSL https://go.dev/dl/go${GO_VERSION}.linux-amd64.tar.gz \
    | tar -C /usr/local -xzf - \
    && mkdir -p /go/bin

RUN go install \
    github.com/go-delve/delve/cmd/dlv@latest \
    golang.org/x/tools/gopls@latest \
    golang.org/x/tools/cmd/goimports@latest \
    github.com/golangci/golangci-lint/cmd/golangci-lint@latest \
    honnef.co/go/tools/cmd/staticcheck@latest

# =========================================================
# Workspace
# =========================================================
WORKDIR /workspaces/network-library

CMD ["bash"]
# =========================================================