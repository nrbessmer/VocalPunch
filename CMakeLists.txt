cmake_minimum_required(VERSION 3.22)
project(VocalPunch VERSION 1.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "" FORCE)

# JUCE 8.0.7 - defaults to the shared copy next to this project (Projects/JUCE)
if(NOT DEFINED JUCE_PATH)
  set(JUCE_PATH "${CMAKE_SOURCE_DIR}/../JUCE" CACHE PATH "Path to JUCE")
endif()
if(NOT EXISTS "${JUCE_PATH}/CMakeLists.txt")
  message(FATAL_ERROR "JUCE not found at ${JUCE_PATH} (git clone https://github.com/juce-framework/JUCE.git --branch 8.0.7)")
endif()
add_subdirectory("${JUCE_PATH}" juce_build)

juce_add_plugin(VocalPunch
    COMPANY_NAME             "Tully EDM Vibe"
    COMPANY_COPYRIGHT        "(c)(p) 2026 Tully EDM Vibe"
    COMPANY_EMAIL            "info@tullyedmvibe.com"
    COMPANY_WEBSITE          "https://www.tullyedmvibe.com"
    BUNDLE_ID                com.TullyEDMVibe.VocalPunch
    PLUGIN_MANUFACTURER_CODE Tull
    PLUGIN_CODE              Vpch
    FORMATS                  AU VST3 Standalone
    PRODUCT_NAME             "VocalPunch"
    IS_SYNTH                 FALSE
    NEEDS_MIDI_INPUT         FALSE
    NEEDS_MIDI_OUTPUT        FALSE
    IS_MIDI_EFFECT           FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    VST3_CATEGORIES          "Fx|Dynamics|EQ"
    AU_MAIN_TYPE             kAudioUnitType_Effect
)

juce_add_binary_data(VocalPunchData SOURCES Resources/TullyLogo.png Resources/VocalPunchLogo.png)

target_sources(VocalPunch PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

target_compile_definitions(VocalPunch PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(VocalPunch
    PRIVATE
        VocalPunchData
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Optional self-test (cmake -DVP_TESTS=ON)
option(VP_TESTS "Build the self-test console app" OFF)
if(VP_TESTS)
  juce_add_console_app(VPTest PRODUCT_NAME "VPTest")
  target_compile_definitions(VPTest PRIVATE JucePlugin_Name="VocalPunch" JucePlugin_VersionString="1.0.0" JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
  target_link_libraries(VPTest PRIVATE VocalPunchData juce::juce_audio_utils juce::juce_dsp PUBLIC juce::juce_recommended_config_flags)
  target_sources(VPTest PRIVATE Tests/VPTest.cpp Source/PluginProcessor.cpp Source/PluginEditor.cpp)
endif()
