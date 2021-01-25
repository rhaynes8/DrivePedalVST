/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once
#include "Distortion.h"
#include "Convolution.h"
#include <JuceHeader.h>

//==============================================================================
/**
*/
class PedalModellingAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    PedalModellingAudioProcessor();
    ~PedalModellingAudioProcessor() override;
    
    float volume;
    float previousVolume;

    juce::AudioProcessorValueTreeState filterTree;

    bool distortionToggle = 0;
    
    //==============================================================================
    void updateFilter();
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    juce::AudioBuffer<float> hfBuffer;
    juce::AudioBuffer<float> feedbackBuffer;
    juce::AudioBuffer<float> ogBuffer;
    
    Distortion hfDistortion;
    Distortion fbDistortion;
    Convolution hfConvolution;
    Convolution fbConvolution;

    // Declare to duplicate a mono object to create a stereo object and rename
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> toneControlFilter;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> hfHighPass;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> lfLowPass;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PedalModellingAudioProcessor)
};
