/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SVF1AudioProcessor::SVF1AudioProcessor()
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
    cutoff = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("cutoff"));
    jassert(cutoff != nullptr);

    resonance = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("resonance"));
    jassert(resonance != nullptr);

    type = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter("type"));
    jassert(type != nullptr);

    //mix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("mix"));
    //jassert(mix != nullptr);

    bypass = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter("bypass"));
    jassert(bypass != nullptr);
}

SVF1AudioProcessor::~SVF1AudioProcessor()
{
}

//==============================================================================
const juce::String SVF1AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SVF1AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SVF1AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SVF1AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SVF1AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SVF1AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SVF1AudioProcessor::getCurrentProgram()
{
    return 0;
}

void SVF1AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SVF1AudioProcessor::getProgramName (int index)
{
    return {};
}

void SVF1AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SVF1AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec;

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    filter.prepare(spec);
    filter.reset();

    //mixer.prepare(spec);
    //mixer.reset();
}

void SVF1AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SVF1AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainInputChannelSet()  == juce::AudioChannelSet::disabled()
     || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::disabled())

    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet();
  #endif
}
#endif

void SVF1AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    //mixer.pushDrySamples(buffer);
    //mixer.setWetMixProportion (mix->get());

    filter.setCutoffFrequency (cutoff->get());
    filter.setResonance (resonance->get());

    switch (type->getIndex())
    {
    case 0:
        filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        break;
    case 1:
        filter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
        break;
    case 2:
        filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
        break;
    case 3:
        filter.setType(juce::dsp::StateVariableTPTFilterType::notch);
        break;
    case 4:
        filter.setType(juce::dsp::StateVariableTPTFilterType::peak);
        break;
    case 5:
        filter.setType(juce::dsp::StateVariableTPTFilterType::lp);
        break;
    case 6:
        filter.setType(juce::dsp::StateVariableTPTFilterType::hp);
        break;
    default:
        filter.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    }

    auto block = juce::dsp::AudioBlock <float> (buffer);
    auto context = juce::dsp::ProcessContextReplacing <float> (block);

    context.isBypassed = bypass->get();

    filter.process(context);
}

//==============================================================================
bool SVF1AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SVF1AudioProcessor::createEditor()
{
    //return new SVF1AudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void SVF1AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void SVF1AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout SVF1AudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout layout;
    using namespace juce;

    auto cutoffRange = NormalisableRange<float>(20.00f, 20000.00f, 00.01f, 0.198893f);
    layout.add(std::make_unique<AudioParameterFloat>("cutoff", "Cutoff", cutoffRange, 632.45f));

    auto resonanceRange = NormalisableRange<float>(0.707107f, 100.000f, 00.001f, 0.271119f);
    layout.add(std::make_unique<AudioParameterFloat>("resonance", "Resonance", resonanceRange, 1.00f));

    layout.add(std::make_unique<AudioParameterChoice>("type", "Type", juce::StringArray{ "Lowpass 12dB", "Highpass 12dB", "Bandpass 12dB", "Notch", "Peak", "Lowpass 6dB", "Highpass 6dB" }, 1));

    //auto mixRange = NormalisableRange<float>(0.00f, 1.00f, 00.01f, 0.5f);
    //layout.add(std::make_unique<AudioParameterFloat>("mix", "Mix", mixRange, 1.00f));

    layout.add(std::make_unique<AudioParameterBool>("bypass", "Bypass", false));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SVF1AudioProcessor();
}
