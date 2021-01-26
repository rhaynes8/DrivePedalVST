/*
  ==============================================================================

    Distortion.h
    Created: 6 Oct 2020 10:08:25pm
    Author:  Richie Haynes

  ==============================================================================
*/
#include <JuceHeader.h>
#include "../JuceLibraryCode/JuceHeader.h"
#pragma once

class Distortion
{
public:
    Distortion();
    float gain;
    float centreFreq = 200;
    void prepare (const juce::dsp::ProcessSpec& spec);
    //template <typename ProcessContext>
    void process (juce::dsp::ProcessContextReplacing<float> context) noexcept;
    void reset() noexcept;
    void setGain(float gainValue);
    float getGain();
    void setFreq(float freq);
    float getFreq();
    

private:
    //==============================================================================
    
    
    
    enum
    {
        filterIndex,
        preGainIndex,
        waveshaperIndex,
        postGainIndex
    };
    
    using Filter = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;
    
    static const size_t oversamplingOrder = 4;
    static const int    oversamplingFactor = 1 << oversamplingOrder;
    static const int numChannels = 2;
    static const auto filterType = juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR;
    
    
    juce::dsp::ProcessorChain<juce::dsp::ProcessorDuplicator<Filter, FilterCoefs>,
                             juce::dsp::Gain<float>, juce::dsp::WaveShaper<float, std::function<float (float)>>, juce::dsp::Gain<float>> processorChain;
    juce::dsp::Oversampling<float> oversampling {numChannels, oversamplingOrder, filterType};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Distortion)
    
};

