/*
  ==============================================================================

    Convolution.h
    Created: 6 Oct 2020 10:08:53pm
    Author:  Richie Haynes

  ==============================================================================
*/


#pragma once
#include <JuceHeader.h>
#include "../JuceLibraryCode/JuceHeader.h"

class Convolution
{
public:
    //==============================================================================
    Convolution();

    //==============================================================================
    void prepare (const juce::dsp::ProcessSpec& spec);
    //==============================================================================
    //template <typename ProcessContext>
    void process (juce::dsp::ProcessContextReplacing<float> context) noexcept;

    //==============================================================================
    void reset() noexcept;

private:
    //==============================================================================
    enum
    {
        convolutionIndex
    };
    juce::dsp::ProcessorChain<juce::dsp::Convolution> processorChain;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Convolution)
    
};

