# Elden ring launcher

Launches Elden Ring without Easy Anti-Cheat. By default the game will not connect to FromSoftware's matchmaking servers, so online play is disabled.

Intended for convenience, and may be useful as launchers to inject dlls in order to run offline mods.

Users can inject dlls by creating a `eldenring_mods.txt` file and putting the name of each dll you want to load into the file on a new line.  

The "launch_modded_eldenring.exe" launcher must be in the `ELDEN RING/Game` directory (where eldenring.exe is located)

You can prefix a mod with a folder name, and organize your mods, if you'd like. Example:

```
dllmods/elden_ring_seamless_coop.dll
```

this will look for `elden_ring_seamless_coop.dll` in the `ELDEN RING/Game/dllmods` folder.

Please don't attempt to play online without Easy Anti-Cheat.

Feel free to modify/redistribute/repackage this launcher with your own mods.
 
