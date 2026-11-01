// VPTest: automated checks for VocalPunch (DSP, presets, state, editor).
// Usage: VPTest vox_sung.wav vox_rap.wav outDir
#include <juce_audio_utils/juce_audio_utils.h>
#include "../Source/PluginProcessor.h"
#include "../Source/PluginEditor.h"
#include <cstdio>

static int fails = 0, passes = 0;
static void check (bool ok, const juce::String& what)
{
    printf ("%s  %s\n", ok ? "PASS" : "FAIL", what.toRawUTF8());
    (ok ? passes : fails)++;
}
static juce::AudioBuffer<float> load (const juce::File& f)
{
    juce::AudioFormatManager fm; fm.registerBasicFormats();
    std::unique_ptr<juce::AudioFormatReader> r (fm.createReaderFor (f));
    juce::AudioBuffer<float> b (1, (int) r->lengthInSamples);
    r->read (&b, 0, (int) r->lengthInSamples, 0, true, false);
    return b;
}
static void setP (VocalPunchProcessor& p, const char* id, float v)
{
    auto* prm = p.apvts.getParameter (id);
    prm->setValueNotifyingHost (prm->convertTo0to1 (v));
}
// run mono source through the processor (stereo out), plus `tailSec` of silence
static juce::AudioBuffer<float> run (VocalPunchProcessor& p, const juce::AudioBuffer<float>& src, double sr, double tailSec = 0.0, int block = 512)
{
    const int n = src.getNumSamples() + (int) (tailSec * sr);
    juce::AudioBuffer<float> out (2, n); out.clear();
    p.setPlayConfigDetails (2, 2, sr, block);
    p.prepareToPlay (sr, block);
    juce::MidiBuffer m;
    for (int s = 0; s < n; s += block)
    {
        const int k = juce::jmin (block, n - s);
        juce::AudioBuffer<float> b (2, k); b.clear();
        if (s < src.getNumSamples())
        {
            const int c = juce::jmin (k, src.getNumSamples() - s);
            b.copyFrom (0, 0, src, 0, s, c); b.copyFrom (1, 0, src, 0, s, c);
        }
        p.processBlock (b, m);
        out.copyFrom (0, s, b, 0, 0, k); out.copyFrom (1, s, b, 1, 0, k);
    }
    return out;
}
static double rmsDb (const juce::AudioBuffer<float>& b, int ch, int s0, int n)
{
    double e = 0; n = juce::jmin (n, b.getNumSamples() - s0);
    for (int i = 0; i < n; ++i) { const double v = b.getSample (ch, s0 + i); e += v * v; }
    return 10.0 * std::log10 (e / juce::jmax (1, n) + 1e-20);
}
static bool finite (const juce::AudioBuffer<float>& b)
{
    for (int c = 0; c < b.getNumChannels(); ++c) for (int i = 0; i < b.getNumSamples(); ++i) if (! std::isfinite (b.getSample (c, i))) return false;
    return true;
}

