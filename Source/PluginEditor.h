/*
  ==============================================================================
    VocalPunch - PluginEditor.h
    (c)(p) 2026 Tully EDM Vibe
  ==============================================================================
*/
#pragma once
#include "PluginProcessor.h"

// EQ curve of the current chain (low cut + EQ + tone)
class CurveView : public juce::Component, public juce::SettableTooltipClient
{
public:
    std::function<double (double)> response;
    juce::Colour colour = tui::Col::pink;
    float range = 18.0f;
    void paint (juce::Graphics& g) override
    {
        auto r = getLocalBounds().toFloat();
        g.setColour (juce::Colour (0xcc0d1014)); g.fillRoundedRectangle (r, 6.0f);
        g.setFont (juce::FontOptions (9.0f)); 
        for (double f : { 100.0, 1000.0, 10000.0 })
        {
            const float x = xFor (f, r);
            g.setColour (tui::Col::line.withAlpha (0.7f)); g.drawVerticalLine ((int) x, r.getY(), r.getBottom());
            g.setColour (tui::Col::dim); g.drawText (f >= 1000 ? juce::String ((int) (f / 1000)) + "k" : juce::String ((int) f), (int) x + 2, (int) r.getBottom() - 12, 30, 10, juce::Justification::left);
        }
        g.setColour (tui::Col::line); g.drawHorizontalLine ((int) r.getCentreY(), r.getX(), r.getRight());
        if (! response) return;
        juce::Path p, fill;
        for (int i = 0; i <= 120; ++i)
        {
            const double f = 20.0 * std::pow (1000.0, i / 120.0);
            const float db = juce::jlimit (-range, range, (float) response (f));
            const float x = xFor (f, r), y = r.getCentreY() - db / range * r.getHeight() * 0.45f;
            if (i == 0) { p.startNewSubPath (x, y); fill.startNewSubPath (x, r.getCentreY()); }
            else p.lineTo (x, y);
            fill.lineTo (x, y);
        }
        fill.lineTo (r.getRight(), r.getCentreY()); fill.closeSubPath();
        g.setColour (colour.withAlpha (0.18f)); g.fillPath (fill);
        g.setColour (colour); g.strokePath (p, juce::PathStrokeType (2.0f));
        g.setColour (tui::Col::line); g.drawRoundedRectangle (r.reduced (0.5f), 6.0f, 1.0f);
    }
private:
    static float xFor (double f, juce::Rectangle<float> r) { return r.getX() + r.getWidth() * (float) (std::log (f / 20.0) / std::log (1000.0)); }
};

class VocalPunchEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit VocalPunchEditor (VocalPunchProcessor&);
    ~VocalPunchEditor() override;
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

    VocalPunchProcessor& proc;
    tui::LnF lnf;
    juce::Image logo, productLogo;
    tui::PresetBar presetBar;
    tui::BrandLinks links;
    CurveView curve;
    tui::Scope scope;
    tui::LevelMeter mIn { "IN", tui::Col::cyan }, mDs { "DE-ESS", tui::Col::amber, -16.0f, 0.0f, true },
                    mGr { "COMP", tui::Col::pink, -24.0f, 0.0f, true }, mOut { "OUT", tui::Col::cyan };
    std::vector<std::unique_ptr<tui::Knob>> knobs;
    std::vector<tui::Knob*> macro, outK, clean, eq, comp, character, doubler, delay, reverb, duck;
    tui::Choice satType, delayTime;
    tui::Toggle pingPong, bypass;
    juce::TooltipWindow tooltips { this, 500 };
private:
    juce::Rectangle<int> rMacro, rCurve, rOut, rClean, rEq, rComp, rChar, rDbl, rDelay, rRev, rDuck;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalPunchEditor)
};
