FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    cmake \
    make \
    mingw-w64 \
    && rm -rf /var/lib/apt/lists/*

# Default to i686 (32-bit) posix threads variant for std::thread support
RUN update-alternatives --set i686-w64-mingw32-g++ /usr/bin/i686-w64-mingw32-g++-posix 2>/dev/null || true && \
    update-alternatives --set i686-w64-mingw32-gcc /usr/bin/i686-w64-mingw32-gcc-posix 2>/dev/null || true

WORKDIR /build

ENTRYPOINT ["bash", "-c", \
    "cmake -DCMAKE_TOOLCHAIN_FILE=/src/toolchain-mingw32.cmake -DCMAKE_CXX_STANDARD=17 -B /build /src && cmake --build /build --verbose && cp /build/ag_mho.exe /build/ag_mho.dll /build/TerSafe.dll /build/BugTrace.dll /out/"]
