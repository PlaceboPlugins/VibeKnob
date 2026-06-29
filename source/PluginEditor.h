#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

// This tells the compiler "Hey, this class exists, don't panic!"
class PamplejuceAudioProcessor; 

class PamplejuceAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                        public juce::Slider::Listener
{
public:
    explicit PamplejuceAudioProcessorEditor (PamplejuceAudioProcessor&);
    ~PamplejuceAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged (juce::Slider* slider) override;

private:
    juce::Slider vibeKnob;
    juce::Label titleLabel;
    juce::Label statusLabel;
    float currentVibeValue = 0.0f;

    PamplejuceAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PamplejuceAudioProcessorEditor)
};
