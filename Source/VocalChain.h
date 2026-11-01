/*
  ==============================================================================
    VocalPunch - VocalChain.h
    (c)(p) 2026 Tully EDM Vibe

    The complete vocal chain: input trim > high-pass > gate > de-esser > EQ +
    tone tilt > compressor (auto make-up) > saturation > lo-fi band > doubler >
    tempo delay + reverb (ducked by the vocal) > dry/wet > output.
    Four macros (PUNCH, TONE, WIDTH, SPACE) scale the chain the way a one-knob
    vocal plugin does, while every module stays editable.
  ==============================================================================
*/
#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

#include "TullyDSP.h"
#include "SynthParams.h"   // tsyn::noteValues(), Preset, applyPreset

namespace vpunch
{
    inline juce::StringArray satTypes() { return { "Tape", "Tube", "Fuzz", "Crush" }; }
    inline juce::StringArray delayNames() { juce::StringArray s; for (auto& n : tsyn::noteValues()) s.add (n.name); return s; }

    inline void addParameters (juce::AudioProcessorValueTreeState::ParameterLayout& l)
    {
        using namespace juce;
        auto pct = AudioParameterFloatAttributes().withStringFromValueFunction ([] (float v, int) { return String (roundToInt (v * 100.0f)) + "%"; });
        auto bip = AudioParameterFloatAttributes().withStringFromValueFunction ([] (float v, int) { return (v > 0.004f ? "+" : "") + String (roundToInt (v * 100.0f)) + "%"; });
        auto db  = AudioParameterFloatAttributes().withStringFromValueFunction ([] (float v, int) { return (v > 0.04f ? "+" : "") + String (v, 1) + " dB"; });
        auto hz  = AudioParameterFloatAttributes().withStringFromValueFunction ([] (float v, int) { return v >= 1000.0f ? String (v / 1000.0f, 1) + " kHz" : String (roundToInt (v)) + " Hz"; });
        auto ms  = AudioParameterFloatAttributes().withStringFromValueFunction ([] (float v, int) { return v < 10.0f ? String (v, 1) + " ms" : String (roundToInt (v)) + " ms"; });
        auto gate = AudioParameterFloatAttributes().withStringFromValueFunction ([] (float v, int) { return v <= -79.5f ? String ("Off") : String (roundToInt (v)) + " dB"; });
        auto ratio = AudioParameterFloatAttributes().withStringFromValueFunction ([] (float v, int) { return String (v, v < 10 ? 1 : 0) + ":1"; });
        auto f = [&l] (const char* id, const char* name, NormalisableRange<float> r, float def, AudioParameterFloatAttributes a)
        { l.add (std::make_unique<AudioParameterFloat> (ParameterID { id, 1 }, name, r, def, a)); };
        auto skewed = [] (float lo, float hi, float centre) { NormalisableRange<float> r (lo, hi); r.setSkewForCentre (centre); return r; };

        // macros
        f ("punch", "Punch", { 0.0f, 2.0f }, 1.0f, pct);
        f ("tone", "Tone", { -1.0f, 1.0f }, 0.0f, bip);
        f ("width", "Width", { 0.0f, 2.0f }, 1.0f, pct);
        f ("space", "Space", { 0.0f, 2.0f }, 1.0f, pct);
        f ("mix", "Mix", { 0.0f, 1.0f }, 1.0f, pct);
        f ("out_gain", "Output", { -24.0f, 12.0f }, 0.0f, db);
        l.add (std::make_unique<AudioParameterBool> (ParameterID { "bypass", 1 }, "Bypass", false));

        // cleanup
        f ("in_gain", "Input", { -24.0f, 24.0f }, 0.0f, db);
        f ("hpf", "Low Cut", skewed (20.0f, 400.0f, 100.0f), 80.0f, hz);
        f ("gate", "Gate", { -80.0f, -20.0f }, -80.0f, gate);
        f ("deess", "De-ess", { 0.0f, 1.0f }, 0.3f, pct);
        f ("deess_freq", "De-ess Freq", skewed (3000.0f, 10000.0f, 6000.0f), 6500.0f, hz);
        // eq
        f ("eq_low", "Low", { -12.0f, 12.0f }, 0.0f, db);
        f ("eq_mud", "Mud", { -12.0f, 12.0f }, 0.0f, db);
        f ("eq_pres", "Presence", { -12.0f, 12.0f }, 0.0f, db);
        f ("eq_air", "Air", { -12.0f, 12.0f }, 0.0f, db);
        // compressor
        f ("comp", "Compress", { 0.0f, 1.0f }, 0.5f, pct);
        f ("comp_ratio", "Ratio", skewed (1.5f, 20.0f, 4.0f), 4.0f, ratio);
        f ("comp_att", "Attack", skewed (0.1f, 50.0f, 5.0f), 5.0f, ms);
        f ("comp_rel", "Release", skewed (20.0f, 500.0f, 120.0f), 120.0f, ms);
        // character
        l.add (std::make_unique<AudioParameterChoice> (ParameterID { "sat_type", 1 }, "Drive Type", satTypes(), 0));
        f ("sat", "Drive", { 0.0f, 1.0f }, 0.15f, pct);
        f ("lofi", "Lo-Fi", { 0.0f, 1.0f }, 0.0f, pct);
        // space
        f ("doubler", "Doubler", { 0.0f, 1.0f }, 0.0f, pct);
        f ("dbl_spread", "Detune", { 0.0f, 1.0f }, 0.4f, pct);
        f ("delay", "Delay", { 0.0f, 1.0f }, 0.0f, pct);
        l.add (std::make_unique<AudioParameterChoice> (ParameterID { "delay_time", 1 }, "Delay Time", delayNames(), 6));
        f ("delay_fb", "Feedback", { 0.0f, 0.9f }, 0.3f, pct);
        l.add (std::make_unique<AudioParameterBool> (ParameterID { "delay_pp", 1 }, "Ping-Pong", true));
        f ("reverb", "Reverb", { 0.0f, 1.0f }, 0.15f, pct);
        f ("reverb_size", "Size", { 0.0f, 1.0f }, 0.5f, pct);
        f ("reverb_pre", "Pre-Delay", { 0.0f, 120.0f }, 20.0f, ms);
        f ("duck", "Duck", { 0.0f, 1.0f }, 0.5f, pct);
    }

