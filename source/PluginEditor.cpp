#include "PluginProcessor.h"
#include "PluginEditor.h"

PamplejuceAudioProcessorEditor::PamplejuceAudioProcessorEditor (PamplejuceAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    // Setup the Giant Vibe Knob
    vibeKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    vibeKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    vibeKnob.setRange(0.0, 100.0, 1.0);
    vibeKnob.setValue(0.0);
    vibeKnob.addListener(this);
    addAndMakeVisible(vibeKnob);

    // Setup Main Title Text
    titleLabel.setText("THE VIBE KNOB", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(22.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    // Setup the Producer Psychology Subtext
    statusLabel.setText("\"It's missing something...\"", juce::dontSendNotification);
    statusLabel.setFont(juce::FontOptions(13.0f, juce::Font::italic));
    statusLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(statusLabel);

    // Initial Plugin Window Size
    setSize (320, 420);
}

PamplejuceAudioProcessorEditor::~PamplejuceAudioProcessorEditor() {}

void PamplejuceAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &vibeKnob)
    {
        currentVibeValue = (float)vibeKnob.getValue() / 100.0f;
        
        // Update client quotes based on knob value
        if (vibeKnob.getValue() <= 1.0) statusLabel.setText("\"It's missing something...\"", juce::dontSendNotification);
        else if (vibeKnob.getValue() <= 25.0) statusLabel.setText("\"Yeah, I think it's opening up...\"", juce::dontSendNotification);
        else if (vibeKnob.getValue() <= 50.0) statusLabel.setText("\"Wow, the analog warmth is coming in!\"", juce::dontSendNotification);
        else if (vibeKnob.getValue() <= 75.0) statusLabel.setText("\"The mid-range feels way more transparent!\"", juce::dontSendNotification);
        else if (vibeKnob.getValue() < 100.0) statusLabel.setText("\"YES! This is a hit! Don't touch a thing!\"", juce::dontSendNotification);
        else statusLabel.setText("\"MAXIMUM VIBE ACHIEVED 🌈✨\"", juce::dontSendNotification);

        repaint(); // Tells the plugin window to redraw its colors
    }
}

void PamplejuceAudioProcessorEditor::paint (juce::Graphics& g)
{
    // C++ Math for changing colors dynamically!
    // Shifts background color from studio gray to saturated psychedelic pink/purple
    float hue = 0.55f + (currentVibeValue * 0.35f); 
    float saturation = currentVibeValue;          
    float brightness = 0.15f + (currentVibeValue * 0.55f); 

    juce::Colour backgroundColor = (currentVibeValue == 0.0f) 
        ? juce::Colour::fromRGB(44, 44, 44) 
        : juce::Colour::fromHSV(hue, saturation, brightness, 1.0f);

    g.fillAll(backgroundColor);
    
    // Change text colors so they look crisp against bright backgrounds
    juce::Colour textColour = (currentVibeValue > 0.5f) ? juce::Colours::black : juce::Colours::white;
    titleLabel.setColour(juce::Label::textColourId, textColour);
    statusLabel.setColour(juce::Label::textColourId, textColour.withAlpha(0.7f));
}

void PamplejuceAudioProcessorEditor::resized()
{
    // Position everything beautifully inside our fixed bounds
    titleLabel.setBounds(0, 30, getWidth(), 30);
    vibeKnob.setBounds(getWidth() / 2 - 90, getHeight() / 2 - 90, 180, 180);
    statusLabel.setBounds(10, getHeight() - 60, getWidth() - 20, 40);
}
