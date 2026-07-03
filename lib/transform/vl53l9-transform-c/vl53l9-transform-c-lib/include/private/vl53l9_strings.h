#ifndef VL53L9_STRINGS_H
#define VL53L9_STRINGS_H

#if VL53L9_TRANSFORM_DEBUG == 1
#define CTRL_BYPASS_R2P_ALGO_NICK                  "bypass-r2p-algo"
#define CTRL_BYPASS_R2P_ALGO_DESCRIPTION           "TODO"
#define CTRL_BYPASS_SHARPENER_ALGO_NICK            "bypass-sharpener-algo"
#define CTRL_BYPASS_SHARPENER_ALGO_DESCRIPTION     "TODO"
#define CTRL_BYPASS_R2P_FILTER_NICK                "bypass-r2p-filter"
#define CTRL_BYPASS_R2P_FILTER_DESCRIPTION         "TODO"
#define CTRL_BYPASS_CONFIDENCE_FILTER_NICK         "bypass-conf-filter"
#define CTRL_BYPASS_CONFIDENCE_FILTER_DESCRIPTION  "TODO"
#define CTRL_BYPASS_REFLECTANCE_FILTER_NICK        "bypass-refl-filter"
#define CTRL_BYPASS_REFLECTANCE_FILTER_DESCRIPTION "TODO"
#define CTRL_BYPASS_SHARPENER_FILTER_NICK          "bypass-sharpener-filter"
#define CTRL_BYPASS_SHARPENER_FILTER_DESCRIPTION   "TODO"
#define CTRL_CALIB_BUFFER_NICK                     "calib-buffer"
#define CTRL_CALIB_BUFFER_DESCRIPTION              "TODO"
#define CTRL_COVER_GLASS_NICK                      "cover-glass"
#define CTRL_COVER_GLASS_DESCRIPTION               "TODO"
#define CTRL_CUSTOM_OUTPUT_NICK                    "custom-output"
#define CTRL_CUSTOM_OUTPUT_DESCRIPTION             "TODO"
#else
#define CTRL_BYPASS_R2P_ALGO_NICK                  ""
#define CTRL_BYPASS_R2P_ALGO_DESCRIPTION           ""
#define CTRL_BYPASS_SHARPENER_ALGO_NICK            ""
#define CTRL_BYPASS_SHARPENER_ALGO_DESCRIPTION     ""
#define CTRL_BYPASS_R2P_FILTER_NICK                ""
#define CTRL_BYPASS_R2P_FILTER_DESCRIPTION         ""
#define CTRL_BYPASS_CONFIDENCE_FILTER_NICK         ""
#define CTRL_BYPASS_CONFIDENCE_FILTER_DESCRIPTION  ""
#define CTRL_BYPASS_REFLECTANCE_FILTER_NICK        ""
#define CTRL_BYPASS_REFLECTANCE_FILTER_DESCRIPTION ""
#define CTRL_BYPASS_SHARPENER_FILTER_NICK          ""
#define CTRL_BYPASS_SHARPENER_FILTER_DESCRIPTION   ""
#define CTRL_CALIB_BUFFER_NICK                     ""
#define CTRL_CALIB_BUFFER_DESCRIPTION              ""
#define CTRL_COVER_GLASS_NICK                      ""
#define CTRL_COVER_GLASS_DESCRIPTION               ""
#define CTRL_CUSTOM_OUTPUT_NICK                    ""
#define CTRL_CUSTOM_OUTPUT_DESCRIPTION             ""
#endif

#if VL53L9_TRANSFORM_DEBUG == 1
#define STREAM_RAW_DESCRIPTION        "Raw input stream from sensor"
#define STREAM_DEPTH_DESCRIPTION      "Depth stream containing calibrated depth data"
#define STREAM_AMPLITUDE_DESCRIPTION  "Amplitude stream containing amplitude data"
#define STREAM_AMBIENT_DESCRIPTION    "Ambient stream containing ambient light data"
#define STREAM_CONFIDENCE_DESCRIPTION "Confidence stream containing confidence data"
#else
#define STREAM_RAW_DESCRIPTION        ""
#define STREAM_DEPTH_DESCRIPTION      ""
#define STREAM_AMPLITUDE_DESCRIPTION  ""
#define STREAM_AMBIENT_DESCRIPTION    ""
#define STREAM_CONFIDENCE_DESCRIPTION ""
#endif

#endif // VL53L9_STRINGS_H