    //==========================================================================
    struct Settings
    {
        float punch = 1, tone = 0, width = 1, space = 1, mix = 1, outDb = 0; bool bypass = false;
        float inDb = 0, hpf = 80, gate = -80, deess = 0.3f, deessFreq = 6500;
        float eqLow = 0, eqMud = 0, eqPres = 0, eqAir = 0;
        float comp = 0.5f, ratio = 4, att = 5, rel = 120;
        int satType = 0; float sat = 0.15f, lofi = 0;
        float doubler = 0, spread = 0.4f, delay = 0; int delayTime = 6; float delayFb = 0.3f; bool pingPong = true;
        float reverb = 0.15f, size = 0.5f, pre = 20, duck = 0.5f;
    };

    class Reader
    {
    public:
        void attach (juce::AudioProcessorValueTreeState& s)
        {
            auto g = [&s] (const char* id) { auto* p = s.getRawParameterValue (id); jassert (p != nullptr); return p; };
            for (auto* id : { "punch", "tone", "width", "space", "mix", "out_gain", "bypass", "in_gain", "hpf", "gate", "deess", "deess_freq",
                              "eq_low", "eq_mud", "eq_pres", "eq_air", "comp", "comp_ratio", "comp_att", "comp_rel", "sat_type", "sat", "lofi",
                              "doubler", "dbl_spread", "delay", "delay_time", "delay_fb", "delay_pp", "reverb", "reverb_size", "reverb_pre", "duck" })
                p[id] = g (id);
        }
        Settings read() const
        {
            Settings s;
            auto v = [this] (const char* id) { return p.at (id)->load(); };
            s.punch = v ("punch"); s.tone = v ("tone"); s.width = v ("width"); s.space = v ("space"); s.mix = v ("mix"); s.outDb = v ("out_gain");
            s.bypass = v ("bypass") > 0.5f; s.inDb = v ("in_gain"); s.hpf = v ("hpf"); s.gate = v ("gate"); s.deess = v ("deess"); s.deessFreq = v ("deess_freq");
            s.eqLow = v ("eq_low"); s.eqMud = v ("eq_mud"); s.eqPres = v ("eq_pres"); s.eqAir = v ("eq_air");
            s.comp = v ("comp"); s.ratio = v ("comp_ratio"); s.att = v ("comp_att"); s.rel = v ("comp_rel");
            s.satType = (int) v ("sat_type"); s.sat = v ("sat"); s.lofi = v ("lofi");
            s.doubler = v ("doubler"); s.spread = v ("dbl_spread"); s.delay = v ("delay"); s.delayTime = (int) v ("delay_time"); s.delayFb = v ("delay_fb");
            s.pingPong = v ("delay_pp") > 0.5f; s.reverb = v ("reverb"); s.size = v ("reverb_size"); s.pre = v ("reverb_pre"); s.duck = v ("duck");
            return s;
        }
    private:
        std::map<std::string, std::atomic<float>*> p;
    };

