cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)
#target_compile_features (fxseq-${format} PRIVATE cxx_std_20)

set(COMP_TARGET"linux") #linux or windows

if (COMP_TARGET STREQUAL "windows")

    set(CMAKE_SYSTEM_NAME Windows)
    add_definitions(-D_WIN32_WINNT=0x0603) # windows vista is 0x0600, 8.1 is 0x0603
    set(CMAKE_SYSTEM_PROCESSOR x86_64)
    set(THREADS_PREFER_PTHREAD_FLAG ON)

    set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)
    set(TOOLCHAIN_SUFFIX -posix) #or ""
    set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc${TOOLCHAIN_SUFFIX})
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++${TOOLCHAIN_SUFFIX})
    set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)
    set(CMAKE_CXX_COMPILER /usr/bin/x86_64-w64-mingw32-g++${TOOLCHAIN_SUFFIX})
   
    #set(CMAKE_FIND_ROOT_PATH /usr/share/mingw-w64/include)
    set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32/include/) #path to mingw windows sdk
endif()

project (fxseq VERSION 0.1)

# Adding JUCE
add_subdirectory (JUCE)
#add_subdirectory (JuceLibraryCode)

# The subdirectory contains its own CMakeLists that creates BinaryData targets containing graphics and fonts
#add_subdirectory (BinaryResources)

# Adding JSON
#set (JSON_BuildTests OFF CACHE INTERNAL "")
#set (JSON_Install OFF CACHE INTERNAL "")
#add_subdirectory (Ext/JBPluginBase)
#add_subdirectory (Ext/Resvg4JUCE)

if (NOT DEFINED COPY_PLUGIN_AFTER_BUILD)
    set (COPY_PLUGIN_AFTER_BUILD TRUE)
endif()

function (add_fxseq_version format)

# Add the plugin target itself
juce_add_plugin (fxseq-${format}
        VERSION "0.1.0"
        COMPANY_NAME "ssabug"
        COMPANY_WEBSITE "https://github.com/ssabug/fxseq"
        PRODUCT_NAME fxseq
        PLUGIN_NAME fxseq
        PLUGIN_MANUFACTURER_CODE Sabg
        PLUGIN_CODE Fxsq
        VST3_CATEGORIES Fx
        BUNDLE_ID "com.ssabug.fxseq"
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT FALSE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE

        COPY_PLUGIN_AFTER_BUILD ${COPY_PLUGIN_AFTER_BUILD}
        FORMATS ${format}

        ${ARGN})


#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall  -O3 -march=native -std=c++11")


# Gather some info regarding our git commit to inject them into a header file.
#jb_add_git_version_info (fxseq-${format})

# Adding all source files to the plugin target
target_sources (fxseq-${format}
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/ui/EffectComponent.cpp
        Source/ui/OptionsComponent.cpp
        Source/ui/OutputComponent.cpp
        Source/ui/SequencerComponent.cpp
        Source/ui/SequenceSequencerComponent.cpp)

target_compile_definitions (fxseq-${format}
    PUBLIC
        DONT_SET_USING_JUCE_NAMESPACE=1
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_STRICT_REFCOUNTEDPTR=1
        JUCE_VST3_CAN_REPLACE_VST2=0
        JB_INCLUDE_JSON=1)

target_link_libraries (fxseq-${format}
    PRIVATE
        # JUCE Modules
        juce::juce_dsp

        # Custom modules
        #jb_plugin_base
        #jb::Resvg4JUCE

        # JSON dependncy in plugin base
        #nlohmann_json::nlohmann_json

        # Binary Data
        #EmbeddedBinaryData

        # Recommended flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
        juce::juce_recommended_config_flags)

if (UNIX AND NOT APPLE)
    set_target_properties (fxseq-${format}
        PROPERTIES
            POSITION_INDEPENDENT_CODE TRUE)

   # set_target_properties (jb_git_version-fxseq-${format}
   #     PROPERTIES
   #         POSITION_INDEPENDENT_CODE TRUE)
endif()

endfunction()

if (UNIX AND NOT APPLE)
    #set_target_properties (EmbeddedBinaryData
    #    PROPERTIES
    #        POSITION_INDEPENDENT_CODE TRUE)
endif()

add_fxseq_version (VST3
        # A four-character manufacturer id with at least one upper-case character
        # A unique four-character plugin id with at least one upper-case character

        # When the fxseq was released, there was a bug in JUCE that wrongly reported the plugin code "proj". Applying this
        # to not break existing sessions
        USE_LEGACY_COMPATIBILITY_PLUGIN_CODE TRUE)

        target_compile_definitions (fxseq-VST3 PRIVATE JUCE_DISPLAY_SPLASH_SCREEN=0)



add_fxseq_version (LV2
        # A four-character manufacturer id with at least one upper-case character
        PLUGIN_MANUFACTURER_CODE Sabg
        # A unique four-character plugin id with at least one upper-case character
        PLUGIN_CODE Fxsq
        # When the fxseq was released, there was a bug in JUCE that wrongly reported the plugin code "proj". Applying this
        # to not break existing sessions
        USE_LEGACY_COMPATIBILITY_PLUGIN_CODE TRUE

        LV2URI "http://github.com/ssabug/fxseq"
        LVZ_SHARED_LIBRARY_NAME fxseq)
        target_compile_definitions (fxseq-LV2 PRIVATE JUCE_DISPLAY_SPLASH_SCREEN=0)


if (APPLE)
add_fxseq_version (AU
        # A four-character manufacturer id with at least one upper-case character
        PLUGIN_MANUFACTURER_CODE Sabg
        # A unique four-character plugin id with at least one upper-case character
        PLUGIN_CODE Fxsq

        AU_MAIN_TYPE kAudioUnitType_Effect)
        target_compile_definitions (fxseq-AU PRIVATE JUCE_DISPLAY_SPLASH_SCREEN=0)
endif()

if (AAX_SDK_PATH)

    if (WIN32)
        # Extra escaping is needed to not mess up the macro inside the AAX wrapper with the path created by the
        # GitHub Actions runner
        string (REPLACE "\\" "\\\\" AAX_SDK_PATH ${AAX_SDK_PATH})
    endif()

    juce_set_aax_sdk_path ("${AAX_SDK_PATH}")
    add_fxseq_version (AAX
            # A four-character manufacturer id with at least one upper-case character
            PLUGIN_MANUFACTURER_CODE Sabg
            # A unique four-character plugin id with at least one upper-case character
            PLUGIN_CODE Fxsq

            AAX_IDENTIFIER io.ssabug.fxseq
            AAX_CATEGORY AAX_ePlugInCategory_Harmonic)

    set_target_properties(fxseq-AAX_AAX PROPERTIES OSX_ARCHITECTURES x86_64)
    
    target_compile_definitions (fxseq-AAX
        PRIVATE
            JUCE_DISPLAY_SPLASH_SCREEN=1
            _CRT_SECURE_NO_WARNINGS=1)
endif()

add_library (fxseq-ALL_FORMATS INTERFACE)
target_link_libraries (fxseq-ALL_FORMATS
    INTERFACE
    fxseq-AU_AU
    fxseq-VST3_VST3
    fxseq-LV2_LV2
    fxseq-AAX_AAX)
