#include "PluginProcessor.h"
#include "PluginEditor.h"

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
        statusLabel.setText("Vibe Level: " + juce::String(currentVibeValue), juce::dontSendNotification);
    };

    // Configure the Title Label
    titleLabel.setText("The Vibe Knob", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(24.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    // Configure the Status Label
    statusLabel.setText("Vibe Level: 0.0", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(statusLabel);

    setSize (400, 300);
}

void PamplejuceAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);
}

void PamplejuceAudioProcessorEditor::resized()
{
    titleLabel.setBounds(0, 20, getWidth(), 40);
    vibeKnob.setBounds(getWidth() / 2 - 75, getHeight() / 2 - 75, 150, 150);
    statusLabel.setBounds(0, getHeight() - 50, getWidth(), 30);
}