int main (int argc, char** argv)
{
    juce::ScopedJuceInitialiser_GUI init;
    const auto sung = load (juce::File (argv[1])), rap = load (juce::File (argv[2]));
    const juce::File outDir (argv[3]); outDir.createDirectory();
    const double SR = 48000.0;

    // ---- every preset at 44.1, 48 and 96 kHz
    {
        VocalPunchProcessor p;
        int bad = 0, loud = 0, quiet = 0, tails = 0;
        juce::String worst;
        for (int i = 0; i < p.getNumPrograms(); ++i)
        {
            p.setCurrentProgram (i);
            for (double sr : { 44100.0, 48000.0, 96000.0 })
            {
                auto o = run (p, sung, sr, 8.0, sr == 96000.0 ? 1024 : 512);
                const float pk = o.getMagnitude (0, o.getNumSamples());
                const double lvl = rmsDb (o, 0, 0, sung.getNumSamples());
                if (! finite (o)) { ++bad; worst << p.getProgramName (i) << " NaN; "; }
                if (pk > 1.0f) { ++loud; worst << p.getProgramName (i) << " peak " << pk << "; "; }
                if (lvl < -40.0) { ++quiet; worst << p.getProgramName (i) << " rms " << lvl << "; "; }
                const double tail = rmsDb (o, 0, o.getNumSamples() - (int) (0.5 * sr), (int) (0.5 * sr));
                if (tail > -60.0) { ++tails; worst << p.getProgramName (i) << " tail " << tail << "; "; }
            }
        }
        check (bad == 0, "all " + juce::String (p.getNumPrograms()) + " presets x 3 sample rates: no NaN/inf");
        check (loud == 0, "no preset exceeds 0 dBFS");
        check (quiet == 0, "no preset drops the vocal below -40 dB RMS");
        check (tails == 0, "every preset's echoes/reverb die away within 8 s  " + worst.substring (0, 200));
    }

    // ---- preset level spread (presets should be roughly level-matched)
    {
        VocalPunchProcessor p; double lo = 0, hi = -200; juce::String loN, hiN;
        for (int i = 0; i < p.getNumPrograms(); ++i)
        {
            p.setCurrentProgram (i);
            auto o = run (p, sung, SR);
            const double l = 0.5 * (rmsDb (o, 0, 0, o.getNumSamples()) + rmsDb (o, 1, 0, o.getNumSamples()));
            printf ("   %-18s %6.1f dB\n", p.getProgramName (i).toRawUTF8(), l);
            if (l < lo || loN.isEmpty()) { lo = l; loN = p.getProgramName (i); }
            if (l > hi) { hi = l; hiN = p.getProgramName (i); }
        }
        check (hi - lo < 14.0, "preset loudness spread " + juce::String (hi - lo, 1) + " dB (" + loN + " .. " + hiN + ") < 14 dB");
    }

    // ---- bypass and mix = 0 pass the input untouched
    {
        VocalPunchProcessor p; p.setCurrentProgram (0);
        setP (p, "bypass", 1.0f);
        auto o = run (p, sung, SR);
        float d = 0; for (int i = 0; i < sung.getNumSamples(); ++i) d = juce::jmax (d, std::abs (o.getSample (0, i) - sung.getSample (0, i)));
        check (d < 1e-6f, "BYPASS output equals input");
        setP (p, "bypass", 0.0f); setP (p, "mix", 0.0f);
        o = run (p, sung, SR); d = 0;
        for (int i = 0; i < sung.getNumSamples(); ++i) d = juce::jmax (d, std::abs (o.getSample (1, i) - sung.getSample (0, i)));
        check (d < 1e-5f, "MIX 0% output equals input (no latency)");
    }

    // ---- low cut removes rumble
    {
        juce::AudioBuffer<float> sine (1, (int) SR * 2);
        for (int i = 0; i < sine.getNumSamples(); ++i) sine.setSample (0, i, 0.3f * std::sin (2.0f * juce::MathConstants<float>::pi * 40.0f * (float) i / (float) SR));
        VocalPunchProcessor p; p.setCurrentProgram (p.getNumPrograms() - 1);   // Bypass Flat
        setP (p, "hpf", 120.0f);
        auto o = run (p, sine, SR);
        const double att = rmsDb (sine, 0, (int) SR, (int) SR) - rmsDb (o, 0, (int) SR, (int) SR);
        check (att > 20.0, "LOW CUT 120 Hz takes a 40 Hz hum down " + juce::String (att, 1) + " dB (> 20)");
    }

    // ---- de-esser: sibilant bursts are reduced, vowels are not
    {
        juce::AudioBuffer<float> x (1, (int) SR * 4); x.clear();
        juce::Random rng (7);
        tdsp::Biquad hp; hp.set (tdsp::Biquad::HighPass, SR, 5500.0, 0.707);
        for (int i = 0; i < x.getNumSamples(); ++i)
        {
            const double t = i / SR; const int seg = (int) (t / 0.5);
            float v = 0.2f * std::sin ((float) (2 * tdsp::kPi * 220 * t)) + 0.08f * std::sin ((float) (2 * tdsp::kPi * 660 * t));
            if (seg % 2 == 1) v = hp.process (rng.nextFloat() * 2 - 1) * 0.35f + 0.05f * std::sin ((float) (2 * tdsp::kPi * 220 * t));   // "sss"
            x.setSample (0, i, v);
        }
        auto measure = [&] (float amt, double& sib, double& vow)
        {
            VocalPunchProcessor p; p.setCurrentProgram (p.getNumPrograms() - 1);
            setP (p, "deess", amt);
            auto o = run (p, x, SR);
            sib = rmsDb (o, 0, (int) (0.6 * SR), (int) (0.3 * SR)); vow = rmsDb (o, 0, (int) (1.1 * SR), (int) (0.3 * SR));
        };
        double s0, v0, s1, v1; measure (0.0f, s0, v0); measure (1.0f, s1, v1);
        check (s0 - s1 > 5.0, "DE-ESS 100% cuts sibilance " + juce::String (s0 - s1, 1) + " dB (> 5)");
        check (std::abs (v0 - v1) < 0.5, "DE-ESS leaves vowels alone (" + juce::String (v0 - v1, 2) + " dB)");
    }

    // ---- compressor evens out an uneven performance
    {
        auto lineLevels = [&] (const juce::AudioBuffer<float>& b, int ch)
        {
            const double bar = 4 * 60.0 / 128.0; std::vector<double> v;
            for (int k = 0; k < 8; ++k) v.push_back (rmsDb (b, ch, (int) ((k * bar + 0.1) * SR), (int) ((bar - 0.3) * SR)));
            return v;
        };
        auto spread = [] (const std::vector<double>& v) { return *std::max_element (v.begin(), v.end()) - *std::min_element (v.begin(), v.end()); };
        VocalPunchProcessor p;
        for (int i = 0; i < p.getNumPrograms(); ++i) if (p.getProgramName (i) == "Rap Upfront") p.setCurrentProgram (i);
        setP (p, "delay", 0.0f); setP (p, "reverb", 0.0f);
        auto o = run (p, rap, SR);
        const double before = spread (lineLevels (rap, 0)), after = spread (lineLevels (o, 0));
        check (after < before * 0.6, "Rap Upfront evens the level: line spread " + juce::String (before, 1) + " dB -> " + juce::String (after, 1) + " dB");
    }

    // ---- doubler widens, centre stays centred without it
    {
        auto corr = [] (const juce::AudioBuffer<float>& b)
        {
            double lr = 0, ll = 0, rr = 0;
            for (int i = 0; i < b.getNumSamples(); ++i) { const double l = b.getSample (0, i), r = b.getSample (1, i); lr += l * r; ll += l * l; rr += r * r; }
            return lr / std::sqrt (ll * rr + 1e-20);
        };
        VocalPunchProcessor p; p.setCurrentProgram (p.getNumPrograms() - 1);
        auto o0 = run (p, sung, SR);
        setP (p, "doubler", 1.0f); setP (p, "dbl_spread", 0.6f);
        auto o1 = run (p, sung, SR);
        check (corr (o0) > 0.999, "no doubler: mono vocal stays centred (corr " + juce::String (corr (o0), 4) + ")");
        check (corr (o1) < 0.9, "doubler 100%: stereo width (corr " + juce::String (corr (o1), 3) + " < 0.9)");
        setP (p, "width", 0.0f);
        auto o2 = run (p, sung, SR);
        check (corr (o2) > 0.999, "WIDTH macro 0% switches the doubler off");
    }

    // ---- gate removes noise between phrases
    {
        VocalPunchProcessor p; p.setCurrentProgram (p.getNumPrograms() - 1);
        setP (p, "hpf", 100.0f);
        auto o0 = run (p, rap, SR);
        setP (p, "gate", -40.0f);
        auto o1 = run (p, rap, SR);
        // last 0.8 s of the file is noise only (after the last line)
        const int s0 = rap.getNumSamples() - (int) (0.8 * SR);
        const double d = rmsDb (o0, 0, s0, (int) (0.7 * SR)) - rmsDb (o1, 0, s0, (int) (0.7 * SR));
        check (d > 15.0, "GATE -40 dB (with low cut) drops the noise floor " + juce::String (d, 1) + " dB (> 15)");
        const double v = rmsDb (o0, 0, (int) (0.3 * SR), (int) SR) - rmsDb (o1, 0, (int) (0.3 * SR), (int) SR);
        check (std::abs (v) < 1.0, "GATE keeps the words (" + juce::String (v, 2) + " dB)");
    }

    // ---- tempo-synced delay lands on time
    {
        struct Host : juce::AudioPlayHead { juce::Optional<PositionInfo> getPosition() const override { PositionInfo p; p.setBpm (120.0); return p; } } host;
        juce::AudioBuffer<float> imp (1, (int) SR); imp.clear(); imp.setSample (0, 100, 1.0f);
        VocalPunchProcessor p; p.setCurrentProgram (p.getNumPrograms() - 1); p.setPlayHead (&host);
        setP (p, "delay", 1.0f); setP (p, "delay_fb", 0.0f); setP (p, "duck", 0.0f); setP (p, "delay_pp", 0.0f);
        p.apvts.getParameter ("delay_time")->setValueNotifyingHost (p.apvts.getParameter ("delay_time")->convertTo0to1 (6.0f));  // 1/8D
        auto o = run (p, imp, SR);
        int at = 0; float best = 0;
        for (int i = 2000; i < o.getNumSamples(); ++i) if (std::abs (o.getSample (0, i)) > best) { best = std::abs (o.getSample (0, i)); at = i; }
        const double ms = (at - 100) / SR * 1000.0;
        check (std::abs (ms - 375.0) < 2.0, "1/8 dotted at 120 BPM echoes at " + juce::String (ms, 1) + " ms (375)");
        p.setPlayHead (nullptr);
    }

    // ---- presets fully define the chain (no leftovers from the previous preset)
    {
        VocalPunchProcessor p;
        int a = -1, b = -1;
        for (int i = 0; i < p.getNumPrograms(); ++i) { if (p.getProgramName (i) == "Telephone") a = i; if (p.getProgramName (i) == "Pop Shine") b = i; }
        p.setCurrentProgram (a); p.setCurrentProgram (b);
        check (p.apvts.getRawParameterValue ("lofi")->load() < 0.001f, "Telephone -> Pop Shine resets LO-FI to 0");
        check ((int) p.apvts.getRawParameterValue ("sat_type")->load() == 0, "Pop Shine sets drive type Tape");
    }

    // ---- state round trip
    {
        VocalPunchProcessor a, b;
        a.setCurrentProgram (7); setP (a, "punch", 1.6f); setP (a, "space", 0.3f);
        juce::MemoryBlock mb; a.getStateInformation (mb);
        b.setStateInformation (mb.getData(), (int) mb.getSize());
        check (b.getCurrentProgram() == 7 && std::abs (b.apvts.getRawParameterValue ("punch")->load() - 1.6f) < 0.01f
               && std::abs (b.apvts.getRawParameterValue ("space")->load() - 0.3f) < 0.01f, "state save/restore (preset + macros)");
    }

    // ---- layouts
    {
        VocalPunchProcessor p;
        juce::AudioProcessor::BusesLayout ms, ss, sm;
        ms.inputBuses.add (juce::AudioChannelSet::mono()); ms.outputBuses.add (juce::AudioChannelSet::stereo());
        ss.inputBuses.add (juce::AudioChannelSet::stereo()); ss.outputBuses.add (juce::AudioChannelSet::stereo());
        sm.inputBuses.add (juce::AudioChannelSet::stereo()); sm.outputBuses.add (juce::AudioChannelSet::mono());
        check (p.checkBusesLayoutSupported (ms) && p.checkBusesLayoutSupported (ss) && ! p.checkBusesLayoutSupported (sm), "mono->stereo and stereo->stereo supported");
    }

    // ---- editor builds, renders, shows branding; render processed demo files
    {
        VocalPunchProcessor p; p.setCurrentProgram (0);
        auto o = run (p, sung, SR);
        std::unique_ptr<juce::AudioProcessorEditor> ed (p.createEditor());
        static_cast<VocalPunchEditor*> (ed.get())->timerCallback();
        auto img = ed->createComponentSnapshot (ed->getLocalBounds(), true, 1.0f);
        juce::File f = outDir.getChildFile ("vocalpunch_ui.png"); f.deleteFile();
        { juce::FileOutputStream fos (f); juce::PNGImageFormat().writeImageToStream (img, fos); }
        check (img.getWidth() == 1080 && img.getHeight() == 700, "editor 1080x700 renders");
        juce::WavAudioFormat wav;
        auto wf = outDir.getChildFile ("pop_shine.wav"); wf.deleteFile();
        std::unique_ptr<juce::AudioFormatWriter> w (wav.createWriterFor (new juce::FileOutputStream (wf), SR, 2, 24, {}, 0));
        w->writeFromAudioSampleBuffer (o, 0, o.getNumSamples());
    }

    printf ("\n%d passed, %d failed\n", passes, fails);
    printf (fails == 0 ? "ALL TESTS PASSED\n" : "SOME TESTS FAILED\n");
    return fails == 0 ? 0 : 1;
}
