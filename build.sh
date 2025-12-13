#!/bin/bash

# Remove old files
docker run --volume="$PWD:$PWD" --user="$(id -u):$(id -g)" --workdir="$PWD" --env=HOME --entrypoint=make --rm -it ghcr.io/pgaskin/nickeltc:1.0 clean

# Build plugin
docker run --volume="$PWD:$PWD" --user="$(id -u):$(id -g)" --workdir="$PWD" --env=HOME --entrypoint=make --rm -it ghcr.io/pgaskin/nickeltc:1.0 all koboroot
