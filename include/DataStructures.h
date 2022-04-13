#pragma once
// Enums
enum LightLevel {DARK, MEDIUM_DARK, MEDIUM, MEDIUM_BRIGHT, BRIGHT};
typedef enum LightLevel LightLevel;
enum WaterLevel {DRY, MEDIUM_DRY, MEDIUM, MEDIUM_WET, WET};
typedef enum WaterLevel WaterLevel;

// Preferences - returned from wifi loop, sent to hardware loop, determines actions of lights and pump
struct Preferences {
    LightLevel lightLevel;
    WaterLevel waterLevel;
};
typedef struct Preferences Preferences;

// Measurements - returned from hardware loop, sent to wifi loop
struct Measurements {
    float light;
    uint16_t water;
    bool breakBeam;
};
typedef struct Measurements Measurements;