#!/bin/bash

# Remove old files
rm -f KoboRoot.tgz libtweaks.so
find ./src -type f \( -name '*.o' -o -name '*.moc' \) -delete

# Build resources
set -a
source .env
set +a
"$RCC_PATH" src/resources.qrc -o src/resources.qrc.cpp

# Build plugin
docker run --volume="$PWD:$PWD" --user="$(id -u):$(id -g)" --workdir="$PWD" --env=HOME --entrypoint=make --rm -it ghcr.io/pgaskin/nickeltc:1.0 all koboroot
