#pragma once
#define MIN_X (-10.0f)
#define MAX_X 30.0f
#define MIN_Z (-30.0f)
#define MAX_Z 30.0f

#define OVERFLOW_TEST 0xF100D5E7
#define UNDERFLOW_TEST 0x5E7F100D

#define MAX_POOL_COUNT 64

// gravity - change it and see what happens (usually negative!)
constexpr float GRAVITY = -19.81f;