    //==========================================================================
    class Chain
    {
    public:
        void prepare (double sampleRate, int maxBlock)
        {
            sr = sampleRate;
            const int maxDelay = (int) (sr * 2.6);
            for (auto& d : dly) d.prepare (maxDelay);
            for (auto& d : dbl) d.prepare ((int) (sr * 0.05));
            preDelay.prepare ((int) (sr * 0.13));
            dry.clear(); ensureBlock (juce::jmax (maxBlock, 4096));
            rvb.setSampleRate (sr);
            dcL.set (tdsp::Biquad::HighPass, sr, 12.0, 0.707); dcR = dcL;
            for (auto& b : fbHp) b.set (tdsp::Biquad::HighPass, sr, 250.0, 0.707);
            for (auto& b : fbLp) b.set (tdsp::Biquad::LowPass, sr, 5500.0, 0.707);
            revHp.set (tdsp::Biquad::HighPass, sr, 220.0, 0.707);
            lofiHp.set (tdsp::Biquad::HighPass, sr, 450.0, 0.9); lofiLp.set (tdsp::Biquad::LowPass, sr, 3200.0, 0.9);
            dblLp.set (tdsp::Biquad::LowPass, sr, 9000.0, 0.707);
            envFast = tdsp::timeCoef (1.0, sr); envSlow = tdsp::timeCoef (60.0, sr);
            gateEnvRel = tdsp::timeCoef (90.0, sr);
            duckAtt = tdsp::timeCoef (10.0, sr); duckRel = tdsp::timeCoef (350.0, sr);
            makeupCoef = tdsp::timeCoef (1500.0, sr);
            deessSm.setTimes (0.5, 40.0, sr);
            gateSm.setTimes (1.0, 60.0, sr);
            last = {}; last.hpf = -1;   // force coefficient update
            reset();
        }
        void reset()
        {
            for (auto* b : { &hp1, &hp2, &ds, &dsLp, &eLow, &eMud, &ePres, &eAir, &tLo, &tHi, &lofiHp, &lofiLp, &dcL, &dcR, &revHp, &dblLp }) b->reset();
            for (auto& b : fbHp) b.reset(); for (auto& b : fbLp) b.reset();
            for (auto& d : dly) d.reset(); for (auto& d : dbl) d.reset(); preDelay.reset();
            rvb.reset();
            dsEnvH = dsEnvF = gateEnv = duckEnv = 0; compGr.state = 0; deessSm.state = 0; gateSm.state = 0;
            avgGr = 0; makeup = 0; ph1 = 0; ph2 = 0.37; ph3 = 0.11; ph4 = 0.73; holdCount = 0; crushHold[0] = crushHold[1] = 0; crushCount = 0;
            delaySmooth = -1;
        }

