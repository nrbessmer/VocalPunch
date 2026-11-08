/*
  ==============================================================================
    VocalPunch - PluginEditor.cpp
    (c)(p) 2026 Tully EDM Vibe
  ==============================================================================
*/
#include "PluginEditor.h"
#include "BinaryData.h"

namespace Col = tui::Col;

VocalPunchEditor::VocalPunchEditor (VocalPunchProcessor& p)
    : AudioProcessorEditor (&p), proc (p), presetBar (vpunch::presets()), scope (p.scope, Col::pink),
      satType (p.apvts, "sat_type", "Drive character: Tape = warm and smooth, Tube = rich even harmonics, Fuzz = hard and aggressive, Crush = bit-crushed digital grit."),
      delayTime (p.apvts, "delay_time", "Delay time as a note value at your song tempo (D = dotted, T = triplet)."),
      pingPong (p.apvts, "delay_pp", "PING-PONG", Col::cyan, "Echoes bounce between left and right."),
      bypass (p.apvts, "bypass", "BYPASS", Col::amber, "Compare with the untouched vocal.")
{
    lnf.setAccent (Col::pink);
    setLookAndFeel (&lnf);
    logo = juce::ImageCache::getFromMemory (BinaryData::TullyLogo_png, BinaryData::TullyLogo_pngSize);
    productLogo = juce::ImageCache::getFromMemory (BinaryData::VocalPunchLogo_png, BinaryData::VocalPunchLogo_pngSize);

    presetBar.getCurrent = [this] { return proc.getCurrentPreset(); };
    presetBar.load = [this] (int i) { proc.loadPreset (i); };
    presetBar.sync();

    auto& s = p.apvts;
    auto k = [this, &s] (std::vector<tui::Knob*>& grp, const char* id, const char* cap, juce::Colour c, const char* tip)
    { knobs.push_back (std::make_unique<tui::Knob> (s, id, cap, c, tip)); addAndMakeVisible (*knobs.back()); grp.push_back (knobs.back().get()); };

    k (macro, "punch", "PUNCH", Col::pink, "The main knob: more compression, drive and EQ shine. 100% = the preset as designed.");
    k (macro, "tone", "TONE", Col::amber, "Darker (left) or brighter (right) without touching the EQ.");
    k (macro, "width", "WIDTH", Col::violet, "Scales the doubler: from a centred mono vocal to a wide double-tracked sound.");
    k (macro, "space", "SPACE", Col::cyan, "Scales delay and reverb together: dry and upfront to big and washed.");
    k (outK, "mix", "MIX", Col::text, "Blend the processed vocal with the original (parallel processing).");
    k (outK, "out_gain", "OUTPUT", Col::text, "Output level.");

    k (clean, "in_gain", "INPUT", Col::cyan, "Input level into the chain. Aim for peaks around -10 dB on the IN meter.");
    k (clean, "hpf", "LOW CUT", Col::cyan, "Removes rumble, handling noise and plosive boom below this frequency.");
    k (clean, "gate", "GATE", Col::cyan, "Silences the track between phrases (breaths, room noise). Off by default.");
    k (clean, "deess", "DE-ESS", Col::amber, "Tames harsh S, T and SH sounds. Watch the DE-ESS meter.");
    k (clean, "deess_freq", "S FREQ", Col::amber, "Where the sibilance lives: lower for deep voices, higher for bright voices.");
    k (eq, "eq_low", "LOW", Col::amber, "Body and warmth (shelf at 140 Hz).");
    k (eq, "eq_mud", "MUD", Col::amber, "Boxy, muddy area around 350 Hz. Usually cut.");
    k (eq, "eq_pres", "PRESENCE", Col::amber, "Clarity and words (3.2 kHz). Boost to cut through a mix.");
    k (eq, "eq_air", "AIR", Col::amber, "Breathy sparkle on top (shelf at 11 kHz).");
    k (comp, "comp", "AMOUNT", Col::pink, "How hard the compressor works. Level is made up automatically.");
    k (comp, "comp_ratio", "RATIO", Col::pink, "Gentle (2:1) to brick-wall (20:1).");
    k (comp, "comp_att", "ATTACK", Col::pink, "Fast = smooth and controlled, slow = more bite on word starts.");
    k (comp, "comp_rel", "RELEASE", Col::pink, "How quickly the compressor lets go.");
    k (character, "sat", "DRIVE", Col::violet, "Harmonic saturation: from subtle warmth to full distortion.");
    k (character, "lofi", "LO-FI", Col::violet, "Narrows the band like a phone or old radio.");
    k (doubler, "doubler", "AMOUNT", Col::violet, "Adds two detuned, delayed copies left and right, like a double-tracked take.");
    k (doubler, "dbl_spread", "DETUNE", Col::violet, "How far the doubles drift in pitch and timing.");
    k (delay, "delay", "MIX", Col::cyan, "Echo level. Follows your song tempo.");
    k (delay, "delay_fb", "FEEDBACK", Col::cyan, "Number of repeats.");
    k (reverb, "reverb", "MIX", Col::cyan, "Reverb level.");
    k (reverb, "reverb_size", "SIZE", Col::cyan, "Small room to huge hall.");
    k (reverb, "reverb_pre", "PRE-DELAY", Col::cyan, "Gap before the reverb starts: keeps words clear.");
    k (duck, "duck", "DUCK", Col::green, "Turns delay and reverb down while you sing and lets them bloom in the gaps.");

    for (auto* m : macro) m->slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 15);

    curve.response = [this] { return [this] (double f) { return proc.chain.eqResponseDb (f); }; }();
    curve.setTooltip ("Tone curve of the chain: low cut, EQ and TONE.");
    mIn.setTooltip ("Input level (after INPUT gain)."); mOut.setTooltip ("Output level.");
    mDs.setTooltip ("De-esser gain reduction on S sounds."); mGr.setTooltip ("Compressor gain reduction.");

    for (juce::Component* c : std::initializer_list<juce::Component*> { &presetBar, &links, &curve, &scope, &mIn, &mDs, &mGr, &mOut, &satType, &delayTime, &pingPong, &bypass })
        addAndMakeVisible (c);

    setSize (1080, 700);
    startTimerHz (30);
}

