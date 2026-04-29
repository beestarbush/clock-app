FROM ubuntu:25.10

ARG USER_ID=1000
ARG GROUP_ID=1000

ENV DEBIAN_FRONTEND=noninteractive \
    QT_QPA_FONTDIR=/usr/share/fonts/truetype/dejavu \
    LANG=C.UTF-8 \
    LC_ALL=C.UTF-8

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        ca-certificates \
        cmake \
        git \
        libqrencode-dev \
        libssl-dev \
        ninja-build \
        pkg-config \
        qml6-module-qtqml-models \
        qml6-module-qtqml-workerscript \
        qml6-module-qtquick \
        qml6-module-qtquick-controls \
        qml6-module-qtquick-layouts \
        qml6-module-qtquick-shapes \
        qml6-module-qtquick-templates \
        qml6-module-qtquick-window \
        qt6-base-dev \
        qt6-base-dev-tools \
        qt6-declarative-dev \
        qt6-declarative-dev-tools \
        qt6-image-formats-plugins \
        qt6-shadertools-dev \
        qt6-svg-dev \
        qt6-svg-plugins \
        qt6-websockets-dev \
        tzdata \
    && rm -rf /var/lib/apt/lists/* /tmp/*

RUN groupadd -g ${GROUP_ID} builder && \
    useradd -m -u ${USER_ID} -g builder builder && \
    mkdir -p /workdir && chown builder:builder /workdir

WORKDIR /workdir

USER builder

CMD ["/bin/bash"]
