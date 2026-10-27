cmake_minimum_required(VERSION 3.22)

project(IndependentMusicPlugin VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Pass -DJUCE_DIR="C:/path/to/JUCE" when configuring.
if(NOT DEFINED JUCE_DIR OR NOT EXISTS "${JUCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR
        "JUCE_DIR is not set or does not point to a JUCE source checkout. "
        "Example: -DJUCE_DIR=C:/Libraries/JUCE")
endif()

add_subdirectory("${JUCE_DIR}" "${CMAKE_BINARY_DIR}/JUCE")

juce_add_plugin(IndependentMusicPlugin
    COMPANY_NAME "IndependentAudio"
    BUNDLE_ID "com.independentaudio.musicplugin"
    PLUGIN_MANUFACTURER_CODE InAu
    PLUGIN_CODE ImP1
    PRODUCT_NAME "Independent Music Plugin"
    DESCRIPTION "Audio capture and music analysis starter"
    FORMATS VST3
    IS_SYNTH FALSE
    IS_EFFECT TRUE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(IndependentMusicPlugin PRIVATE
    plugin/PluginProcessor.cpp
    plugin/PluginProcessor.h
    plugin/PluginEditor.cpp
    plugin/PluginEditor.h
    plugin/CaptureBuffer.h
    plugin/CaptureBuffer.cpp
)

target_compile_definitions(IndependentMusicPlugin PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

target_link_libraries(IndependentMusicPlugin PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags
)

if(MSVC)
    target_compile_options(IndependentMusicPlugin PRIVATE /W4 /permissive-)
endif()
