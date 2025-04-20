# Pokémon Emerald - Mystery Event Online Trading

> Warning - this code is functional but still in development. Trades may freeze the game or fail, requiring restart. 

The project aims to convert the trade functionality of https://github.com/KittyPBoxx/pokeemerald-net-demo to a mystery event so that it can be run on an original cartrige. 

## Install 

Run the ./build_events script to build everything, the mystery events, the multiboot roms and the GC/Wii channels

## Run

- Make sure CelioServer is running somewhere (either a local computer or the cloud)
- Load either the Gamecube or Wii channel in your chosen device (you should then configure it to connect to the server)
- Connect a GBA to the GC/Wii with a DOL-011 cable, insert an Emerald (USA/EUR) cart with a game started, and boot the device with it plugged in

## References

Multiboot injection
https://github.com/Wack0/gba-gen3multiboot

GC/Wii GUI
https://github.com/dborth/libwiigui

GC/Wii Joybus Data Transfer
https://github.com/afska/gba-link-connection/blob/master/lib/LinkCube.hpp
https://github.com/shonumi/ninja/blob/main/NINJA.c