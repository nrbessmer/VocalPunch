/*
  ==============================================================================
    VocalPunch - VocalPresets.h
    (c)(p) 2026 Tully EDM Vibe

    Each preset is a complete vocal chain. Keys not listed return to their
    defaults, so every preset sounds the same no matter what was loaded before.
  ==============================================================================
*/
#pragma once
#include "SynthParams.h"

namespace vpunch
{
    inline const std::vector<tsyn::Preset>& presets()
    {
        static const std::vector<tsyn::Preset> p = {
            // ---------------- Pop
            { "Pop Shine", "Pop", "Bright, polished lead vocal: airy top, controlled dynamics, short slap + plate.",
              "hpf=90 deess=0.45 eq_mud=-2 eq_pres=2.5 eq_air=4 comp=0.6 comp_ratio=4 comp_att=3 comp_rel=100 sat_type=Tape sat=0.2 doubler=0.12 delay=0.1 delay_time=1/8D delay_fb=0.25 reverb=0.16 reverb_size=0.55 reverb_pre=30 duck=0.6" },
            { "Radio Ready", "Pop", "Loud, upfront and consistent: the vocal sits on top of a dense mix.",
              "hpf=100 deess=0.55 eq_mud=-3 eq_pres=3.5 eq_air=3.5 comp=0.8 comp_ratio=6 comp_att=1.5 comp_rel=80 sat_type=Tape sat=0.3 doubler=0.1 reverb=0.1 reverb_size=0.4 duck=0.6" },
            { "Intimate Close", "Pop", "Close-mic, breathy and warm. Little reverb so every detail is heard.",
              "hpf=70 deess=0.4 eq_low=1.5 eq_mud=-1.5 eq_pres=1 eq_air=3 comp=0.55 comp_ratio=3 comp_att=8 comp_rel=150 sat_type=Tube sat=0.18 reverb=0.08 reverb_size=0.3 reverb_pre=10" },
            { "Big Chorus Lead", "Pop", "Wide, doubled hook vocal with a big hall behind it.",
              "hpf=100 deess=0.5 eq_mud=-2.5 eq_pres=2.5 eq_air=4.5 comp=0.7 comp_ratio=5 comp_att=3 comp_rel=100 sat_type=Tape sat=0.25 doubler=0.55 dbl_spread=0.45 delay=0.14 delay_time=1/4 delay_fb=0.3 reverb=0.25 reverb_size=0.75 reverb_pre=40 duck=0.6" },
            { "Ballad Glow", "Pop", "Smooth and lush for slow songs: gentle compression, long warm reverb.",
              "hpf=75 deess=0.4 eq_low=1 eq_mud=-1.5 eq_pres=1.5 eq_air=3 comp=0.45 comp_ratio=3 comp_att=10 comp_rel=200 sat_type=Tube sat=0.12 delay=0.12 delay_time=1/4D delay_fb=0.35 reverb=0.32 reverb_size=0.85 reverb_pre=50 duck=0.4" },

            // ---------------- Hip-Hop & Trap
            { "Rap Upfront", "Hip-Hop & Trap", "Dry, punchy and in your face: every word cuts through the beat.",
              "hpf=110 deess=0.5 eq_mud=-3 eq_pres=4 eq_air=2.5 comp=0.85 comp_ratio=8 comp_att=1 comp_rel=60 sat_type=Tube sat=0.3 reverb=0.05 reverb_size=0.25 duck=0.7" },
            { "Trap Lead", "Hip-Hop & Trap", "Modern trap vocal: bright, compressed, with a dotted 1/8 throw.",
              "hpf=110 deess=0.55 eq_mud=-2.5 eq_pres=3 eq_air=5 comp=0.8 comp_ratio=6 comp_att=2 comp_rel=80 sat_type=Tape sat=0.3 doubler=0.15 delay=0.2 delay_time=1/8D delay_fb=0.35 reverb=0.14 reverb_size=0.5 duck=0.8" },
            { "Melodic Trap", "Hip-Hop & Trap", "Spacious melodic trap: wide doubles, long delays, dark plate.",
              "hpf=100 deess=0.55 eq_mud=-2 eq_pres=2 eq_air=4.5 comp=0.75 comp_ratio=5 comp_att=3 comp_rel=100 sat_type=Tape sat=0.25 doubler=0.45 delay=0.26 delay_time=1/4 delay_fb=0.45 reverb=0.24 reverb_size=0.7 reverb_pre=30 duck=0.8 tone=0.1" },
            { "Ad-Lib Throw", "Hip-Hop & Trap", "For ad-libs: thinner, wider and washed in ping-pong echoes.",
              "hpf=200 deess=0.5 eq_low=-3 eq_mud=-3 eq_pres=3 eq_air=4 comp=0.8 comp_ratio=6 sat_type=Fuzz sat=0.2 doubler=0.6 dbl_spread=0.6 delay=0.35 delay_time=1/8 delay_fb=0.45 reverb=0.25 reverb_size=0.6 duck=0.5" },
            { "Boom Bap Grit", "Hip-Hop & Trap", "Gritty 90s rap vocal through a dusty preamp.",
              "hpf=120 deess=0.4 eq_mud=-1 eq_pres=3 eq_air=-1 comp=0.8 comp_ratio=8 comp_att=4 comp_rel=90 sat_type=Tube sat=0.5 lofi=0.12 reverb=0.08 reverb_size=0.3 tone=-0.2" },
            { "Drill Dark", "Hip-Hop & Trap", "Dark, aggressive drill vocal with a tight slap.",
              "hpf=110 deess=0.5 eq_low=1 eq_mud=-2 eq_pres=3 eq_air=1 comp=0.85 comp_ratio=8 comp_att=1.5 comp_rel=70 sat_type=Fuzz sat=0.25 delay=0.12 delay_time=1/16 delay_fb=0.2 reverb=0.08 reverb_size=0.35 tone=-0.25" },

            // ---------------- R&B & Soul
            { "Silky R&B", "R&B & Soul", "Silky lead: rounded lows, sweet air, smooth plate and 1/4 delay.",
              "hpf=80 deess=0.5 eq_low=1 eq_mud=-2 eq_pres=1.5 eq_air=4 comp=0.6 comp_ratio=4 comp_att=6 comp_rel=150 sat_type=Tube sat=0.15 doubler=0.15 delay=0.16 delay_time=1/4 delay_fb=0.35 reverb=0.24 reverb_size=0.65 reverb_pre=35 duck=0.6" },
            { "Neo Soul Warm", "R&B & Soul", "Warm vintage soul tone with tape character.",
              "hpf=70 deess=0.35 eq_low=2 eq_mud=-1 eq_pres=1.5 eq_air=1 comp=0.55 comp_ratio=3 comp_att=10 comp_rel=180 sat_type=Tape sat=0.4 reverb=0.18 reverb_size=0.5 tone=-0.3" },
            { "Falsetto Air", "R&B & Soul", "Lifts thin falsetto lines: more body, extra air, wide doubles.",
              "hpf=90 deess=0.6 eq_low=2.5 eq_mud=-1 eq_pres=1 eq_air=5 comp=0.65 comp_ratio=4 comp_att=5 comp_rel=120 sat_type=Tube sat=0.18 doubler=0.35 reverb=0.28 reverb_size=0.75 reverb_pre=40 duck=0.5" },
            { "Gospel Stack", "R&B & Soul", "Big, wide ensemble sound for stacked harmonies.",
              "hpf=120 deess=0.5 eq_mud=-3 eq_pres=1 eq_air=3 comp=0.7 comp_ratio=4 sat_type=Tape sat=0.2 doubler=0.8 dbl_spread=0.55 reverb=0.3 reverb_size=0.8 reverb_pre=30 duck=0.3" },

            // ---------------- Rock & Indie
            { "Rock Lead", "Rock & Indie", "Driven rock vocal that stands up to loud guitars.",
              "hpf=110 deess=0.45 eq_mud=-2.5 eq_pres=4 eq_air=2 comp=0.8 comp_ratio=8 comp_att=2 comp_rel=80 sat_type=Tube sat=0.45 delay=0.12 delay_time=1/8 delay_fb=0.2 reverb=0.12 reverb_size=0.45 duck=0.6" },
            { "Indie Slapback", "Rock & Indie", "Classic slapback echo with a bit of tape grit.",
              "hpf=100 deess=0.4 eq_mud=-1.5 eq_pres=2 eq_air=1.5 comp=0.6 comp_ratio=4 sat_type=Tape sat=0.35 delay=0.28 delay_time=1/16 delay_fb=0.1 delay_pp=0 reverb=0.08 reverb_size=0.35" },
            { "Grunge Scream", "Rock & Indie", "Heavy distortion for screams and gang shouts.",
              "hpf=140 deess=0.3 eq_mud=-3 eq_pres=5 eq_air=0 comp=0.9 comp_ratio=12 comp_att=1 comp_rel=60 sat_type=Fuzz sat=0.7 reverb=0.12 reverb_size=0.4 tone=-0.1" },
            { "Dream Pop Haze", "Rock & Indie", "Hazy, washed-out vocal floating in delay and reverb.",
              "hpf=120 deess=0.5 eq_mud=-2 eq_pres=-1 eq_air=4 comp=0.6 comp_ratio=4 sat_type=Tape sat=0.25 doubler=0.5 delay=0.35 delay_time=1/4D delay_fb=0.55 reverb=0.45 reverb_size=0.95 reverb_pre=60 duck=0.3" },

            // ---------------- EDM & Dance
            { "EDM Topline", "EDM & Dance", "Festival topline: bright, wide, ducked delays that bloom between phrases.",
              "hpf=120 deess=0.55 eq_mud=-3 eq_pres=3 eq_air=5 comp=0.8 comp_ratio=6 comp_att=2 comp_rel=90 sat_type=Tape sat=0.25 doubler=0.35 delay=0.28 delay_time=1/8D delay_fb=0.4 reverb=0.28 reverb_size=0.75 reverb_pre=40 duck=0.9" },
            { "House Diva", "EDM & Dance", "Soulful house vocal with a lush plate and 1/4 echo.",
              "hpf=110 deess=0.5 eq_mud=-2 eq_pres=2.5 eq_air=4 comp=0.7 comp_ratio=5 comp_att=4 comp_rel=120 sat_type=Tube sat=0.2 doubler=0.2 delay=0.2 delay_time=1/4 delay_fb=0.35 reverb=0.28 reverb_size=0.7 duck=0.8" },
            { "Trance Angel", "EDM & Dance", "Ethereal trance vocal: huge hall, long ping-pong, extra air.",
              "hpf=140 deess=0.6 eq_mud=-3 eq_pres=1.5 eq_air=6 comp=0.7 comp_ratio=5 sat_type=Tape sat=0.15 doubler=0.45 delay=0.32 delay_time=1/4D delay_fb=0.5 reverb=0.4 reverb_size=0.95 reverb_pre=60 duck=0.85 tone=0.2" },
            { "Tech House Chop", "EDM & Dance", "Short, punchy vocal chops with a tight 1/16 bounce.",
              "hpf=150 deess=0.5 eq_low=-2 eq_mud=-3 eq_pres=4 eq_air=3 comp=0.85 comp_ratio=8 comp_att=1 comp_rel=60 sat_type=Tube sat=0.35 delay=0.22 delay_time=1/16 delay_fb=0.3 reverb=0.1 reverb_size=0.3 duck=0.6" },
            { "Future Bass Vox", "EDM & Dance", "Glossy, wide future-bass vocal with crushed shimmer.",
              "hpf=130 deess=0.55 eq_mud=-2.5 eq_pres=2 eq_air=5 comp=0.8 comp_ratio=6 sat_type=Crush sat=0.18 doubler=0.55 delay=0.24 delay_time=1/8D delay_fb=0.4 reverb=0.3 reverb_size=0.8 duck=0.85 tone=0.15" },

            // ---------------- Creative & Lo-Fi
            { "Telephone", "Creative & Lo-Fi", "Classic phone-line effect: narrow band and crunchy.",
              "hpf=300 deess=0.2 comp=0.8 comp_ratio=8 sat_type=Fuzz sat=0.35 lofi=1 reverb=0.04 reverb_size=0.2" },
            { "Old Radio", "Creative & Lo-Fi", "AM-radio tone with tube warmth and a small room.",
              "hpf=200 deess=0.3 eq_pres=2 comp=0.75 comp_ratio=6 sat_type=Tube sat=0.5 lofi=0.75 reverb=0.12 reverb_size=0.3 tone=-0.2" },
            { "Megaphone", "Creative & Lo-Fi", "Loud-hailer distortion for chants and hooks.",
              "hpf=500 deess=0.2 eq_pres=6 comp=0.9 comp_ratio=12 sat_type=Fuzz sat=0.75 lofi=0.85 reverb=0.06" },
            { "Lo-Fi Bedroom", "Creative & Lo-Fi", "Soft, dusty bedroom-pop vocal with bit-crushed edges.",
              "hpf=120 deess=0.4 eq_low=1 eq_pres=-1 eq_air=-2 comp=0.6 comp_ratio=4 sat_type=Crush sat=0.3 lofi=0.25 doubler=0.2 delay=0.15 delay_time=1/4 delay_fb=0.35 reverb=0.2 reverb_size=0.5 tone=-0.4" },
            { "Robot Crush", "Creative & Lo-Fi", "Heavy bit-crush and sample-rate reduction for robotic hooks.",
              "hpf=150 deess=0.2 comp=0.8 comp_ratio=8 sat_type=Crush sat=0.85 doubler=0.3 delay=0.15 delay_time=1/16 delay_fb=0.3 reverb=0.1" },
            { "Ghost Whisper", "Creative & Lo-Fi", "Distant, wide whisper layer drowned in reverb.",
              "hpf=250 deess=0.7 eq_low=-4 eq_mud=-3 eq_air=6 comp=0.7 comp_ratio=5 sat_type=Tape sat=0.1 doubler=0.9 dbl_spread=0.8 delay=0.25 delay_time=1/4D delay_fb=0.55 reverb=0.6 reverb_size=1 reverb_pre=80 duck=0.2 mix=1" },
            { "Infinite Wash", "Creative & Lo-Fi", "Ambient tail: vocal turns into a long pad of echoes.",
              "hpf=150 deess=0.5 eq_mud=-2 eq_air=3 comp=0.6 comp_ratio=4 doubler=0.5 delay=0.5 delay_time=1/2 delay_fb=0.75 reverb=0.7 reverb_size=1 reverb_pre=100 duck=0.2" },

            // ---------------- Spoken & Podcast
            { "Podcast Clean", "Spoken & Podcast", "Clear, even speech: removes rumble and room, tames S's, no effects.",
              "hpf=85 gate=-55 deess=0.5 eq_low=1 eq_mud=-2.5 eq_pres=2.5 eq_air=1.5 comp=0.7 comp_ratio=4 comp_att=5 comp_rel=150 sat_type=Tube sat=0.1 reverb=0 delay=0 duck=0" },
            { "Broadcast Voice", "Spoken & Podcast", "Rich, close 'radio host' voice with strong compression.",
              "hpf=70 gate=-50 deess=0.55 eq_low=3 eq_mud=-2 eq_pres=2 eq_air=2 comp=0.85 comp_ratio=6 comp_att=3 comp_rel=120 sat_type=Tube sat=0.2 reverb=0 delay=0" },
            { "Voice-Over Crisp", "Spoken & Podcast", "Crisp, articulate narration for video and ads.",
              "hpf=90 gate=-55 deess=0.5 eq_mud=-3 eq_pres=3 eq_air=3 comp=0.75 comp_ratio=5 sat_type=Tape sat=0.1 reverb=0.03 reverb_size=0.2" },
            { "Streamer Mic", "Spoken & Podcast", "Fixes a cheap mic: gate, low cut, warmth and presence.",
              "hpf=110 gate=-48 deess=0.6 eq_low=2 eq_mud=-3 eq_pres=3 eq_air=2 comp=0.8 comp_ratio=6 sat_type=Tube sat=0.15 reverb=0 delay=0" },

            // ---------------- Backing & Doubles
            { "Backing Wide", "Backing & Doubles", "Pushes backing vocals out to the sides and behind the lead.",
              "hpf=150 deess=0.6 eq_low=-2 eq_mud=-3 eq_pres=-1 eq_air=3 comp=0.75 comp_ratio=5 doubler=0.7 dbl_spread=0.55 reverb=0.3 reverb_size=0.7 duck=0.3 tone=0.1" },
            { "Harmony Pad", "Backing & Doubles", "Soft, dark harmony layer that sits under the lead.",
              "hpf=150 deess=0.6 eq_mud=-2 eq_pres=-3 eq_air=1 comp=0.7 comp_ratio=4 sat_type=Tape sat=0.15 doubler=0.6 reverb=0.4 reverb_size=0.85 reverb_pre=40 tone=-0.3" },
            { "Double Tracked", "Backing & Doubles", "Instant double-tracking: a second take in each ear.",
              "hpf=100 deess=0.45 eq_mud=-1.5 eq_pres=1.5 eq_air=2.5 comp=0.6 comp_ratio=4 sat_type=Tape sat=0.15 doubler=1 dbl_spread=0.5 reverb=0.12 reverb_size=0.5" },
            { "Gang Vocals", "Backing & Doubles", "Crowd-style gang shouts: wide, driven and roomy.",
              "hpf=160 deess=0.4 eq_mud=-3 eq_pres=3 comp=0.85 comp_ratio=8 sat_type=Tube sat=0.4 doubler=1 dbl_spread=0.9 reverb=0.22 reverb_size=0.55" },

            // ---------------- Utility
            { "Clean Start", "Utility", "Gentle starting point: low cut, light de-ess and compression, no effects.",
              "hpf=80 deess=0.3 comp=0.4 comp_ratio=3 sat=0 reverb=0 delay=0 duck=0.5" },
            { "Bypass Flat", "Utility", "Everything off. Build your own chain from scratch.",
              "hpf=20 deess=0 comp=0 sat=0 reverb=0 delay=0 doubler=0 duck=0" },
        };
        return p;
    }
}
