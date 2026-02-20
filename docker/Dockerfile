FROM ubuntu:22.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive

# Install required system dependencies
RUN apt-get update && apt-get install -y \
	build-essential \
	cmake \
	ninja-build \
	git \
	wget \
	curl \
	unzip \
	libxcb-xinerama0 \
	libglib2.0-0 \
	libgl1-mesa-glx \
	libegl1 \
	libfontconfig1 \
	libxkbcommon0 \
	libxkbcommon-x11-0 \
	libxcb-cursor0 \
	libxcb-icccm4 \
	libxcb-keysyms1 \
	libxcb-shape0 \
	libxcb-image0 \
	libxcb-render-util0 \
	libxcb-xkb1 \
	libX11-* \
	libxkb-* \
	libxcb-* \
	libdbus-1-3 \
	mesa-common-dev \
	libglu1-mesa-dev \
	libfreetype6 \
	libssl-dev \
	ca-certificates \
	python3 \
	'^libxcb.*-dev' \
	libx11-xcb-dev \
	libxrender-dev \
	libxi-dev \
	libxkbcommon-dev \
	libxkbcommon-x11-dev \
	libgl-dev \
	libglu-dev \
	'libxcb*-dev' \
	libpcre2-dev \
	libz-dev \
	libfreetype6-dev \
	libpng-dev \
	libjpeg-dev \
	libsqlite3-dev \
	libharfbuzz-dev \
	libb2-dev \
	libdouble-conversion-dev \
	tzdata \
	&& rm -rf /var/lib/apt/lists/* \
	&& apt clean
	

# Download and install Qt 6.9.0 using the official installer
WORKDIR /tmp
RUN wget -q  https://download.qt.io/official_releases/qt/6.9/6.9.0/single/qt-everywhere-src-6.9.0.tar.xz && \
	tar xf qt-everywhere-src-6.9.0.tar.xz && \
	mkdir -p build && \
	cd build && \
	/tmp/qt-everywhere-src-6.9.0/configure -nomake tests -nomake examples -submodules qtbase,qtdeclarative,qtwebsockets -xcb-xlib -xcb -xkbcommon -no-feature-printer -no-feature-clipboard && \
	cmake --build . --parallel && \
	cmake --install . && \
	rm -rf /tmp/qt* && \
	rm -rf /tmp/build

# Set environment variables for Qt (installed to /usr/local by default)
ENV PATH="/usr/local/Qt-6.9.0/bin:$PATH"
ENV LD_LIBRARY_PATH="/usr/local/Qt-6.9.0/lib"
ENV QT_QPA_FONTDIR="/usr/share/fonts/truetype/dejavu"

# Create a non-root user for building
ARG USER_ID=1000
ARG GROUP_ID=1000
RUN groupadd -g ${GROUP_ID} builder && \
	useradd -m -u ${USER_ID} -g builder builder

# Set working directory
RUN mkdir -p /workdir && chown builder:builder /workdir
WORKDIR /workdir

# Switch to non-root user
USER builder

CMD ["/bin/bash"]
