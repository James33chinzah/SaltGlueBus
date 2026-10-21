#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::AudioProcessorValueTreeState::ParameterLayout makeParams() {
    using APF = juce::AudioParameterFloat;
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;
    p.push_back(std::make_unique<APF>("drive", "Salt", juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 12.f));
    p.push_back(std::make_unique<APF>("amount", "Glue", juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 20.f));
    p.push_back(std::make_unique<juce::AudioParameterBool>("punch", "Punch", false));
    p.push_back(std::make_unique<APF>("low", "Low Control", juce::NormalisableRange<float>(-6.f, 6.f, 0.1f), 0.f));
    p.push_back(std::make_unique<APF>("beef", "Beef", juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 0.f));
    p.push_back(std::make_unique<APF>("body", "Body", juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 0.f));
    p.push_back(std::make_unique<APF>("air", "Air", juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 0.f));
    p.push_back(std::make_unique<APF>("loud", "Output", juce::NormalisableRange<float>(-12.f, 12.f, 0.1f), 0.f));
    p.push_back(std::make_unique<APF>("mix", "Mix", juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 100.f));
    return { p.begin(), p.end() };
}
SaltGlueBusAudioProcessor::SaltGlueBusAudioProcessor()
 : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)), state(*this, nullptr, "PARAMETERS", makeParams()) {}
void SaltGlueBusAudioProcessor::prepareToPlay(double sr, int block) {
    compressor.prepare({ sr, (juce::uint32)block, (juce::uint32)getTotalNumOutputChannels() });
    compressor.setThreshold(-18.f); compressor.setRatio(2.f); compressor.setAttack(15.f); compressor.setRelease(120.f);
    juce::dsp::ProcessSpec spec{sr, (juce::uint32)block, (juce::uint32)getTotalNumOutputChannels()};
    lowShelfL.reset(); lowShelfR.reset(); airShelfL.reset(); airShelfR.reset();
    lowShelfL.prepare(spec); lowShelfR.prepare(spec); airShelfL.prepare(spec); airShelfR.prepare(spec);
}
void SaltGlueBusAudioProcessor::releaseResources() {}
bool SaltGlueBusAudioProcessor::isBusesLayoutSupported(const BusesLayout& l) const { return l.getMainInputChannelSet() == juce::AudioChannelSet::stereo() && l.getMainOutputChannelSet() == juce::AudioChannelSet::stereo(); }
void SaltGlueBusAudioProcessor::processBlock(juce::AudioBuffer<float>& b, juce::MidiBuffer&) {
    juce::ScopedNoDenormals noDenormals;
    const int n = b.getNumSamples(); if (b.getNumChannels() < 2) return;
    juce::AudioBuffer<float> dry; dry.makeCopyOf(b);
    auto val = [this](const char* id) { return state.getRawParameterValue(id)->load(); };
    const float amount = val("amount"), drive = val("drive"), low = val("low"), beef = val("beef"), body = val("body"), air = val("air"), out = val("loud"), mix = val("mix") / 100.f;
    compressor.setRatio(val("punch") > .5f ? 3.2f : 2.0f);
    compressor.setThreshold(-3.f - amount * .22f);
    compressor.setAttack(val("punch") > .5f ? 5.f : 18.f);
    compressor.setRelease(val("punch") > .5f ? 75.f : 150.f);
    auto lowC = juce::dsp::IIR::Coefficients<float>::makeLowShelf(getSampleRate(), 110.0, 0.707, juce::Decibels::decibelsToGain(low + beef * .035f));
    auto airC = juce::dsp::IIR::Coefficients<float>::makeHighShelf(getSampleRate(), 8500.0, 0.707, juce::Decibels::decibelsToGain(air * .045f + body * .015f));
    *lowShelfL.coefficients = *lowC; *lowShelfR.coefficients = *lowC; *airShelfL.coefficients = *airC; *airShelfR.coefficients = *airC;
    juce::dsp::AudioBlock<float> block(b); juce::dsp::ProcessContextReplacing<float> ctx(block); compressor.process(ctx);
    for (int ch=0; ch<2; ++ch) {
        auto* x = b.getWritePointer(ch); auto* d = dry.getReadPointer(ch);
        auto& lf = ch == 0 ? lowShelfL : lowShelfR; auto& hf = ch == 0 ? airShelfL : airShelfR;
        for (int i=0; i<n; ++i) {
            float y = x[i]; y = lf.processSample(y); y = hf.processSample(y);
            const float driven = std::tanh(y * (1.f + drive * .025f));
            y = y * (1.f - drive/100.f) + driven * (drive/100.f);
            const float gain = juce::Decibels::decibelsToGain(out + body * .015f);
            x[i] = (d[i] * (1.f-mix) + y * mix) * gain;
        }
    }
}
juce::AudioProcessorEditor* SaltGlueBusAudioProcessor::createEditor() { return new SaltGlueBusAudioProcessorEditor(*this); }
void SaltGlueBusAudioProcessor::getStateInformation(juce::MemoryBlock& dest) { auto xml = state.copyState().createXml(); copyXmlToBinary(*xml, dest); }
void SaltGlueBusAudioProcessor::setStateInformation(const void* data, int size) { std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, size)); if (xml && xml->hasTagName(state.state.getType())) state.replaceState(juce::ValueTree::fromXml(*xml)); }
