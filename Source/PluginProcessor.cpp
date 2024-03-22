/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MicrobiomeAudioProcessor::MicrobiomeAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    reverbWet = .8f;
}

MicrobiomeAudioProcessor::~MicrobiomeAudioProcessor()
{
}

//==============================================================================
const juce::String MicrobiomeAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MicrobiomeAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MicrobiomeAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MicrobiomeAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MicrobiomeAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MicrobiomeAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MicrobiomeAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MicrobiomeAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MicrobiomeAudioProcessor::getProgramName (int index)
{
    return {};
}

void MicrobiomeAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MicrobiomeAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    DBG("Preparing to Play...");
    DBG("Sample Rate: " << sampleRate << "\nBlock Size: " << samplesPerBlock);
    testVerb.setSampleRate(sampleRate);

    testDelay.setMaximumDelayInSamples(sampleRate * 20);
    testDelay.setDelay(sampleRate * 15);
    juce::dsp::ProcessSpec procSpec;
    procSpec.sampleRate = sampleRate;
    procSpec.numChannels = 2; // TODO: figure this out
    procSpec.maximumBlockSize = samplesPerBlock;
    testDelay.prepare(procSpec);

    EngineParams engineParams;
    engineParams.procSpec = procSpec;

    engine.prepare(engineParams);
}

void MicrobiomeAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    DBG("Releasing Resources...");
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MicrobiomeAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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
#endif

void MicrobiomeAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    //DBG("Wet Level: " << reverbWet);

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    //buffer.applyGain(0.5);
    // for (int channel = 0; channel < totalNumInputChannels; ++channel)
    // {
    //     auto* channelData = buffer.getWritePointer (channel);
        
    //     for (int i = 0; i < buffer.getNumSamples(); i++) {
    //         auto inVal = channelData[i];
    //         channelData[i] = channelData[i] + 0.5 * testDelay.popSample(channel);
    //         testDelay.pushSample(channel, channelData[i]); // switch this to *channelData to do feedback
    //     }
    // }

    // TODO: return immediately if no samples
    engine.processAudio(buffer);
    // testVerb.processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), buffer.getNumSamples());
}

//==============================================================================
bool MicrobiomeAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MicrobiomeAudioProcessor::createEditor()
{
    return new MicrobiomeAudioProcessorEditor (*this);
}

//==============================================================================
void MicrobiomeAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MicrobiomeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void MicrobiomeAudioProcessor::setReverbWet(float wet)
{
    if (wet >= 0.0 && wet <= 1.0) {
        reverbWet = wet;
        testVerbParams.damping = 0.8f;
        testVerbParams.dryLevel = 0.3;
        testVerbParams.wetLevel = reverbWet;
        testVerbParams.roomSize = 0.65f;
        testVerb.setParameters(testVerbParams);
    }
}

MicrobiomeEngine& MicrobiomeAudioProcessor::getEngine()
{
    return engine;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MicrobiomeAudioProcessor();
}