        void process (float* L, float* R, int n, const Settings& s, double bpm)
        {
            ensureBlock (n);
            updateFilters (s);
            if (s.bypass) { meterIn.push (L, R, n); meterOut.push (L, R, n); compGrDb.store (0.0f); deessGrDb.store (0.0f); return; }

            const float inG = tdsp::dbToGain (s.inDb), outG = tdsp::dbToGain (s.outDb);
            const float comp = juce::jlimit (0.0f, 1.0f, s.comp * s.punch);
            const float sat = juce::jlimit (0.0f, 1.0f, s.sat * (0.4f + 0.6f * s.punch));
            const float thr = -8.0f - 28.0f * comp;
            compGr.setTimes (s.att, s.rel, sr);
            const float dblAmt = juce::jlimit (0.0f, 1.5f, s.doubler * s.width);
            const float delMix = juce::jlimit (0.0f, 1.0f, s.delay * s.space), revMix = juce::jlimit (0.0f, 1.0f, s.reverb * s.space);
            const float duck = s.duck;
            // doubler detune depth: cents -> modulation depth in samples
            const double cents = 3.0 + 22.0 * s.spread;
            const double f1 = 0.37, f2 = 0.53;
            const double depth1 = (std::pow (2.0, cents / 1200.0) - 1.0) / (2.0 * tdsp::kPi * f1) * sr;
            const double depth2 = (std::pow (2.0, cents / 1200.0) - 1.0) / (2.0 * tdsp::kPi * f2) * sr;
            const double base1 = 0.012 * sr + depth1, base2 = 0.019 * sr + depth2;
            const double inc1 = f1 / sr, inc2 = f2 / sr, inc3 = 0.11 / sr, inc4 = 0.07 / sr;
            // tempo delay
            const double beats = tsyn::noteValues()[(size_t) juce::jlimit (0, (int) tsyn::noteValues().size() - 1, s.delayTime)].beats;
            const double targetDelay = juce::jlimit (1.0, sr * 2.5, beats * 60.0 / juce::jlimit (30.0, 300.0, bpm) * sr);
            if (delaySmooth < 0) delaySmooth = targetDelay;
            const float preSamples = (float) (s.pre * 0.001 * sr);
            const int crushStep = 1 + (int) (sat * 7.0f);
            const float crushLevels = std::pow (2.0f, 14.0f - 10.0f * sat);
            const float drive = 1.0f + 14.0f * std::pow (sat, 1.5f);
            const float gateThr = s.gate;
            const float dsAmt = s.deess;

            float maxComp = 0, maxDs = 0;
            meterIn.pushScaled (L, R, n, inG);

            for (int i = 0; i < n; ++i)
            {
                float l = L[i] * inG, r = R[i] * inG;
                const float dryL = l, dryR = r;
                // high-pass (24 dB/oct)
                l = hp2.process (hp1.process (l, 0), 0); r = hp2.process (hp1.process (r, 1), 1);
                // gate
                if (gateThr > -79.5f)
                {
                    const float a = juce::jmax (std::abs (l), std::abs (r));
                    gateEnv = a > gateEnv ? a : a + gateEnvRel * (gateEnv - a);
                    const bool open = tdsp::gainToDb (gateEnv) > gateThr;
                    if (open) holdCount = (int) (0.05 * sr); else if (holdCount > 0) --holdCount;
                    const float g = gateSm.process (open || holdCount > 0 ? 0.0f : -30.0f);
                    const float gg = tdsp::dbToGain (g); l *= gg; r *= gg;
                }
                // de-esser (split band: only the top band is turned down)
                {
                    const float hl = ds.process (l, 0), hr = ds.process (r, 1);
                    const float ah = juce::jmax (std::abs (hl), std::abs (hr)), af = juce::jmax (std::abs (l), std::abs (r));
                    dsEnvH = ah > dsEnvH ? ah + envFast * (dsEnvH - ah) : ah + envSlow * (dsEnvH - ah);
                    dsEnvF = af > dsEnvF ? af + envFast * (dsEnvF - af) : af + envSlow * (dsEnvF - af);
                    float target = 0.0f;
                    if (dsAmt > 0.0f && dsEnvH > 0.003f)
                    {
                        const float excess = tdsp::gainToDb (dsEnvH) - tdsp::gainToDb (dsEnvF) + 9.0f;
                        target = -juce::jlimit (0.0f, 16.0f, excess * 1.6f) * dsAmt;
                    }
                    const float gr = deessSm.process (target);
                    maxDs = juce::jmin (maxDs, gr);
                    const float g = tdsp::dbToGain (gr);
                    // complementary split: top band = input minus a low-pass, so low + top == input exactly
                    const float tl = l - dsLp.process (l, 0), tr = r - dsLp.process (r, 1);
                    l -= tl * (1.0f - g); r -= tr * (1.0f - g);
                }
                // EQ + tone tilt
                l = tHi.process (tLo.process (eAir.process (ePres.process (eMud.process (eLow.process (l, 0), 0), 0), 0), 0), 0);
                r = tHi.process (tLo.process (eAir.process (ePres.process (eMud.process (eLow.process (r, 1), 1), 1), 1), 1), 1);
                // compressor with slow automatic make-up
                if (comp > 0.001f)
                {
                    const float xDb = tdsp::gainToDb (juce::jmax (std::abs (l), std::abs (r)));
                    const float gr = compGr.process (tdsp::compGainDb (xDb, thr, s.ratio, 6.0f));
                    if (xDb > -45.0f) avgGr = gr + makeupCoef * (avgGr - gr);
                    makeup = -0.8f * avgGr;
                    const float g = tdsp::dbToGain (gr + makeup);
                    l *= g; r *= g; maxComp = juce::jmin (maxComp, gr);
                }
                // saturation
                if (sat > 0.001f)
                {
                    if (s.satType == 3)
                    {
                        if (++crushCount >= crushStep) { crushCount = 0; crushHold[0] = std::round (l * crushLevels) / crushLevels; crushHold[1] = std::round (r * crushLevels) / crushLevels; }
                        l = l + sat * (crushHold[0] - l); r = r + sat * (crushHold[1] - r);
                    }
                    else
                    {
                        const float lw = shape (s.satType, l * drive) / drive, rw = shape (s.satType, r * drive) / drive;
                        const float comp2 = 1.0f + 0.9f * sat;
                        l = l + sat * (lw * comp2 - l); r = r + sat * (rw * comp2 - r);
                    }
                    l = dcL.process (l); r = dcR.process (r);
                }
                // lo-fi band
                if (s.lofi > 0.001f)
                {
                    const float bl = lofiLp.process (lofiHp.process (l, 0), 0) * 1.6f, br = lofiLp.process (lofiHp.process (r, 1), 1) * 1.6f;
                    l += s.lofi * (bl - l); r += s.lofi * (br - r);
                }
                // doubler: two detuned, delayed copies panned left and right
                const float mono = 0.5f * (l + r);
                if (dblAmt > 0.001f)
                {
                    const float m = dblLp.process (mono);
                    dbl[0].push (m); dbl[1].push (m);
                    const float d1 = (float) (base1 + depth1 * (0.8 * std::sin (2 * tdsp::kPi * ph1) + 0.2 * std::sin (2 * tdsp::kPi * ph3)));
                    const float d2 = (float) (base2 + depth2 * (0.8 * std::sin (2 * tdsp::kPi * ph2) + 0.2 * std::sin (2 * tdsp::kPi * ph4)));
                    const float vL = dbl[0].read (d1), vR = dbl[1].read (d2);
                    const float k = 0.75f * dblAmt;
                    l = l * (1.0f - 0.15f * dblAmt) + k * vL; r = r * (1.0f - 0.15f * dblAmt) + k * vR;
                }
                ph1 += inc1; ph2 += inc2; ph3 += inc3; ph4 += inc4;
                if (ph1 > 1) ph1 -= 1; if (ph2 > 1) ph2 -= 1; if (ph3 > 1) ph3 -= 1; if (ph4 > 1) ph4 -= 1;

                // ducking envelope follows the processed vocal
                const float a = std::abs (mono);
                duckEnv = a > duckEnv ? a + duckAtt * (duckEnv - a) : a + duckRel * (duckEnv - a);
                const float duckG = tdsp::dbToGain (-duck * 12.0f * juce::jlimit (0.0f, 1.0f, (tdsp::gainToDb (duckEnv) + 42.0f) / 24.0f));

                // tempo delay
                float wl = 0, wr = 0;
                delaySmooth += 0.0005 * (targetDelay - delaySmooth);
                {
                    const float yl = dly[0].read ((float) delaySmooth), yr = dly[1].read ((float) delaySmooth);
                    const float fl = fbLp[0].process (fbHp[0].process (yl)), fr = fbLp[1].process (fbHp[1].process (yr));
                    const float send = delMix > 0.0f ? mono : 0.0f;
                    if (s.pingPong) { dly[0].push (send + s.delayFb * fr); dly[1].push (s.delayFb * fl); }
                    else            { dly[0].push (send + s.delayFb * fl); dly[1].push (send + s.delayFb * fr); }
                    wl += yl * delMix * duckG; wr += yr * delMix * duckG;
                }
                // reverb send (pre-delayed, low end removed); processed after the loop
                preDelay.push (revHp.process (mono));
                const float pd = preSamples < 1.0f ? mono : preDelay.read (preSamples);
                reverbIn[(size_t) i] = pd * revMix * duckG;
                // wet signal for this sample (reverb added below)
                L[i] = l + wl; R[i] = r + wr;
                dryBufL (i) = dryL; dryBufR (i) = dryR;
            }

            // reverb (juce::Reverb is block based)
            if (revMix > 0.0001f || rvbActive)
            {
                juce::Reverb::Parameters rp;
                rp.roomSize = 0.45f + 0.5f * s.size; rp.damping = 0.5f - 0.2f * s.size; rp.wetLevel = 1.0f; rp.dryLevel = 0.0f; rp.width = 1.0f;
                rvb.setParameters (rp);
                float* rl = reverbIn.get(); float* rr = reverbIn.get() + n;
                std::copy (rl, rl + n, rr);
                rvb.processStereo (rl, rr, n);
                float e = 0;
                for (int i = 0; i < n; ++i) { L[i] += 0.55f * rl[i]; R[i] += 0.55f * rr[i]; e = juce::jmax (e, std::abs (rl[i])); }
                rvbActive = revMix > 0.0001f || e > 1e-5f;
            }

            // dry/wet, output trim, gentle safety clip
            for (int i = 0; i < n; ++i)
            {
                float l = s.mix * L[i] + (1.0f - s.mix) * dryBufL (i);
                float r = s.mix * R[i] + (1.0f - s.mix) * dryBufR (i);
                L[i] = safety (l * outG); R[i] = safety (r * outG);
            }
            compGrDb.store (maxComp); deessGrDb.store (maxDs);
            meterOut.push (L, R, n);
        }

