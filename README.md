# MAME (Skanner TV fork)

See [original MAME repository ](https://github.com/mamedev/mamehttp://opensource.org/licenses/GPL-2.0).

## Why this fork exists

This fork exists for one purpose: get the Radica **Skannerz TV** (`rad_sktv`)
driver working, by any means necessary — mostly by brute-forcing the
reverse-engineering with an AI coding agent — and to rip assets and 
ship a WebAssembly build of it that runs in a browser.

### Changes made in this fork so far

- Added a `uart_rx_force()` bypass to the shared SPG2xx UART device and
  reworked the `rad_sktv` scanner state machine, to handle peripherals (like
  the Skannerz scanner) that disable RxEn during TX faster than the normal
  baud-rate delay.
- Implemented full scanner emulation: the save protocol handshake so the
  game can write monster data back to the scanner after battles, and a
  cheat table of 100 monster/item type IDs so the player starts with the
  full roster of 79 monsters and 20 items.
- Fixed a Down/Left input bit swap on the console IOA port.
- Fixed three bugs breaking input on the battle/monster screens: player 2's
  connection probe being ignored, reversed BHI/BLO byte order in the
  scanner protocol, and IOA directional bits conflicting with scanner input.
- Added a "Scanner 2" DIP switch to support 1-player mode without a second
  physical scanner.
- Added optional SPG2xx debug hooks for asset ripping (dumping decoded
  audio channels as WAV and unique rendered tiles as PPM), off by default.
- Added a minimal Emscripten/WebAssembly build (`sktv` subtarget) targeting
  `rad_sktv` only, with a browser UI that auto-fetches the ROM, a dev
  server with the required COOP/COEP headers, and assorted Emscripten 6.x
  compatibility fixes.
- Added `build-wasm.sh` and `build-native.sh` convenience scripts so the
  WASM and native `tvgames` builds can each be run with a single command.


## License

Nothing is changed from the original repository. This repository uses same license.
Same terms apply.
