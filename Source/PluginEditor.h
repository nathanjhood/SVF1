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
class SVF1AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SVF1AudioProcessorEditor (SVF1AudioProcessor&);
    ~SVF1AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SVF1AudioProcessor& audioProcessor;

    juce::Slider freqSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> freqSliderAttachmentPtr;

    juce::Slider resSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> resSliderAttachmentPtr;

    juce::ComboBox typeBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> typeBoxAttachmentPtr;

    juce::Slider mixSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixSliderAttachmentPtr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SVF1AudioProcessorEditor)
};