        // UI / tests
        struct InMeter : tdsp::Meter
        {
            void pushScaled (const float* l, const float* r, int n, float g) noexcept
            {
                float a = 0, b = 0;
                for (int i = 0; i < n; ++i) { a = juce::jmax (a, std::abs (l[i])); b = juce::jmax (b, std::abs (r[i])); }
                peakL.store (juce::jmax (tdsp::gainToDb (a * g), peakL.load() - 1.5f));
                peakR.store (juce::jmax (tdsp::gainToDb (b * g), peakR.load() - 1.5f));
            }
        } meterIn;
        tdsp::Meter meterOut;
        std::atomic<float> compGrDb { 0.0f }, deessGrDb { 0.0f };

        // EQ curve for the display (magnitude in dB)
        double eqResponseDb (double f) const
        {
            double m = 0;
            for (auto* b : { &hp1, &hp2, &eLow, &eMud, &ePres, &eAir, &tLo, &tHi }) m += b->magnitudeDb (f, sr);
            return m;
        }
        double getSampleRate() const { return sr; }

    private:
        static float shape (int type, float u) noexcept
        {
            switch (type)
            {
                case 0: return std::tanh (u + 0.08f * u * u * (u > 0 ? 1.0f : -0.5f));                  // tape: soft, slight asymmetry
                case 1: return u >= 0 ? std::tanh (u) : std::tanh (0.6f * u) / 0.6f * 0.8f;              // tube: asymmetric, even harmonics
                default: return juce::jlimit (-1.0f, 1.0f, u * (1.5f - 0.5f * juce::jmin (1.0f, u * u)));   // fuzz: hard
            }
        }
        static float safety (float x) noexcept
        {
            const float t = 0.9f, a = std::abs (x);
            if (a <= t) return x;
            const float y = t + (1.0f - t) * std::tanh ((a - t) / (1.0f - t));
            return x < 0 ? -y : y;
        }
        float& dryBufL (int i) { return dry[(size_t) i]; }
        float& dryBufR (int i) { return dry[(size_t) i + dryHalf]; }
    public:
        void ensureBlock (int n) { if ((int) dry.size() < 2 * n) { dry.assign ((size_t) n * 2, 0.0f); dryHalf = (size_t) n; reverbIn.allocate ((size_t) n * 2, true); } }
    private:
        void updateFilters (const Settings& s)
        {
            const float pf = 0.55f + 0.45f * s.punch;   // PUNCH also scales the EQ moves
            if (s.hpf != last.hpf) { hp1.set (tdsp::Biquad::HighPass, sr, s.hpf, 0.541); hp2.set (tdsp::Biquad::HighPass, sr, s.hpf, 1.307); }
            if (s.deessFreq != last.deessFreq || last.hpf < 0) { ds.set (tdsp::Biquad::HighPass, sr, s.deessFreq, 0.707); dsLp.set (tdsp::Biquad::LowPass, sr, s.deessFreq * 0.7, 0.707); }
            if (s.eqLow != last.eqLow || s.punch != last.punch || last.hpf < 0) eLow.set (tdsp::Biquad::LowShelf, sr, 140.0, 0.707, s.eqLow * pf);
            if (s.eqMud != last.eqMud || s.punch != last.punch || last.hpf < 0) eMud.set (tdsp::Biquad::Peak, sr, 350.0, 1.0, s.eqMud * pf);
            if (s.eqPres != last.eqPres || s.punch != last.punch || last.hpf < 0) ePres.set (tdsp::Biquad::Peak, sr, 3200.0, 0.8, s.eqPres * pf);
            if (s.eqAir != last.eqAir || s.punch != last.punch || last.hpf < 0) eAir.set (tdsp::Biquad::HighShelf, sr, 11000.0, 0.707, s.eqAir * pf);
            if (s.tone != last.tone || last.hpf < 0)
            {
                tLo.set (tdsp::Biquad::LowShelf, sr, 250.0, 0.6, -3.5 * s.tone);
                tHi.set (tdsp::Biquad::HighShelf, sr, 2800.0, 0.6, 5.0 * s.tone);
            }
            last = s;
        }

        double sr = 48000.0;
        Settings last;
        tdsp::Biquad hp1, hp2, ds, dsLp, eLow, eMud, ePres, eAir, tLo, tHi, lofiHp, lofiLp, dcL, dcR, revHp, dblLp;
        tdsp::Biquad fbHp[2], fbLp[2];
        tdsp::DelayLine dly[2], dbl[2], preDelay;
        juce::Reverb rvb; bool rvbActive = false;
        juce::HeapBlock<float> reverbIn;
        std::vector<float> dry; size_t dryHalf = 0;
        tdsp::GrSmoother compGr, deessSm, gateSm;
        float envFast = 0, envSlow = 0, gateEnvRel = 0, duckAtt = 0, duckRel = 0, makeupCoef = 0;
        float dsEnvH = 0, dsEnvF = 0, gateEnv = 0, duckEnv = 0, avgGr = 0, makeup = 0;
        double ph1 = 0, ph2 = 0, ph3 = 0, ph4 = 0, delaySmooth = -1;
        int holdCount = 0, crushCount = 0; float crushHold[2] { 0, 0 };
    };
}
