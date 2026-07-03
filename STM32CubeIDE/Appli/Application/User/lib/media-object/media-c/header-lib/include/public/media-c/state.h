/*******************************************************************************
 *
 * Copyright (c) 2025 STMicroelectronics - All Rights Reserved
 *
 * License terms: STMicroelectronics Proprietary in accordance with licensing
 * terms at SLA0106
 *
 * STMicroelectronics confidential
 * Reproduction and Communication of this document is strictly prohibited unless
 * specifically authorized in writing by STMicroelectronics.
 *
 *******************************************************************************
 */

#ifndef _MEDIA_C_STATE_H_
#define _MEDIA_C_STATE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { MEDIA_STATE_NULL, MEDIA_STATE_INITIALIZED, MEDIA_STATE_PREPARED, MEDIA_STATE_STREAMING } media_state_t;

typedef enum {
    MEDIA_STATE_TRANSITION_NONE,  // No transition set yet
    MEDIA_STATE_TRANSITION_NULL_TO_INITIALIZED,
    MEDIA_STATE_TRANSITION_INITIALIZED_TO_PREPARED,
    MEDIA_STATE_TRANSITION_PREPARED_TO_STREAMING,
    MEDIA_STATE_TRANSITION_STREAMING_TO_PREPARED,
    MEDIA_STATE_TRANSITION_PREPARED_TO_INITIALIZED,
    MEDIA_STATE_TRANSITION_INITIALIZED_TO_NULL,
} media_state_transition_t;

#ifdef __cplusplus
}
#endif

#endif  // _MEDIA_C_STATE_H_