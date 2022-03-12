/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SVF1AudioProcessorEditor::SVF1AudioProcessorEditor (SVF1AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 300);

    addAndMakeVisible(freqSlider);
    freqSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    freqSliderAttachmentPtr.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "cutoff", freqSlider));

    addAndMakeVisible(resSlider);
    resSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    resSliderAttachmentPtr.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "resonance", resSlider));

    addAndMakeVisible(typeBox);
    typeBox.addItem("Lowpass 12dB", 1);
    typeBox.addItem("Highpass 12dB", 2);
    typeBox.addItem("Bandpass 12dB", 3);
    typeBox.addItem("Notch", 4);
    typeBox.addItem("Peak", 5);
    typeBox.addItem("Lowpass 6dB", 6);
    typeBox.addItem("Highpass 6dB", 7);
    typeBox.addItem("Bandpass 12dB (n)", 8);
    typeBox.addItem("Lowpass 12dB (n)", 9);
    typeBox.addItem("Highpass 12dB (n)", 10);
    typeBoxAttachmentPtr.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(audioProcessor.apvts, "type", typeBox));

    addAndMakeVisible(mixSlider);
    mixSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mixSliderAttachmentPtr.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(audioProcessor.apvts, "mix", mixSlider));
}

SVF1AudioProcessorEditor::~SVF1AudioProcessorEditor()
{
}

//==============================================================================
void SVF1AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    //g.setColour (juce::Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void SVF1AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    freqSlider.setBounds(50, 50, 350, 50);
    resSlider.setBounds(50, 100, 350, 50);
    typeBox.setBounds(50, 170, 200, 22);
    mixSlider.setBounds(50, 230, 350, 50);
}
