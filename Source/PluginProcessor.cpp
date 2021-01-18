/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PedalModellingAudioProcessor::PedalModellingAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),

filterTree(*this, nullptr, "Parameter", { std::make_unique<juce::AudioParameterFloat> ("centreFreq", "CentreFreq", 200.0f, 2800.0f, 723.4f), std::make_unique<juce::AudioParameterFloat> ("gain", "Gain", 0.0f, 30.0f, 0.0f), std::make_unique<juce::AudioParameterFloat> ("volume", "Volume", -48.0f, 0.0f, -18.0f)   } ),



// Initialise the low pass filter - works exactly the same for other filters
hfDistortion(),
fbDistortion(),
hfConvolution(),
fbConvolution(),
//Q = fr / BW
toneControlFilter(juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(44100, 723.4)),
hfHighPass(juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(44100, 700)),
lfLowPass(juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(44100, 700))
//overallBandPass(juce::dsp::IIR::Coefficients<float>::makeBandPass(44100, 2000, 0.45))

#endif
{
}

PedalModellingAudioProcessor::~PedalModellingAudioProcessor()
{
}

//==============================================================================
const juce::String PedalModellingAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PedalModellingAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PedalModellingAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PedalModellingAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PedalModellingAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PedalModellingAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PedalModellingAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PedalModellingAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PedalModellingAudioProcessor::getProgramName (int index)
{
    return {};
}

void PedalModellingAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PedalModellingAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{

    // Set up the DSP module ready for processing
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getMainBusNumOutputChannels();
    
    hfBuffer.setSize(getTotalNumInputChannels(), samplesPerBlock);
    feedbackBuffer.setSize(getTotalNumInputChannels(), samplesPerBlock);
    ogBuffer.setSize(getTotalNumInputChannels(), samplesPerBlock);

    toneControlFilter.prepare(spec);
    toneControlFilter.reset();
    hfHighPass.prepare(spec);
    hfHighPass.reset();
    lfLowPass.prepare(spec);
    lfLowPass.reset();
    //overallBandPass.prepare(spec);
    //overallBandPass.reset();
    
    hfDistortion.prepare(spec);
    hfConvolution.prepare(spec);
    hfDistortion.reset();
    hfConvolution.reset();
    fbDistortion.prepare(spec);
    fbConvolution.prepare(spec);
    fbDistortion.reset();
    fbConvolution.reset();
    
    previousVolume = *filterTree.getRawParameterValue("volume");
}

void PedalModellingAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PedalModellingAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void PedalModellingAudioProcessor::updateFilter ()
{
    float centreFreq = *filterTree.getRawParameterValue("centreFreq");
    float gain = *filterTree.getRawParameterValue("gain");

    *toneControlFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), centreFreq);
    hfDistortion.setGain(gain);
    fbDistortion.setGain(gain/2);
    
    hfDistortion.setFreq(centreFreq);
    fbDistortion.setFreq(centreFreq);
    
    std::cout << "gain in dB = " << pow(10, hfDistortion.getGain() / 20) << std::endl;
    
}

void PedalModellingAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto bufferLength = buffer.getNumSamples();

    auto volume = pow(10, *filterTree.getRawParameterValue("volume") / 20);
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    //copy input into each band's buffer
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        // Write the buffer's data to do separate variables
        auto* channelData = buffer.getWritePointer (channel);
        auto* bufferData = buffer.getWritePointer (channel);
        //auto* originalData = buffer.getWritePointer (channel);
        // Copy the buffer data to our two new buffers
        hfBuffer.copyFrom(channel, 0, channelData, bufferLength);
        feedbackBuffer.copyFrom(channel, 0, bufferData, bufferLength);
        //ogBuffer.copyFrom(channel, 0, originalData, bufferLength);
        //ogBuffer.applyGain(0.5);
    }
    juce::dsp::AudioBlock<float> hfBlock(hfBuffer);
    juce::dsp::AudioBlock<float> feedbackBlock(feedbackBuffer);
    //juce::dsp::AudioBlock<float> ogBlock(ogBuffer);

    //overallBandPass.process(juce::dsp::ProcessContextReplacing<float>(ogBlock));
    
    if (convolutionToggle == 1) {
        hfConvolution.process(juce::dsp::ProcessContextReplacing<float>(hfBlock));
        fbConvolution.process(juce::dsp::ProcessContextReplacing<float>(feedbackBlock));
        DBG("Convolution On");
    } else {
        hfConvolution.reset();
        fbConvolution.reset();
        DBG("Convolution Off");
    }
    
    if (distortionToggle == 1) {
        hfDistortion.process(juce::dsp::ProcessContextReplacing<float>(hfBlock));
        fbDistortion.process(juce::dsp::ProcessContextReplacing<float>(feedbackBlock));
        updateFilter();
        DBG("Distortion On");
    } else {
        hfDistortion.reset();
        fbDistortion.reset();
        updateFilter();
        DBG("Distortion Off");
    }

    hfHighPass.process(juce::dsp::ProcessContextReplacing<float>(hfBlock));
    lfLowPass.process(juce::dsp::ProcessContextReplacing<float>(feedbackBlock));
    
    
    buffer.clear();
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* hfData = hfBuffer.getReadPointer(channel);
        auto* feedbackData = feedbackBuffer.getReadPointer(channel);
        //auto* ogData = ogBuffer.getReadPointer(channel);
        buffer.addFrom(channel, 0, hfData, bufferLength);
        buffer.addFrom(channel, 0, feedbackData, bufferLength);
        //buffer.addFrom(channel, 0, ogData, bufferLength);
    }
    
    if (volume == previousVolume)
    {
        // apply the current gain if there is no change in the level (slider value)
        buffer.applyGain(volume);
    } else {
        // Apply a ramp of buffer size (0 to 512 as default) if the gains are different
        buffer.applyGainRamp(0, buffer.getNumSamples(), previousVolume, volume);
        // Make sure the previous gain variable gets updated upon slider change
        previousVolume = volume;
    }
    
    // Summing output to original 'block'
    juce::dsp::AudioBlock<float> block(buffer);
    // Attaching pedal's tone control to block
    toneControlFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
    updateFilter();

}

//==============================================================================
bool PedalModellingAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PedalModellingAudioProcessor::createEditor()
{
    return new PedalModellingAudioProcessorEditor (*this);
}

//==============================================================================
void PedalModellingAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PedalModellingAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PedalModellingAudioProcessor();
}
