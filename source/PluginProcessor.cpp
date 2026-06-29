#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PamplejuceAudioProcessor::PamplejuceAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
}

PamplejuceAudioProcessor::~PamplejuceAudioProcessor()
{
}

//==============================================================================
const juce::String PamplejuceAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PamplejuceAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PamplejuceAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PamplejuceAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PamplejuceAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PamplejuceAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PamplejuceAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PamplejuceAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String PamplejuceAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void PamplejuceAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void PamplejuceAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (sampleRate, samplesPerBlock);
}

void PamplejuceAudioProcessor::releaseResources()
{
}

bool PamplejuceAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void PamplejuceAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
}

//==============================================================================
bool PamplejuceAudioProcessor::hasEditor() const
{
    return true; 
}

juce::AudioProcessorEditor* PamplejuceAudioProcessor::createEditor()
{
    return new PamplejuceAudioProcessorEditor (*this);
}

//==============================================================================
void PamplejuceAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::ignoreUnused (destData);
}

void PamplejuceAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PamplejuceAudioProcessor();
}
