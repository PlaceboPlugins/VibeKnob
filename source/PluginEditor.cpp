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
        
        // 1. Convert 0-10 raw value into a 0-100 percentage scale
        float percentage = currentVibeValue * 10.0f;
        
        // 2. Round it to the nearest whole number (no ugly decimals!)
        int roundedPercentage = juce::roundToInt (percentage);
        
        // 3. Update the label with a clean format
        statusLabel.setText ("Vibe Level: " + juce::String (roundedPercentage) + "%", juce::dontSendNotification);
        
        repaint();
    };

    // Configure the Title Label
    titleLabel.setText("The Vibe Knob", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(24.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    // Configure the Status Label
    statusLabel.setText("Vibe Level: 0%", juce::dontSendNotification); // Clean initial text!
    statusLabel.setFont(juce::FontOptions("Futura", 16.0f, juce::Font::bold)); // Custom Font & Size!
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::cyan); // Electric Cyan Text!
    statusLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(statusLabel);

    setSize (400, 300);
}

// 👇 INSERT THIS DESTRUCTOR RIGHT HERE 
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
        // Draw the full neon-color version of your background image
        g.drawImageWithin (cachedImage, 0, 0, getWidth(), getHeight(), 
                           juce::RectanglePlacement::fillDestination);

        // 3. Create the 0% saturation overlay trick!
        // If vibe is less than 100%, blend a grayscale overlay on top
        if (saturation < 1.0f)
        {
            // Create a temporary black & white snapshot copy of your image
            auto grayscaleImage = cachedImage.createCopy();
            grayscaleImage.desaturate();

            // The overlay transparency is inverse to the knob position
            // (e.g., 0% knob = 1.0 opacity grayscale overlay, 100% knob = 0.0 opacity overlay)
            float grayscaleOpacity = 1.0f - saturation;

            g.setOpacity (grayscaleOpacity);
            g.drawImageWithin (grayscaleImage, 0, 0, getWidth(), getHeight(), 
                               juce::RectanglePlacement::fillDestination);
            
            // Reset opacity back to normal for other text/elements
            g.setOpacity (1.0f);
        }
    }
    else
    {
        // Fallback safety color if the image fails to load for any reason
        g.fillAll (juce::Colours::darkgrey);
    }

    // 4. Draw a futuristic neon ring that glows brighter as vibe increases
    auto center = bounds.getCentre();
    float ringRadius = 90.0f;
    g.setColour (juce::Colour (0xFF00F5FF).withAlpha (0.1f + (saturation * 0.5f))); 
    g.drawEllipse (center.x - ringRadius, center.y - ringRadius, ringRadius * 2.0f, ringRadius * 2.0f, 3.0f);

    // 5. Drop our custom italic watermark text at the bottom
    g.setColour (juce::Colours::white.withAlpha (0.2f + (saturation * 0.8f)));
    g.setFont (juce::FontOptions (15.0f, juce::Font::bold | juce::Font::italic));
    g.drawText ("\"Unicorn power levels are optimal.\" 🦄✨", 
                0, getHeight() - 30, getWidth(), 25, 
                juce::Justification::centred);
}

void PamplejuceAudioProcessorEditor::resized()
{
    titleLabel.setBounds(0, 20, getWidth(), 40);
    vibeKnob.setBounds(getWidth() / 2 - 75, getHeight() / 2 - 75, 150, 150);
    statusLabel.setBounds(0, getHeight() - 50, getWidth(), 30);
}
