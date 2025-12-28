FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# =========================================================
# Base system + C/C++ toolchain + core dependencies
# =========================================================
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
    python3-setuptools \
    python3-venv \
    ccache \
    doxygen \
    graphviz \
    vim \
    jq \
    zip \
    unzip \
    ca-certificates \
    openssh-client \
    openssh-server \
    && rm -rf /var/lib/apt/lists/*

# =========================================================
# GCC 12 as default
# =========================================================
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 100

ENV CC=gcc
ENV CXX=g++

# =========================================================
# Node.js 20 LTS + npm + frontend tooling
# =========================================================
RUN curl -fsSL https://deb.nodesource.com/setup_20.x | bash - \
    && apt-get update \
    && apt-get install -y nodejs \
    && npm install -g \
    yarn \
    pnpm \
    node-gyp \
    eslint \
    prettier \
    && npm cache clean --force \
    && rm -rf /var/lib/apt/lists/*

# =========================================================
# Go (official binary) + Go tooling + debugger
# =========================================================
ENV GO_VERSION=1.22.4

RUN wget -q https://go.dev/dl/go${GO_VERSION}.linux-amd64.tar.gz && \
    tar -C /usr/local -xzf go${GO_VERSION}.linux-amd64.tar.gz && \
    rm go${GO_VERSION}.linux-amd64.tar.gz

ENV GOROOT=/usr/local/go
ENV GOPATH=/go
ENV PATH=$PATH:/usr/local/go/bin:/go/bin

RUN mkdir -p /go/{bin,src,pkg}

RUN go install github.com/go-delve/delve/cmd/dlv@latest && \
    go install golang.org/x/tools/gopls@latest && \
    go install golang.org/x/tools/cmd/goimports@latest && \
    go install github.com/golangci/golangci-lint/cmd/golangci-lint@latest && \
    go install honnef.co/go/tools/cmd/staticcheck@latest && \
    go install golang.org/x/vuln/cmd/govulncheck@latest && \
    go install github.com/cweill/gotests/gotests@latest && \
    go install github.com/fatih/gomodifytags@latest && \
    go install github.com/josharian/impl@latest

# =========================================================
# SSH configuration (remote debugging / VS Code)
# =========================================================
RUN mkdir /var/run/sshd && \
    echo 'root:root' | chpasswd && \
    sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config && \
    sed -i 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' /etc/pam.d/sshd

EXPOSE 22

# =========================================================
# Workspace
# =========================================================
WORKDIR /workspaces/network-library

# =========================================================
# Default command
# =========================================================
CMD ["/usr/sbin/sshd", "-D"]
