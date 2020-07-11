# Lise Control

TODO: Description

## Installation

1. Make sure to clone the repository and all its submodules
   `git clone --recurse-submodules https://github.com/HenningBrandt/lise_control.git`
2. The build assumes [WiringPi](http://wiringpi.com) and [libyaml](https://github.com/yaml/libyaml) installed on your system.
   You can install these dependencies yourself or use `make deps` to install them.
3. If you have the repo and the required dependencies run `make` to build the project
   and `make install` to install it.
4. The wiring can be configured via `~/.config/lise/wiring.yaml` (see _Configure_)
5. After that you can start and stop the service via `sudo systemctl start lise` and `sudo systemctl stop lise`
   or (what you normally want) start the service as part of the boot process `sudo sytemctl enable lise`

## Configure

TODO: Describe how to configure the wiring
