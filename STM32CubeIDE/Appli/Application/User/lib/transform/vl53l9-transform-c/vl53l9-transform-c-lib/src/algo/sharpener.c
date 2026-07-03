#include "algo/sharpener.h"

#include <float.h>
#include <math.h>

static inline float minf(float a, float b) {
    return a < b ? a : b;
}

static inline float maxf(float a, float b) {
    return a > b ? a : b;
}

#define MAX_IMAGE_SIZE   (2268)
#define MAX_GROUP_AMOUNT (41)

typedef enum info_id {
    MAX_SIGNAL_TOP,
    SUM_SIGNAL_TOP,
    X_BAR_TOP,
    Y_BAR_TOP,
    SIG_SCORE_TOP,
    MAX_SIGNAL_BOT,
    SUM_SIGNAL_BOT,
    X_BAR_BOT,
    Y_BAR_BOT,
    SIG_SCORE_BOT,
    INFO_ID_MAX
} info_id;

int vl53l9_algo_sharpener(const float *depth, const float *signal,

                          bool *sharp_valid, float *sharp_score,

                          sharpener_params_t params, unsigned width, unsigned height, unsigned step_number) {
    // if one of the input is missing, return
    // if all outputs are missing, return
    if (!depth || !signal || !(sharp_valid || sharp_score)) {
        return EXIT_FAILURE;
    }

    unsigned group_id[MAX_IMAGE_SIZE];

    float group_infos[MAX_GROUP_AMOUNT][INFO_ID_MAX] = { 0 };

    // grouping pre-processing
    float max_range_threshold_mm =
        step_number == 7 ? params.max_range_threshold_mm_7_step : params.max_range_threshold_mm_6_step;
    const unsigned max_nb_group = params.invalid_distance / max_range_threshold_mm + 1;

    // grouping
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            const unsigned i = y * width + x;

            const unsigned gid = (unsigned)(minf(depth[i], params.invalid_distance) / max_range_threshold_mm);
            group_id[i] = gid;

            if (y < height / 2 + params.nb_lines_overlap) {
                group_infos[gid][MAX_SIGNAL_TOP] = maxf(group_infos[gid][MAX_SIGNAL_TOP], signal[i]);
                group_infos[gid][SUM_SIGNAL_TOP] += signal[i];
                group_infos[gid][X_BAR_TOP] += signal[i] * x;
                group_infos[gid][Y_BAR_TOP] += signal[i] * y;
            } else {
                group_infos[gid][MAX_SIGNAL_TOP] =
                    maxf(group_infos[gid][MAX_SIGNAL_TOP], signal[i] / params.channel_ratio);
                group_infos[gid][SUM_SIGNAL_TOP] += signal[i] / params.channel_ratio;
                group_infos[gid][X_BAR_TOP] += signal[i] * x / params.channel_ratio;
                group_infos[gid][Y_BAR_TOP] += signal[i] * y / params.channel_ratio;
            }

            if (y >= height / 2 - params.nb_lines_overlap) {
                group_infos[gid][MAX_SIGNAL_BOT] = maxf(group_infos[gid][MAX_SIGNAL_BOT], signal[i]);
                group_infos[gid][SUM_SIGNAL_BOT] += signal[i];
                group_infos[gid][X_BAR_BOT] += signal[i] * x;
                group_infos[gid][Y_BAR_BOT] += signal[i] * y;
            } else {
                group_infos[gid][MAX_SIGNAL_BOT] =
                    maxf(group_infos[gid][MAX_SIGNAL_BOT], signal[i] / params.channel_ratio);
                group_infos[gid][SUM_SIGNAL_BOT] += signal[i] / params.channel_ratio;
                group_infos[gid][X_BAR_BOT] += signal[i] * x / params.channel_ratio;
                group_infos[gid][Y_BAR_BOT] += signal[i] * y / params.channel_ratio;
            }
        }
    }

    for (unsigned i = 0; i < max_nb_group; ++i) {
        group_infos[i][X_BAR_TOP] =
            group_infos[i][SUM_SIGNAL_TOP] ? group_infos[i][X_BAR_TOP] / group_infos[i][SUM_SIGNAL_TOP] : 0.0f;
        group_infos[i][Y_BAR_TOP] =
            group_infos[i][SUM_SIGNAL_TOP] ? group_infos[i][Y_BAR_TOP] / group_infos[i][SUM_SIGNAL_TOP] : 0.0f;
        group_infos[i][SIG_SCORE_TOP] =
            group_infos[i][MAX_SIGNAL_TOP] ? group_infos[i][SUM_SIGNAL_TOP] / group_infos[i][MAX_SIGNAL_TOP] : 1.0f;
        group_infos[i][X_BAR_BOT] =
            group_infos[i][SUM_SIGNAL_BOT] ? group_infos[i][X_BAR_BOT] / group_infos[i][SUM_SIGNAL_BOT] : 0.0f;
        group_infos[i][Y_BAR_BOT] =
            group_infos[i][SUM_SIGNAL_BOT] ? group_infos[i][Y_BAR_BOT] / group_infos[i][SUM_SIGNAL_BOT] : 0.0f;
        group_infos[i][SIG_SCORE_BOT] =
            group_infos[i][MAX_SIGNAL_BOT] ? group_infos[i][SUM_SIGNAL_BOT] / group_infos[i][MAX_SIGNAL_BOT] : 1.0f;
    }

    // set status pre-processing
    const float distance_power = params.distance_power / 2.0f;
    const float glare_ratio = params.glare_ratio * params.threshold_includes_glare;

    // set status
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            const unsigned i = y * width + x;

            if (depth[i] >= params.invalid_distance) {
                sharp_score[i] = 1.0f;
            }

            const unsigned gid = group_id[i];
            info_id max_signal_id, sum_signal_id, x_bar_id, y_bar_id, sig_score_id;
            if (y < height / 2) {
                max_signal_id = MAX_SIGNAL_TOP;
                sum_signal_id = SUM_SIGNAL_TOP;
                x_bar_id = X_BAR_TOP;
                y_bar_id = Y_BAR_TOP;
                sig_score_id = SIG_SCORE_TOP;
            } else {
                max_signal_id = MAX_SIGNAL_BOT;
                sum_signal_id = SUM_SIGNAL_BOT;
                x_bar_id = X_BAR_BOT;
                y_bar_id = Y_BAR_BOT;
                sig_score_id = SIG_SCORE_BOT;
            }

            const float sig_score_sq = group_infos[gid][sig_score_id] * group_infos[gid][sig_score_id] *
                                       params.sigma_factor * params.sigma_factor;

            float distance = 1.0f;
            if (params.enable_distance) {
                if (params.enable_gaussian) {
                    const float distance_before_expf =
                        ((powf(x - group_infos[gid][x_bar_id], 2.0f) + powf(y - group_infos[gid][y_bar_id], 2.0f)) /
                         (2.0f * sig_score_sq));
                    distance = distance_before_expf < 88.7228f ? expf(distance_before_expf) : FLT_MAX;
                } else {
                    distance =
                        powf(powf(x - group_infos[gid][x_bar_id], 2.0f) + powf(y - group_infos[gid][y_bar_id], 2.0f),
                             distance_power);
                }

                if (distance == 0.0f) {
                    distance = 1.0f;
                }
            }

            const float signal_threshold = group_infos[gid][max_signal_id] * params.signal_threshold_factor +
                                           (glare_ratio * group_infos[gid][sum_signal_id]);
            const float score = signal_threshold ? signal[i] * (distance / signal_threshold) : 0.0f;
            if (sharp_score) {
                sharp_score[i] = score;
            }
            if (sharp_valid) {
                sharp_valid[i] = (bool)((int)score);
            }
        }
    }

    return EXIT_SUCCESS;
}
