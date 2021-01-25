/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PedalModellingAudioProcessorEditor::PedalModellingAudioProcessorEditor (PedalModellingAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 600);
    
    // TONE CONTROL GUI COMPONENTS
    toneControlFreqDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    toneControlFreqDial.setRange(200.0f, 2800.0f);
    toneControlFreqDial.setValue(723.4);
    toneControlFreqDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    toneControlFreqDial.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(&toneControlFreqDial);
    
    toneControlFreqLabel.setText("Tone", juce::dontSendNotification);
    toneControlFreqLabel.attachToComponent(&toneControlFreqDial, false);
    toneControlFreqDial.setSkewFactorFromMidPoint(1000);
    addAndMakeVisible(&toneControlFreqLabel);

    toneControlFreqValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.filterTree, "centreFreq", toneControlFreqDial);
    
    
    // GAIN CONTROL GUI COMPONENTS
    gainDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    gainDial.setRange(0.0f, 41.0f);
    gainDial.setValue(0.0f);
    gainDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    gainDial.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(&gainDial);
    
    gainLabel.setText("Gain", juce::dontSendNotification);
    gainLabel.attachToComponent(&gainDial, false);
    addAndMakeVisible(&gainLabel);

    gainValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.filterTree, "gain", gainDial);
    
    // VOLUME CONTROL GUI COMPONENTS
    volumeDial.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    volumeDial.setRange(-48.0f, 0.0f);
    volumeDial.setValue(-18.0f);
    volumeDial.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeDial.setPopupDisplayEnabled(true, true, this);
    addAndMakeVisible(&volumeDial);

    volumeLabel.setText("Volume (dB)", juce::dontSendNotification);
    volumeLabel.attachToComponent(&volumeDial, false);
    addAndMakeVisible(&volumeLabel);
    
    volumeValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.filterTree, "volume", volumeDial);
    
    // DISTORTION GUI COMPONENTS
    distortionButton.onClick = [this] { distortionButtonClicked(); };
    distortionButton.setButtonText("On/Off");
    addAndMakeVisible(&distortionButton);
    
}


void PedalModellingAudioProcessorEditor::distortionButtonClicked()
{
    if (audioProcessor.distortionToggle == 1) {
        audioProcessor.distortionToggle = 0;
    } else {
        audioProcessor.distortionToggle = 1;
    }
}

PedalModellingAudioProcessorEditor::~PedalModellingAudioProcessorEditor()
{
}

//==============================================================================
void PedalModellingAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::Rectangle<int> titleArea (0, 10, getWidth() - 25, 10);
    // Draw GUI components to the canvas
    g.fillAll(juce::Colours::seagreen);
    g.setColour (juce::Colours::black);
    g.drawText("Tube Screamer", titleArea, juce::Justification::centredTop);
}

void PedalModellingAudioProcessorEditor::resized()
{
    // Set bounds for GUI components in the canvas
    gainDial.setBounds(75, 150, 100, 100);
    volumeDial.setBounds(225, 150, 100, 100);
    toneControlFreqDial.setBounds(150, 275, 100, 100);
    distortionButton.setBounds(162.5, 450, 80, 80);
}