VocalPunchEditor::~VocalPunchEditor() { stopTimer(); setLookAndFeel (nullptr); }

void VocalPunchEditor::timerCallback()
{
    presetBar.sync();
    mIn.setValue (juce::jmax (proc.chain.meterIn.peakL.load(), proc.chain.meterIn.peakR.load()));
    mOut.setValue (juce::jmax (proc.chain.meterOut.peakL.load(), proc.chain.meterOut.peakR.load()));
    mDs.setValue (proc.chain.deessGrDb.load());
    mGr.setValue (proc.chain.compGrDb.load());
    curve.repaint();
    scope.refresh();
}

void VocalPunchEditor::paint (juce::Graphics& g)
{
    neon::paintBackdrop (g, getLocalBounds(), Col::pink, backdrop);
    tui::paintWatermark (g, productLogo, getLocalBounds(), 0.20f);
    tui::paintBrandHeader (g, getWidth(), 76, productLogo, logo, "VOCALPUNCH", "ONE-KNOB VOCAL CHAINS", Col::pink, presetBar.getX() - 10);
    tui::paintGlassSection (g, rMacro, "STYLE MACROS", Col::pink);
    tui::paintGlassSection (g, rCurve, "VOICE CURVE", Col::amber);
    tui::paintGlassSection (g, rOut, "OUTPUT", Col::text);
    tui::paintGlassSection (g, rClean, "CLEAN UP", Col::cyan);
    tui::paintGlassSection (g, rEq, "EQ", Col::amber);
    tui::paintGlassSection (g, rComp, "COMPRESSOR", Col::pink);
    tui::paintGlassSection (g, rChar, "CHARACTER", Col::violet);
    tui::paintGlassSection (g, rDbl, "DOUBLER", Col::violet);
    tui::paintGlassSection (g, rDelay, "DELAY", Col::cyan);
    tui::paintGlassSection (g, rRev, "REVERB", Col::cyan);
    tui::paintGlassSection (g, rDuck, "DUCK", Col::green);
}

