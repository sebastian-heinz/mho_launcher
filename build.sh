#!/bin/bash
set -e

cd "$(dirname "$0")"

# Create output directory
mkdir -p out

# Build the Docker image
docker build -t mho_launcher_build .

# Run the build
# /src  = source (read-only)
# /out  = output directory for artifacts
# /build is internal to the container
docker run --rm \
    -v "$(pwd):/src:ro" \
    -v "$(pwd)/out:/out" \
    mho_launcher_build

echo ""
echo "Build complete! Output files:"
ls -la out/
