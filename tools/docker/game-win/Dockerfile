# TR2X building toolchain.
#
# This is a multi-stage Docker image. It is designed to keep the final image
# size low. Each stage builds an external dependency. The final stage takes the
# artifacts (binaries, includes etc.) from previous stages and installs all the
# tools necessary to build TR2X.

# MinGW
FROM ubuntu:latest as mingw

# don't prompt during potential installation/update of tzinfo
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Warsaw

RUN apt-get update \
    && apt-get upgrade -y \
    && apt-get install -y \
        gcc-mingw-w64-i686 \
        g++-mingw-w64-i686 \
        git \
        make



# pcre
FROM mingw as pcre2
RUN git clone https://github.com/PCRE2Project/pcre2
RUN apt-get -y install libtool
RUN cd pcre2 \
    && autoreconf -fi \
    && ./configure \
        --host=i686-w64-mingw32 \
        --prefix=/ext/ \
    && make -j 4 \
    && make install



# SDL
FROM mingw as sdl
RUN git clone https://github.com/libsdl-org/SDL -b SDL2
RUN apt-get install -y automake
RUN cd SDL \
    && aclocal -I acinclude \
    && autoconf \
    && mkdir build \
    && cd build \
    && ../configure \
        --host=i686-w64-mingw32 \
        --build=i686-pc-mingw32 \
        --prefix=/ext/ \
        --enable-shared \
        --enable-static \
    && make -j 4 \
    && make install



# TR2X
FROM mingw

# set the build dir - actual files are mounted with a Docker volume
RUN mkdir /app
WORKDIR /app

COPY --from=sdl /ext/ /ext/
COPY --from=pcre2 /ext/ /ext/

# system dependencies
# configure pkgconfig manually
# https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=967969
ENV PKG_CONFIG_LIBDIR=/ext/lib/
ENV PKG_CONFIG_PATH=/ext/lib/pkgconfig/
RUN apt-get install -y \
        mingw-w64-tools \
        pkg-config \
        upx \
        python3-pip \
    && python3 -m pip install --break-system-packages \
        pyjson5 \
        meson \
        ninja

ENV PYTHONPATH=/app/tools/
ENTRYPOINT ["/app/tools/docker/game-win/entrypoint.sh"]
