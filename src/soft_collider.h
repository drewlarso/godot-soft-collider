#ifndef SOFT_COLLIDER_H
#define SOFT_COLLIDER_H

#include <godot_cpp/classes/area2d.hpp>

namespace godot
{

    class SoftCollider : public Area2D
    {
        GDCLASS(SoftCollider, Area2D)

    private:
        double push_strength;

    protected:
        static void _bind_methods();

    public:
        SoftCollider();
        ~SoftCollider();

        void set_push_strength(double value);
        double get_push_strength() const;
        Vector2 get_push_vector() const;
        TypedArray<SoftCollider> get_soft_collisions() const;

        PackedStringArray _get_configuration_warnings() const override;
        void _process(double delta) override;
    };

}

#endif