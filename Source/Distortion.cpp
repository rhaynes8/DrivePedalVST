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
        
        //return tanh(pow(10, this->getGain() / 20) * x);
        
        // Allow gain to reach a zero value despite a logarithmic functionality
        // Take on a linear value for 0 <= x <= 1;
        /*if (getGain() < 1) {
            return tanh(double(this->getGain()) * x);
        } else {
            // log otherwise
            return tanh(pow(10, this->getGain() / 20) * x);
        //} */

        //return juce::jlimit (float (-0.75), float (0.75), this->getGain() * x);
 
        
    };
    
    auto& preGain = processorChain.template get<preGainIndex>();
    preGain.setGainDecibels (20.0f);
    
    auto& postGain = processorChain.template get<postGainIndex>();
    postGain.setGainDecibels (10.0f);
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
