#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdlib.h>

/**
 * @brief sharpener module constants and constants computed and/or extracted from OTP
 *
 * Based on Python algo R_1.3.0
 *
 * @param invalid_distance distance where a pixel is considered invalid
 * @param min_range_threshold_mm the minimum range threshold, for grouping, to be used in the event of very close
 *  targets
 * @param scale_range_threshd_by_range when set, scale the range threshold according to the target range, so that
 *  targets with the same degree of tilt will be grouped together
 * @param range_threshold_factor proportion of the current range to use as a threshold for grouping
 * @param enable_max_range_threshold when set, the maximum range threshold, for grouping. Distance above this threshold
 *  regarding the first element of group is considering to be a new group
 * @param max_range_threshold_mm_6_step the maximum range threshold between first pixel and the current one, for
 *  grouping on 6 step capture
 * @param max_range_threshold_mm_7_step the maximum range threshold between first pixel and the current one, for
 *  grouping on 7 step capture
 * @param enable_distance distance between current pixel and barycenter of the group is taking into account
 * @param enable_gaussian gaussian
 * @param sigma_factor gaussian sigma factor
 * @param distance_power power
 * @param signal_threshold_factor the threshold relative to the maximum signal for  the group, below which signals are
 *  blurred
 * @param threshold_includes_glare when set, the signal threshold is modified by the predicted glare on the zone
 * @param glare_ratio lens glare ratio input based on the edge-spread-function with 50% of the field of
 *  view (FoV) covered
 * @param leak_shift_range_grouping leaky integrator control for the grouping by range
 */
typedef struct sharpener_params_t {
    float invalid_distance;
    float min_range_threshold_mm;
    bool scale_range_threshd_by_range;
    float range_threshold_factor;
    bool enable_max_range_threshold;
    float max_range_threshold_mm_6_step;
    float max_range_threshold_mm_7_step;
    bool enable_distance;
    bool enable_gaussian;
    float channel_ratio;
    float sigma_factor;
    float distance_power;
    float signal_threshold_factor;
    bool threshold_includes_glare;
    float glare_ratio;
    int leak_shift_range_grouping;
    int nb_lines_overlap;
} sharpener_params_t;

/**
 * @brief compute sharpener filter map
 *
 * Based on Python algo R_1.3.0
 *
 * @param depth input depth
 * @param signal input signal
 *
 * @param sharp_valid output sharpener valid flag buffer
 * @param sharp_score output sharpener score buffer
 *
 * @param params sharpener constant parameters
 * @param width image width
 * @param height image height
 * @param step_number number of dToF capture steps
 */
int vl53l9_algo_sharpener(const float* depth, const float* signal,

    bool* sharp_valid, float* sharp_score,

    sharpener_params_t params, unsigned width, unsigned height, unsigned step_number);

#ifdef __cplusplus
}
#endif
