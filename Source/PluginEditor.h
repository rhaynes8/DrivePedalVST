/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class PedalModellingAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PedalModellingAudioProcessorEditor (PedalModellingAudioProcessor&);
    ~PedalModellingAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void distortionButtonClicked();

private:

    juce::Slider toneControlFreqDial;
    juce::Slider gainDial;
    juce::Label toneControlFreqLabel;
    juce::Label gainLabel;
    juce::TextButton distortionButton;
    
    juce::Slider volumeDial;
    juce::Label volumeLabel;
    
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> toneControlFreqValue;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> gainValue;
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> volumeValue;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PedalModellingAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PedalModellingAudioProcessorEditor)
};
