# VocalPunch
(c)(p) 2026 Tully EDM Vibe - info@tullyedmvibe.com - www.tullyedmvibe.com

One-knob vocal chains. Pick a style, then shape it with four macros:
PUNCH (compression, drive and EQ shine), TONE (darker / brighter), WIDTH (doubler) and SPACE (delay + reverb).
Every module stays editable: low cut, gate, de-esser, 4-band EQ, compressor with auto make-up,
Tape / Tube / Fuzz / Crush drive, lo-fi band, doubler, tempo-synced ping-pong delay, reverb and ducking.

41 presets: Pop, Hip-Hop & Trap, R&B & Soul, Rock & Indie, EDM & Dance, Creative & Lo-Fi, Spoken & Podcast, Backing & Doubles, Utility.
AU, VST3 and standalone app for macOS 12+ (Apple Silicon + Intel). Zero latency.

Build: cmake -S . -B build -G Xcode && cmake --build build --config Release
Tests: cmake -S . -B build-test -DVP_TESTS=ON && cmake --build build-test --target VPTest
       VPTest vox_sung.wav vox_rap.wav outDir
