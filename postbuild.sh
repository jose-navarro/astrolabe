#!/bin/bash

echo 'COPYING HEADERS AND BINARIES TO headers and binaries folders...'

# Create the header and binaries folder if these do not exist.

mkdir -p ./headers
mkdir -p ./headers/xtl
mkdir -p ./binaries


# Copy all *.h, *.hpp files.

cp ./src/*.hpp   ./headers/
cp ./src/*.h     ./headers/
cp ./src/xtl/*.h ./headers/xtl/

# Copy library

cp ./$1/*.a ./binaries/

