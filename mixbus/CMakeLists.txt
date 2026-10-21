cmake_minimum_required(VERSION 3.22)
project(SaltGlueBus VERSION 1.0.0)

# Set JUCE_DIR to your local JUCE checkout, or use -DJUCE_DIR=...
if(NOT DEFINED JUCE_DIR)
  message(FATAL_ERROR "Set JUCE_DIR to your JUCE source folder, e.g. -DJUCE_DIR=C:/JUCE")
endif()
add_subdirectory("${JUCE_DIR}" JUCE)
juce_add_plugin(SaltGlueBus
    COMPANY_NAME "Independent Audio Tools"
    PLUGIN_MANUFACTURER_CODE IAtl
    PLUGIN_CODE SgB1
    FORMATS VST3 Standalone
    PRODUCT_NAME "Salt Glue Bus"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)
target_sources(SaltGlueBus PRIVATE Source/PluginProcessor.cpp Source/PluginProcessor.h Source/PluginEditor.cpp Source/PluginEditor.h)
target_compile_features(SaltGlueBus PRIVATE cxx_std_17)
target_compile_definitions(SaltGlueBus PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_link_libraries(SaltGlueBus PRIVATE juce::juce_audio_utils juce::juce_dsp PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags juce::juce_recommended_warning_flags)
