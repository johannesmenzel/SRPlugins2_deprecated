#define PLUG_NAME "SRSynth"
#define PLUG_MFR "SRPlugins"
#define PLUG_VERSION_HEX 0x00000001
#define PLUG_VERSION_STR "0.0.1"
#define PLUG_UNIQUE_ID 'Ydgz'
#define PLUG_MFR_ID 'Acme'
#define PLUG_URL_STR "https://github.com/johannesmenzel"
#define PLUG_EMAIL_STR "johannes.menzel@posteo.de"
#define PLUG_COPYRIGHT_STR "Copyright 2019 Acme Inc"
#define PLUG_CLASS_NAME SRSynth

#define BUNDLE_NAME "SRSynth"
#define BUNDLE_MFR "SRPlugins"
#define BUNDLE_DOMAIN "com"

#define PLUG_CHANNEL_IO "0-2"
#define SHARED_RESOURCES_SUBPATH "SRSynth"

#define PLUG_LATENCY 0
#define PLUG_TYPE 1
#define PLUG_DOES_MIDI_IN 1
#define PLUG_DOES_MIDI_OUT 1
#define PLUG_DOES_MPE 1
#define PLUG_DOES_STATE_CHUNKS 0
#define PLUG_HAS_UI 1
#define PLUG_WIDTH 1024
#define PLUG_HEIGHT 669
#define PLUG_FPS 60
#define PLUG_SHARED_RESOURCES 0
#define PLUG_HOST_RESIZE 0

#define AUV2_ENTRY SRSynth_Entry
#define AUV2_ENTRY_STR "SRSynth_Entry"
#define AUV2_FACTORY SRSynth_Factory
#define AUV2_VIEW_CLASS SRSynth_View
#define AUV2_VIEW_CLASS_STR "SRSynth_View"

#define AAX_TYPE_IDS 'IPI1', 'IPI2'
#define AAX_PLUG_MFR_STR "Acme"
#define AAX_PLUG_NAME_STR "SRSynth\nIPIS"
#define AAX_DOES_AUDIOSUITE 0
#define AAX_PLUG_CATEGORY_STR "Synth"

#define VST3_SUBCATEGORY "Instrument|Synth"

#define APP_NUM_CHANNELS 2
#define APP_N_VECTOR_WAIT 0
#define APP_MULT 1
#define APP_COPY_AUV3 0
#define APP_SIGNAL_VECTOR_SIZE 64

#define ROBOTO_FN "Roboto-Regular.ttf"
