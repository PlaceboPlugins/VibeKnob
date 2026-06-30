#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"

PamplejuceAudioProcessorEditor::PamplejuceAudioProcessorEditor (PamplejuceAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    // Configure our Vibe Knob
    vibeKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    vibeKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(vibeKnob);

    // Modern way to handle knob changes without needing a massive Listener class!
    vibeKnob.onValueChange = [this]() {
        currentVibeValue = static_cast<float>(vibeKnob.getValue());
        
        // 1. Round to nearest integer percentage
        int roundedPercentage = juce::roundToInt (currentVibeValue * 10.0f);

        // 2. Match the exact HTML quote boundaries
        juce::String vibeQuote = "";
        if (roundedPercentage <= 0)       vibeQuote = "\"It's missing something...\"";
        else if (roundedPercentage <= 20) vibeQuote = "\"Yeah, I think I hear it opening up...\"";
        else if (roundedPercentage <= 40) vibeQuote = "\"Wow, the analog warmth is coming in now!\"";
        else if (roundedPercentage <= 60) vibeQuote = "\"The mid-range feels way more transparent!\"";
        else if (roundedPercentage <= 80) vibeQuote = "\"Turn it up! The upper-harmonics are shimmering!\"";
        else if (roundedPercentage <= 99) vibeQuote = "\"YES! This is a hit! Don't touch a thing!\"";
        else                              vibeQuote = "\"MAXIMUM VIBE ACHIEVED 🌈✨ (The master bus is peaking!)\"";

        // Update the display text with both the percentage and the dynamic quote
        statusLabel.setText ("Vibe Level: " + juce::String (roundedPercentage) + "% | " + vibeQuote, juce::dontSendNotification);
        
        // 3. Update status text color dynamically based on percentage!
        float saturation = static_cast<float>(roundedPercentage) / 100.0f;
        // Blend from brilliant electric cyan (0%) to neon hot pink (100%)
        juce::Colour dynamicTextColor = juce::Colours::cyan.interpolatedWith (juce::Colour (0xFFFF007F), saturation);
        statusLabel.setColour (juce::Label::textColourId, dynamicTextColor);

        repaint();
    };

    // Configure the Title Label
    titleLabel.setText("The Vibe Knob", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(24.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    // Configure the Status Label
    statusLabel.setText("Vibe Level: 0% | \"It's missing something...\"", juce::dontSendNotification); 
    statusLabel.setFont(juce::FontOptions("Futura", 14.0f, juce::Font::bold)); 
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::cyan); 
    statusLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(statusLabel);

    setSize (400, 300);
}

PamplejuceAudioProcessorEditor::~PamplejuceAudioProcessorEditor()
{
}

void PamplejuceAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // 1. Calculate our knob saturation factor (0.0 to 1.0)
    float saturation = currentVibeValue / 100.0f;
    saturation = juce::jlimit (0.0f, 1.0f, saturation);

    // 2. Load our background image from the baked Binary Data
    static auto cachedImage = juce::ImageCache::getFromMemory (BinaryData::background_jpg, BinaryData::background_jpgSize);

    if (cachedImage.isValid())
    {
        if (saturation >= 1.0f)
        {
            g.drawImageWithin (cachedImage, 0, 0, getWidth(), getHeight(), 
                               juce::RectanglePlacement::fillDestination);
        }
        else
        {
            auto grayscaleImage = cachedImage.createCopy();
            grayscaleImage.desaturate();

            g.drawImageWithin (grayscaleImage, 0, 0, getWidth(), getHeight(), 
                               juce::RectanglePlacement::fillDestination);

            g.setOpacity (saturation);
            g.drawImageWithin (cachedImage, 0, 0, getWidth(), getHeight(), 
                               juce::RectanglePlacement::fillDestination);
            
            g.setOpacity (1.0f); 
        }
    }
    else
    {
        g.fillAll (juce::Colours::darkgrey);
    }

    // 4. Draw a futuristic neon ring that glows brighter as vibe increases
    auto center = bounds.getCentre();
    float ringRadius = 90.0f;
    g.setColour (juce::Colour (0xFF00F5FF).withAlpha (0.1f + (saturation * 0.5f))); 
    g.drawEllipse (center.x - ringRadius, center.y - ringRadius, ringRadius * 2.0f, ringRadius * 2.0f, 3.0f);
}

void PamplejuceAudioProcessorEditor::resized()
{
    titleLabel.setBounds(0, 20, getWidth(), 40);
    vibeKnob.setBounds(getWidth() / 2 - 75, getHeight() / 2 - 75, 150, 150);
    statusLabel.setBounds(0, getHeight() - 50, getWidth(), 30);
}
