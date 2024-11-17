#pragma once
constexpr float MIN_X = -10.0f;
constexpr float MAX_X = 30.0f;
constexpr float MIN_Z = -30.0f;
constexpr float MAX_Z = 30.0f;

constexpr unsigned OVERFLOW_TEST = 0xF100D5E7;
constexpr unsigned UNDERFLOW_TEST = 0x5E7F100D;

//Size in Bytes of Memory Pool
constexpr size_t POOL_SIZE = 128 * 1000 * 1000;

// gravity - change it and see what happens (usually negative!)
constexpr float GRAVITY = -19.81f;