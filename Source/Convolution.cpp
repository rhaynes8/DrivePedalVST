/*
  ==============================================================================

    Convolution.cpp
    Created: 6 Oct 2020 10:08:45pm
    Author:  Richie Haynes

  ==============================================================================
*/

#include "Convolution.h"
#include <JuceHeader.h>

Convolution::Convolution()
   {
       
       // Bad for real-time convolution - Try and implement another way
       
       juce::File path = juce::File::getCurrentWorkingDirectory();
       
       auto& convolution = processorChain.template get<convolutionIndex>();

       if (path.exists())
       {
           convolution.loadImpulseResponse (path.getChildFile("Resources").getChildFile("TS808Highs_CrankedIR.wav"),
                                            juce::dsp::Convolution::Stereo::yes,
                                            juce::dsp::Convolution::Trim::yes,
                                            1024);
       } else {
           std::cout << "Check IR is in Resources folder" << std::endl;
       }
  
   }

   //==============================================================================
   void Convolution::prepare (const juce::dsp::ProcessSpec& spec)
   {
       processorChain.prepare(spec);
   }

   //==============================================================================
   //template <typename ProcessContext>
   void Convolution::process (juce::dsp::ProcessContextReplacing<float> context) noexcept
   {
       processorChain.process (context);
   }

   //==============================================================================
   void Convolution::reset() noexcept
   {
       processorChain.reset();
   }
