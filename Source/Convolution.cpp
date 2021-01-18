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
       
       juce::File path ("/Users/Richie/Desktop/JuceProjects/Plugins/DrivePedal");
       
       //path = path.getSpecialLocation(juce::File::currentApplicationFile);
       
       //juce::String filePath = path.getSpecialLocation(juce::File::currentExecutableFile).getCurrentWorkingDirectory().getFileName();
       
       
       //juce::String filePath = path.getCurrentWorkingDirectory().getFullPathName();
       
       
       
      // std::cout << ("Path = ") << filePath << std::endl;
       //DBG(filePath);
       
       auto& convolution = processorChain.template get<convolutionIndex>();

       convolution.loadImpulseResponse (path.getChildFile("Resources").getChildFile("TS808_LowVolumeIR_.wav"),
                                        juce::dsp::Convolution::Stereo::yes,
                                        juce::dsp::Convolution::Trim::yes,
                                        1024);
  
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
