#!/bin/bash

sudo xcode-select --install

/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

brew install git cmake

sudo xcode-select --reset