void VocalPunchEditor::resized()
{
    const int W = getWidth();
    presetBar.setBounds (330, 14, 400, 50);
    links.setBounds (W - 210, 26, 200, 36);

    auto b = getLocalBounds().reduced (10); b.removeFromTop (76);
    const int gap = 8;
    auto rowA = b.removeFromTop (214); b.removeFromTop (gap);
    auto rowB = b.removeFromTop ((b.getHeight() - gap) / 2); b.removeFromTop (gap);
    auto rowC = b;

    rMacro = rowA.removeFromLeft (470); rowA.removeFromLeft (gap);
    rOut = rowA.removeFromRight (300); rowA.removeFromRight (gap);
    rCurve = rowA;

    auto lay = [] (juce::Rectangle<int> sec, const std::vector<tui::Knob*>& ks, int maxW = 84)
    {
        auto a = sec.reduced (6, 4); a.removeFromTop (26);
        const int w = a.getWidth() / juce::jmax (1, (int) ks.size());
        for (auto* k : ks) k->setBounds (a.removeFromLeft (w).withSizeKeepingCentre (juce::jmin (w, maxW), a.getHeight()));
    };
    {   // big macro knobs
        auto a = rMacro.reduced (8, 6); a.removeFromTop (28);
        const int w = a.getWidth() / 4;
        for (auto* k : macro) k->setBounds (a.removeFromLeft (w).withSizeKeepingCentre (juce::jmin (w, 116), juce::jmin (a.getHeight(), 150)));
    }
    {
        auto a = rCurve.reduced (8, 6); a.removeFromTop (24);
        scope.setBounds (a.removeFromBottom (54)); a.removeFromBottom (6);
        curve.setBounds (a);
    }
    {
        auto a = rOut.reduced (8, 6); a.removeFromTop (24);
        auto meters = a.removeFromLeft (164);
        const int mw = meters.getWidth() / 4;
        for (auto* m : { &mIn, &mDs, &mGr, &mOut }) m->setBounds (meters.removeFromLeft (mw));
        bypass.setBounds (a.removeFromBottom (24).reduced (4, 0));
        a.removeFromBottom (4);
        auto top = a.removeFromTop (a.getHeight() / 2);
        outK[0]->setBounds (top.withSizeKeepingCentre (juce::jmin (top.getWidth(), 76), juce::jmin (top.getHeight(), 74)));
        outK[1]->setBounds (a.withSizeKeepingCentre (juce::jmin (a.getWidth(), 76), juce::jmin (a.getHeight(), 74)));
    }

    const int unitB = (rowB.getWidth() - 2 * gap) / 13;
    rClean = rowB.removeFromLeft (unitB * 5); rowB.removeFromLeft (gap);
    rEq = rowB.removeFromLeft (unitB * 4); rowB.removeFromLeft (gap);
    rComp = rowB;
    lay (rClean, clean); lay (rEq, eq); lay (rComp, comp);

    const int unitC = (rowC.getWidth() - 4 * gap) / 11;
    rChar = rowC.removeFromLeft (unitC * 2 + unitC / 2); rowC.removeFromLeft (gap);
    rDbl = rowC.removeFromLeft (unitC * 2); rowC.removeFromLeft (gap);
    rDelay = rowC.removeFromLeft (unitC * 3); rowC.removeFromLeft (gap);
    rRev = rowC.removeFromLeft (unitC * 3 - unitC / 2); rowC.removeFromLeft (gap);
    rDuck = rowC;
    lay (rChar, character); lay (rDbl, doubler); lay (rDelay, delay); lay (rRev, reverb); lay (rDuck, duck);
    satType.setBounds (rChar.getRight() - 8 - 76, rChar.getY() + 4, 76, 20);
    delayTime.setBounds (rDelay.getRight() - 8 - 64, rDelay.getY() + 4, 64, 20);
    pingPong.setBounds (rDelay.getRight() - 8 - 64 - 6 - 84, rDelay.getY() + 4, 84, 20);
}
