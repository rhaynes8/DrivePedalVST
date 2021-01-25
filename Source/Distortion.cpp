/*
  ==============================================================================

    Distortion.cpp
    Created: 6 Oct 2020 10:08:34pm
    Author:  Richie Haynes

  ==============================================================================
*/
#include "../JuceLibraryCode/JuceHeader.h"
#include "Distortion.h"
#include <JuceHeader.h>


Distortion::Distortion()
{
    auto& waveshaper = processorChain.template get<waveshaperIndex>();
    waveshaper.functionToUse = [&, this] (float x)

    {
        return (tanh(pow(10, this->getGain() / 20) * x))/tanh(pow(10, this->getGain() / 20));
 
        
    };
    
    auto& preGain = processorChain.template get<preGainIndex>();
    preGain.setGainDecibels (20.0f);
    
    auto& postGain = processorChain.template get<postGainIndex>();
    postGain.setGainDecibels (0.0f);
}

//==============================================================================
void Distortion::prepare (const juce::dsp::ProcessSpec& spec)
{
    auto& filter = processorChain.template get<filterIndex>();
    filter.state = FilterCoefs::makeFirstOrderLowPass (spec.sampleRate, 3500.0f);
    filter.state = FilterCoefs::makeFirstOrderHighPass(spec.sampleRate, 50.0f);
    
    processorChain.prepare (spec);
}

//==============================================================================
//template <typename ProcessContext>
void Distortion::process (juce::dsp::ProcessContextReplacing<float> context) noexcept
{
    processorChain.process (context);
}

//==============================================================================
void Distortion::reset() noexcept
{
    processorChain.reset();
}

void Distortion::setGain(float gainValue)
{
    gain = gainValue;
}

float Distortion::getGain()
{
    return gain;
}

void Distortion::setFreq(float freq)
{
    centreFreq = freq;
}

float Distortion::getFreq()
{
    return centreFreq;
}
