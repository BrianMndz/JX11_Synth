#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JX11SynthAudioProcessor::JX11SynthAudioProcessor()
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

JX11SynthAudioProcessor::~JX11SynthAudioProcessor()
{
}

//==============================================================================
const juce::String JX11SynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JX11SynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool JX11SynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool JX11SynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double JX11SynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JX11SynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int JX11SynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JX11SynthAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String JX11SynthAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void JX11SynthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void JX11SynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.allocateResources(sampleRate, samplesPerBlock);
    reset();
}

void JX11SynthAudioProcessor::releaseResources()
{
    synth.deallocateResources();
}

bool JX11SynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void JX11SynthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                            juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    splitBufferByEvents(buffer, midiMessages);

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        juce::ignoreUnused (channelData);
        // ..do something to the data...
    }
}

//==============================================================================
bool JX11SynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* JX11SynthAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void JX11SynthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void JX11SynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

void JX11SynthAudioProcessor::splitBufferByEvents(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages)
{
    //Render the audio that happens before this event
    int bufferOffset = 0;
    for (const auto metadata : midiMessages) {
        int samplesThisSegment = metadata.samplePosition - bufferOffset;
        if (samplesThisSegment > 0) {
            render(buffer, samplesThisSegment, bufferOffset);
            bufferOffset += samplesThisSegment;
        }

        //Handle the MIDI event and ignore what is not needed
        if (metadata.numBytes <= 3) {
            uint8_t data_1 = (metadata.numBytes >= 2) ? metadata.data[1] : 0;
            uint8_t data_2 = (metadata.numBytes == 3) ? metadata.data[2] : 0;
            handleMIDI(metadata.data[0], data_1, data_2);
        }
    }

    //Render the audio after the last midi event. If no event, render entire buffer
    int samplesLastSegment = buffer.getNumSamples() - bufferOffset;
    if (samplesLastSegment > 0)
        render(buffer, samplesLastSegment, bufferOffset);

    midiMessages.clear();
}

void JX11SynthAudioProcessor::handleMIDI(uint8_t data_0, uint8_t data_1, uint8_t data_2)
{
    synth.midiMessage(data_0, data_1, data_2);

    char s[16];
    snprintf(s, 16, "%02hhX %02hhX %02hhX", data_0, data_1, data_2);
    DBG(s);
}

void JX11SynthAudioProcessor::render(juce::AudioBuffer<float> &buffer, int sampleCount, int bufferOffset)
{
    juce::ignoreUnused(buffer, sampleCount, bufferOffset);
}

void JX11SynthAudioProcessor::reset() {
    synth.reset();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JX11SynthAudioProcessor();
}
