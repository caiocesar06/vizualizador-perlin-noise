#ifndef PERLIN_WRAPPER_HPP
#define PERLIN_WRAPPER_HPP

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/core/class_db.hpp>
#include "perlin_noise.hpp"

namespace godot {

    class PerlinNoise : public RefCounted {
        GDCLASS(PerlinNoise, RefCounted);

        public:
            enum FadeType {
                LINEAR = 0,     // Corresponde a f(t) = t
                CUBIC = 1,      // Corresponde a f(t) = 3t² - 2t³
                QUINTIC = 2     // Corresponde a f(t) = 6t⁵ - 15t⁴ + 10t³
            };

        private:
            PerlinNoiseCore core_math;
            int64_t current_seed;

            int64_t octaves;
            double persistence;
            double lacunarity;

            FadeType fade_mode;

        protected:
            static void _bind_methods();

        public:
            PerlinNoise();
            ~PerlinNoise();

            void set_seed(int64_t p_seed);
            int64_t get_seed() const;

            void set_octaves(int64_t p_octaves);
            int64_t get_octaves() const;

            void set_persistence(double p_persistence);
            double get_persistence() const;

            void set_lacunarity(double p_lacunarity);
            double get_lacunarity() const;

            void set_fade_mode(FadeType p_mode);
            FadeType get_fade_mode() const;

            double sample(double x, double y) const;

            PackedByteArray gerar_fbm_buffer(int64_t width, int64_t height, double scale, double offsetX, double offsetY, double degrees);
    };
}

VARIANT_ENUM_CAST(godot::PerlinNoise::FadeType);

#endif
