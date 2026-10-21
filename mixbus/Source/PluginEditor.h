#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
class SaltGlueBusAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    explicit SaltGlueBusAudioProcessorEditor(SaltGlueBusAudioProcessor&);
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    SaltGlueBusAudioProcessor& processor;
    juce::Label title;
    juce::Slider salt, glue, low, beef, body, air, output, mix;
    juce::ToggleButton punch { "PUNCH MODE" };
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> aSalt,aGlue,aLow,aBeef,aBody,aAir,aOutput,aMix;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> aPunch;
    void setupSlider(juce::Slider&, const juce::String&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SaltGlueBusAudioProcessorEditor)
};
