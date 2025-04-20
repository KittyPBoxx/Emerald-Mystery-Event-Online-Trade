# GBA Gen3 Multiboot
A GC and Wii homebrew app that sends a binary to the GBA using the different multiboot protocol used by the third generation of Pokémon games (Ruby, Sapphire, Emerald, FireRed, LeafGreen).

# Usage
Have a GC Controller in Port 1 and a GBA with Gen3 game in Port 2.  
Put your payload code to do some interesting stuff with the Pokémon game you have in `gba` dir. Example code that warps the character to the Hall of Fame, adding a Bad Egg if the save has no Pokémon provided.  
Recompile, send to Wii or GC, turn on your GBA, hope that your code runs after the initial copyright screen of the game.  
(Code execution rate is for some reason not 100% reliable, PR to fix would be greatly appreciated. Sometimes KeyC derivation fails, sometimes GBA ignores the sent multiboot image, could be due to failure of a few different sends)

# Acknowledgements
Thanks to FIX94 for your multiboot game dumper, which the multiboot code is loosely based on (differences in crypto & protocol...)  
Without it, this would have taken longer to do than the 2 days or so that it took.