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

        if (x > 1.0f) {
            return 1.0f;
        } else if (x < -1.0f) {
            return -1.0f;
        } else if (x > 0.0f && x < 1.0f) {
            return static_cast<float>((tanh(pow(10, this->getGain() / 20) * x))/tanh(pow(10, this->getGain() / 20)));
            
        } else if (x < 0.0f) {
            return std::tanh(x);
        } else {
            return 0.0f;
        }
 
        
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
    
    oversampling.initProcessing(spec.maximumBlockSize);
    
    processorChain.prepare (spec);
}

//==============================================================================
//template <typename ProcessContext>
void Distortion::process (juce::dsp::ProcessContextReplacing<float> context) noexcept
{
    auto oversampledBlock = oversampling.processSamplesUp (context.getInputBlock());
    
    processorChain.process (juce::dsp::ProcessContextReplacing<float> (oversampledBlock));
    
    oversampling.processSamplesDown (context.getOutputBlock());
}

//==============================================================================
void Distortion::reset() noexcept
{
    processorChain.reset();
    oversampling.reset();
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
