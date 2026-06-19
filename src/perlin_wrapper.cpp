#include "perlin_wrapper.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <algorithm>
#include <random>

using namespace godot;

void PerlinNoise::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_seed", "seed"), &PerlinNoise::set_seed);
    ClassDB::bind_method(D_METHOD("get_seed"), &PerlinNoise::get_seed);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "seed"), "set_seed", "get_seed");

    ClassDB::bind_method(D_METHOD("set_octaves", "octaves"), &PerlinNoise::set_octaves);
    ClassDB::bind_method(D_METHOD("get_octaves"), &PerlinNoise::get_octaves);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "octaves"), "set_octaves", "get_octaves");

    ClassDB::bind_method(D_METHOD("set_persistence", "persistence"), &PerlinNoise::set_persistence);
    ClassDB::bind_method(D_METHOD("get_persistence"), &PerlinNoise::get_persistence);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "persistence"), "set_persistence", "get_persistence");

    ClassDB::bind_method(D_METHOD("set_lacunarity", "lacunarity"), &PerlinNoise::set_lacunarity);
    ClassDB::bind_method(D_METHOD("get_lacunarity"), &PerlinNoise::get_lacunarity);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "lacunarity"), "set_lacunarity", "get_lacunarity");

    BIND_ENUM_CONSTANT(LINEAR);
    BIND_ENUM_CONSTANT(CUBIC);
    BIND_ENUM_CONSTANT(QUINTIC);

    ClassDB::bind_method(D_METHOD("set_fade_mode", "fade_mode"), &PerlinNoise::set_fade_mode);
    ClassDB::bind_method(D_METHOD("get_fade_mode"), &PerlinNoise::get_fade_mode);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "fade_mode", PROPERTY_HINT_ENUM, "Linear,Cubico,Quintico"), "set_fade_mode", "get_fade_mode");

    ClassDB::bind_method(D_METHOD("gerar_fbm_buffer", "width", "height", "scale", "offsetX", "offsetY", "degrees"), &PerlinNoise::gerar_fbm_buffer);

    ClassDB::bind_method(D_METHOD("sample", "x", "y"), &PerlinNoise::sample);
}

PerlinNoise::PerlinNoise() : core_math(0), octaves(6), persistence(0.5f), lacunarity(2.0f), fade_mode(QUINTIC) {
    std::random_device rd;
    current_seed = rd();
    core_math.set_seed(current_seed);
}

PerlinNoise::~PerlinNoise() {}

void PerlinNoise::set_seed(int64_t p_seed) {
    current_seed = p_seed;
    core_math.set_seed(current_seed);
}
int64_t PerlinNoise::get_seed() const {
    return current_seed;
}

void PerlinNoise::set_octaves(int64_t p_octaves) {
    octaves = std::max(int64_t(1), p_octaves);
}
int64_t PerlinNoise::get_octaves() const {
    return octaves;
}

void PerlinNoise::set_persistence(double p_persistence) {
    persistence = p_persistence;
}
double PerlinNoise::get_persistence() const {
    return persistence;
}

void PerlinNoise::set_lacunarity(double p_lacunarity) {
    lacunarity = p_lacunarity;
}
double PerlinNoise::get_lacunarity() const {
    return lacunarity;
}

void PerlinNoise::set_fade_mode(FadeType p_mode) {
    fade_mode = p_mode;

    if (fade_mode == LINEAR) {
        core_math.fade_mode = FadeMode::NONE;
    }
    else if (fade_mode == CUBIC) {
        core_math.fade_mode = FadeMode::CUBIC;
    }
    else if (fade_mode == QUINTIC) {
        core_math.fade_mode = FadeMode::QUINTIC;
    }
}
PerlinNoise::FadeType PerlinNoise::get_fade_mode() const {
    return fade_mode;
}

double PerlinNoise::sample(double x, double y) const {
    return core_math.fBm(x, y, octaves, persistence, lacunarity);
}

PackedByteArray PerlinNoise::gerar_fbm_buffer(int64_t width, int64_t height, double scale, double offsetX, double offsetY, double degrees) {
    PackedByteArray buffer;
    buffer.resize(width * height * 4);
    uint8_t* raw_ptr = buffer.ptrw();

#pragma omp parallel for
    for (int64_t y = 0; y < height; ++y) {
        for (int64_t x = 0; x < width; ++x) {
            double nx = (x + offsetX) * scale;
            double ny = (y + offsetY) * scale;

            double val = core_math.fBm(nx, ny, static_cast<int>(octaves), static_cast<float>(persistence), static_cast<float>(lacunarity));

            double normalized = (val + 1.0f) / 2.0f;
            double posterized = fast_floor(normalized * degrees) / degrees;
            int64_t c = CLAMP((int64_t)(posterized * 255.0f), 0, 255);

            int64_t idx = (y * width + x) * 4;
            raw_ptr[idx + 0] = (uint8_t)c;
            raw_ptr[idx + 1] = (uint8_t)c;
            raw_ptr[idx + 2] = (uint8_t)c;
            raw_ptr[idx + 3] = 255;
        }
    }
    return buffer;
}
