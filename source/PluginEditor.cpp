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

    // Modern way to handle knob changes
    vibeKnob.onValueChange = [this]() {
        currentVibeValue = static_cast<float>(vibeKnob.getValue());
        
        int roundedPercentage = juce::roundToInt (currentVibeValue * 10.0f);

        juce::String vibeQuote = "";
        if (roundedPercentage <= 0)       vibeQuote = "\"It's missing something...\"";
        else if (roundedPercentage <= 20) vibeQuote = "\"Yeah, I think I hear it opening up...\"";
        else if (roundedPercentage <= 40) vibeQuote = "\"Wow, the analog warmth is coming in now!\"";
        else if (roundedPercentage <= 60) vibeQuote = "\"The mid-range feels way more transparent!\"";
        else if (roundedPercentage <= 80) vibeQuote = "\"Turn it up! The upper-harmonics are shimmering!\"";
        else if (roundedPercentage <= 99) vibeQuote = "\"YES! This is a hit! Don't touch a thing!\"";
        else                              vibeQuote = "\"MAXIMUM VIBE ACHIEVED!! (The master bus is peaking!)\"";

        statusLabel.setText ("Vibe Level: " + juce::String (roundedPercentage) + "% | " + vibeQuote, juce::dontSendNotification);
        
        float saturation = static_cast<float>(roundedPercentage) / 100.0f;
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

    // 🚀 START THE TIMER! (33 milliseconds = ~30 Frames Per Second)
    startTimer (33);
}

PamplejuceAudioProcessorEditor::~PamplejuceAudioProcessorEditor()
{
    stopTimer();
}

// ⏱️ THE PHYSICS ANIMATION LOOP (Triggers 30 times a second)
void PamplejuceAudioProcessorEditor::timerCallback()
{
    float vibeFactor = currentVibeValue / 100.0f; // Scale 0.0 to 1.0

    // 1. Randomly spawn particles based on how high the vibe knob is turned up!
    if (vibeFactor > 0.1f && random.nextFloat() < (vibeFactor * 0.4f))
    {
        Particle p;
        p.x = random.nextFloat() * static_cast<float>(getWidth());
        p.y = static_cast<float>(getHeight()) + 20.0f; // Start just below bottom edge
        p.speedX = random.nextFloatRange (-1.5f, 1.5f);
        p.speedY = random.nextFloatRange (-4.0f, -1.0f) * (1.0f + vibeFactor); // Fly faster at high vibe!
        p.scale = random.nextFloatRange (0.4f, 1.0f);
        p.rotation = random.nextFloat() * juce::MathConstants<float>::twoPi;
        p.rotationSpeed = random.nextFloatRange (-0.1f, 0.1f) * vibeFactor;
        p.isUnicorn = (random.nextFloat() > 0.4f); // 60% unicorns, 40% custom math stars!

        particles.push_back (p);
    }

    // 2. Physics Update: Move existing particles
    for (auto& p : particles)
    {
        p.x += p.speedX;
        p.y += p.speedY;
        p.rotation += p.rotationSpeed;
    }

    // 3. Garbage Collection: Safely delete particles that left the top screen boundary
    particles.erase (
        std::remove_if (particles.begin(), particles.end(), 
            [](const Particle& p) { return p.y < -50.0f; }), 
        particles.end()
    );

    // 4. Force window to draw the new frames!
    if (vibeFactor > 0.0f || !particles.empty())
        repaint();
}

void PamplejuceAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    float saturation = currentVibeValue / 100.0f;
    saturation = juce::jlimit (0.0f, 1.0f, saturation);

    // Draw background layers
    static auto cachedImage = juce::ImageCache::getFromMemory (BinaryData::background_jpg, BinaryData::background_jpgSize);

    if (cachedImage.isValid())
    {
        if (saturation >= 1.0f)
        {
            g.drawImageWithin (cachedImage, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::fillDestination);
        }
        else
        {
            auto grayscaleImage = cachedImage.createCopy();
            grayscaleImage.desaturate();
            g.drawImageWithin (grayscaleImage, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::fillDestination);

            g.setOpacity (saturation);
            g.drawImageWithin (cachedImage, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::fillDestination);
            g.setOpacity (1.0f); 
        }
    }
    else
    {
        g.fillAll (juce::Colours::darkgrey);
    }

    // 🎨 DRAW FLOATING PARTICLES RIGHT OVER BACKGROUND
    static auto unicornImg = juce::ImageCache::getFromMemory (BinaryData::unicorn_particle_png, BinaryData::unicorn_particle_pngSize);

    for (const auto& p : particles)
    {
        g.saveState();
        
        // Center the coordinate context on the particle position for smooth spinning/scaling
        g.addTransform (juce::AffineTransform::rotation (p.rotation, p.x, p.y));

        if (p.isUnicorn && unicornImg.isValid())
        {
            float imgW = static_cast<float>(unicornImg.getWidth()) * p.scale;
            float imgH = static_cast<float>(unicornImg.getHeight()) * p.scale;
            g.drawImageWithin (unicornImg, 
                               juce::roundToInt(p.x - imgW/2.0f), juce::roundToInt(p.y - imgH/2.0f), 
                               juce::roundToInt(imgW), juce::roundToInt(imgH),
                               juce::RectanglePlacement::fillDestination);
        }
        else
        {
            // PRO-TRICK: Procedural Neon Vector Sparkles! (No images needed!)
            float starSize = 15.0f * p.scale;
            g.setColour (juce::Colours::cyan.interpolatedWith (juce::Colour (0xFFFF007F), random.nextFloat()));
            
            juce::Path star;
            star.startNewSubPath (p.x, p.y - starSize);
            star.quadraticToTo (p.x, p.y, p.x + starSize, p.y);
            star.quadraticToTo (p.x, p.y, p.x, p.y + starSize);
            star.quadraticToTo (p.x, p.y, p.x - starSize, p.y);
            star.quadraticToTo (p.x, p.y, p.x, p.y - starSize);
            star.closeSubPath();
            
            g.fillPath (star);
        }
        
        g.restoreState();
    }

    // Draw our signature futuristic neon vibe ring
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
