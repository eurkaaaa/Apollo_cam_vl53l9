#ifndef _MEDIA_C_ERROR_H_
#define _MEDIA_C_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum media_error_code {
    MEDIA_ERROR_NONE = 0,
    MEDIA_ERROR_INVALID_PARAMETER = -1,
    MEDIA_ERROR_INVALID_STATE = -2,
    MEDIA_ERROR_UNALIGNED_MEMORY = -3,
    MEDIA_ERROR_STATIC_CTRL = -4,
    MEDIA_ERROR_R_ONLY_CTRL = -5,
    MEDIA_ERROR_W_ONLY_CTRL = -6,
    MEDIA_ERROR_PENDING = -7,
    MEDIA_ERROR_NOT_FOUND = -8,
    MEDIA_ERROR_INVALID_STREAM_CAPS = -9,
    MEDIA_ERROR_INVALID_STREAM_NAME = -10,
    MEDIA_ERROR_UNIMPLEMENTED = -11,
    MEDIA_ERROR_UNKNOWN = -12
} media_error_code_t;

typedef struct media_error_t {
    int code;             // Error code
    const char* message;  // Error message
} media_error_t;

#ifdef __cplusplus
}
#endif

#endif  // _MEDIA_C_ERROR_H_