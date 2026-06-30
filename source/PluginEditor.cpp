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
        
        // CORRECTION: Since max knob value is 10.0f, multiply by 10 to get 0-100%
        int roundedPercentage = juce::roundToInt (currentVibeValue * 10.0f);

        juce::String vibeQuote = "";
        if (roundedPercentage <= 0)       vibeQuote = "\"It's missing something...\"";
        else if (roundedPercentage <= 20) vibeQuote = "\"Yeah, I think I hear it opening up...\"";
        else if (roundedPercentage <= 40) vibeQuote = "\"Wow, the analog warmth is coming in now!\"";
        else if (roundedPercentage <= 60) vibeQuote = "\"The mid-range feels way more transparent!\"";
        else if (roundedPercentage <= 80) vibeQuote = "\"Turn it up! The upper-harmonics are shimmering!\"";
        else if (roundedPercentage <= 99) vibeQuote = "\"YES! This is a hit! Don't touch a thing!\"";
        else                              vibeQuote = "\"MAXIMUM VIBE ACHIEVED!! +++ (The master bus is peaking!)\"";

        // Update our stacked labels independently
        vibeDisplayLabel.setText ("Vibe Level: " + juce::String (roundedPercentage) + "%", juce::dontSendNotification);
        quoteLabel.setText (vibeQuote, juce::dontSendNotification);
        
        // Blend from electric cyan to synthwave pink based on 0.0 to 10.0 scale
        float saturation = currentVibeValue / 10.0f; 
        saturation = juce::jlimit (0.0f, 1.0f, saturation);
        juce::Colour dynamicTextColor = juce::Colours::cyan.interpolatedWith (juce::Colour (0xFFFF007F), saturation);
        
        vibeDisplayLabel.setColour (juce::Label::textColourId, dynamicTextColor);
        quoteLabel.setColour (juce::Label::textColourId, dynamicTextColor.withAlpha(0.8f)); // slightly softer quote look

        repaint();
    };

    // Configure the Title Label
    titleLabel.setText("The Vibe Knob", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(24.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    // Configure the Vibe Percentage Label (Top Row)
    vibeDisplayLabel.setText("Vibe Level: 0%", juce::dontSendNotification); 
    vibeDisplayLabel.setFont(juce::FontOptions("Futura", 16.0f, juce::Font::bold)); 
    vibeDisplayLabel.setColour(juce::Label::textColourId, juce::Colours::cyan); 
    vibeDisplayLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(vibeDisplayLabel);

    // Configure the Dynamic Quote Label (Bottom Row - Smaller Font)
    quoteLabel.setText("\"It's missing something...\"", juce::dontSendNotification); 
    quoteLabel.setFont(juce::FontOptions("Futura", 12.0f, juce::Font::italic)); // Smaller italic style!
    quoteLabel.setColour(juce::Label::textColourId, juce::Colours::cyan.withAlpha(0.8f)); 
    quoteLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(quoteLabel);

    setSize (400, 300);

    // START THE TIMER! (~30 FPS)
    startTimer (33);
}

PamplejuceAudioProcessorEditor::~PamplejuceAudioProcessorEditor()
{
    stopTimer();
}

void PamplejuceAudioProcessorEditor::timerCallback()
{
    // CORRECTION: Scaling based on the actual 0.0f to 10.0f maximum limits of your knob!
    float vibeFactor = currentVibeValue / 10.0f; 
    vibeFactor = juce::jlimit(0.0f, 1.0f, vibeFactor);

    // Randomly spawn particles based on vibe intensity
    if (vibeFactor > 0.1f && random.nextFloat() < (vibeFactor * 0.4f))
    {
        Particle p;
        p.x = random.nextFloat() * static_cast<float>(getWidth());
        p.y = static_cast<float>(getHeight()) + 20.0f; 
        
        p.speedX = -1.5f + (random.nextFloat() * 3.0f); 
        p.speedY = (-4.0f + (random.nextFloat() * 3.0f)) * (1.0f + vibeFactor); 
        p.scale = 0.4f + (random.nextFloat() * 0.6f); 
        
        p.rotation = random.nextFloat() * juce::MathConstants<float>::twoPi;
        p.rotationSpeed = (-0.1f + (random.nextFloat() * 0.2f)) * vibeFactor; 
        p.isUnicorn = (random.nextFloat() > 0.4f); 

        particles.push_back (p);
    }

    // Physics Update
    for (auto& p : particles)
    {
        p.x += p.speedX;
        p.y += p.speedY;
        p.rotation += p.rotationSpeed;
    }

    // Garbage Collection
    particles.erase (
        std::remove_if (particles.begin(), particles.end(), 
            [](const Particle& p) { return p.y < -50.0f; }), 
        particles.end()
    );

    if (vibeFactor > 0.0f || !particles.empty())
        repaint();
}

void PamplejuceAudioProcessorEditor::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // CORRECTION: Background blending saturation fixed for 0.0 -> 10.0 range!
    float saturation = currentVibeValue / 10.0f;
    saturation = juce::jlimit (0.0f, 1.0f, saturation);

    juce::ignoreUnused (processorRef);

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

    // DRAW FLOATING PARTICLES RIGHT OVER BACKGROUND
    static auto unicornImg = juce::ImageCache::getFromMemory (BinaryData::unicorn_particle_png, BinaryData::unicorn_particle_pngSize);

    for (const auto& p : particles)
    {
        g.saveState();
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
            float starSize = 15.0f * p.scale;
            g.setColour (juce::Colours::cyan.interpolatedWith (juce::Colour (0xFFFF007F), random.nextFloat()));
            
            juce::Path star;
            star.startNewSubPath (p.x, p.y - starSize);
            star.quadraticTo (p.x, p.y, p.x + starSize, p.y);
            star.quadraticTo (p.x, p.y, p.x, p.y + starSize);
            star.quadraticTo (p.x, p.y, p.x - starSize, p.y);
            star.quadraticTo (p.x, p.y, p.x, p.y - starSize);
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
    
    // Position labels layout vertically at the bottom
    vibeDisplayLabel.setBounds(0, getHeight() - 60, getWidth(), 25);
    quoteLabel.setBounds(0, getHeight() - 35, getWidth(), 20);
}
