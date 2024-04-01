/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define PARAMETER_VT_ID "root_parameters"

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
                       ),
        parameters(*this, nullptr, PARAMETER_VT_ID, createParameterLayout()),
        engine(parameters)
#endif
{
    
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
    juce::dsp::ProcessSpec procSpec;
    procSpec.sampleRate = sampleRate;
    procSpec.numChannels = 2; // TODO: figure this out
    procSpec.maximumBlockSize = samplesPerBlock;

    // TODO: if I wanted to make the engine/colonies take in a reference to procSpec, this has to be a member variable
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
    // TODO: solve ableton maxing out at beginning
    engine.setBPM(getPlayHead()->getPosition()->getBpm().orFallback(0));
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
    return new MicrobiomeAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void MicrobiomeAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void MicrobiomeAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
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

juce::AudioProcessorValueTreeState::ParameterLayout MicrobiomeAudioProcessor::createParameterLayout()
{
    // returning this is OK because APVST takes in parameters by value instead of by reference
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    for (int i = 1; i <= MAX_COLONY; i++) {
        layout.add(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID {PARAMETER_ENABLE_ID(i), 1}, 
            PARAMETER_ENABLE_NAME(i), 
            (i-1) < DEFAULT_COLONIES));

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID {PARAMETER_RESAMPLE_RATIO_ID(i), 1}, 
            PARAMETER_RESAMPLE_RATIO_NAME(i), 
            RESAMPLE_RATIO_MIN, 
            RESAMPLE_RATIO_MAX, 
            RESAMPLE_RATIO_DEF)
        );

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID {PARAMETER_RESAMPLE_START_ID(i), 1}, 
            PARAMETER_RESAMPLE_START_NAME(i), 
            RESAMPLE_START_MIN, 
            RESAMPLE_START_MAX, 
            RESAMPLE_START_DEF)
        );

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID {PARAMETER_COLONY_START_ID(i), 1}, 
            PARAMETER_COLONY_START_NAME(i), 
            COLONY_START_MIN, 
            COLONY_START_MAX, 
            COLONY_START_DEF)
        );

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID {PARAMETER_COLONY_END_ID(i), 1}, 
            PARAMETER_COLONY_END_NAME(i), 
            COLONY_END_MIN, 
            COLONY_END_MAX, 
            COLONY_END_DEF)
        );

        layout.add(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID {PARAMETER_COLONY_DBFS_ID(i), 1}, 
            PARAMETER_COLONY_DBFS_NAME(i), 
            COLONY_DBFS_MIN, 
            COLONY_DBFS_MAX, 
            COLONY_DBFS_DEF)
        );

        layout.add(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID{ PARAMETER_LOOP_ID(i), 1 },
            PARAMETER_LOOP_NAME(i),
            COLONY_LOOP_DEF)
        );

        layout.add(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID {PARAMETER_COLONY_GHOST_ID(i), 1}, 
            PARAMETER_COLONY_GHOST_NAME(i), 
            COLONY_GHOSTS_MIN, 
            COLONY_GHOSTS_MAX, 
            COLONY_GHOSTS_DEF)
        );    
    }

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ PARAMETER_ENGINE_WET_ID, 1},
        PARAMETER_ENGINE_WET_NAME,
        ENGINE_WET_MIN,
        ENGINE_WET_MAX,
        ENGINE_WET_DEF)
    );

    return layout;
}