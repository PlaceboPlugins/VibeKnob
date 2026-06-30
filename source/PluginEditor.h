#pragma once

#include "PluginProcessor.h"

class PamplejuceAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                        private juce::Timer
{
public:
    explicit PamplejuceAudioProcessorEditor (PamplejuceAudioProcessor&);
    ~PamplejuceAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct Particle
    {
        float x = 0.0f;
        float y = 0.0f;
        float speedX = 0.0f;
        float speedY = 0.0f;
        float scale = 1.0f;
        float rotation = 0.0f;
        float rotationSpeed = 0.0f;
        bool isUnicorn = true; // true = image, false = procedural star!
    };

    void timerCallback() override;

    PamplejuceAudioProcessor& processorRef;

    juce::Slider vibeKnob;
    juce::Label titleLabel;
    juce::Label statusLabel;

    float currentVibeValue = 0.0f;

    std::vector<Particle> particles;
    juce::Random random;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PamplejuceAudioProcessorEditor)
};
