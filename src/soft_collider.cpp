#include "soft_collider.h"
#include <random>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/circle_shape2d.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

void SoftCollider::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("get_push_strength"), &SoftCollider::get_push_strength);
    ClassDB::bind_method(D_METHOD("set_push_strength"), &SoftCollider::set_push_strength);
    ClassDB::bind_method(D_METHOD("get_soft_collisions"), &SoftCollider::get_soft_collisions);
    ClassDB::bind_method(D_METHOD("get_push_vector"), &SoftCollider::get_push_vector);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "push_strength"), "set_push_strength", "get_push_strength");
}

SoftCollider::SoftCollider()
{
    push_strength = 10.0;
}

SoftCollider::~SoftCollider()
{
}

void SoftCollider::set_push_strength(double value)
{
    push_strength = value;
}

double SoftCollider::get_push_strength() const
{
    return push_strength;
}

TypedArray<SoftCollider> SoftCollider::get_soft_collisions() const
{
    TypedArray<Area2D> areas = get_overlapping_areas();
    TypedArray<SoftCollider> soft_colliders;

    for (int i = 0; i < areas.size(); i++)
    {
        SoftCollider *soft_collider = Object::cast_to<SoftCollider>(areas[i]);
        if (soft_collider)
        {
            soft_colliders.push_back(soft_collider);
        }
    }

    return soft_colliders;
}

Vector2 SoftCollider::get_push_vector() const
{
    Vector2 push_vector = Vector2(0, 0);
    TypedArray<SoftCollider> colliders = get_soft_collisions();

    for (int i = 0; i < colliders.size(); i++)
    {
        SoftCollider *collider = Object::cast_to<SoftCollider>(colliders[i]);
        if (collider)
        {
            Vector2 difference = get_global_position() - collider->get_global_position();
            float distance = difference.length();

            if (distance == 0)
            {
                // Use C++'s random number generation
                std::random_device rd;                           // Seed
                std::mt19937 gen(rd());                          // Mersenne Twister generator
                std::uniform_real_distribution<> dis(-0.5, 0.5); // Uniform distribution between -0.5 and 0.5

                difference = Vector2(dis(gen), dis(gen)).normalized();
                distance = 0.1; // Avoid division by zero
            }
            else
            {
                difference = difference.normalized();
            }

            float strength_modifier = 1.0 / (distance + 1.0);
            push_vector += difference * push_strength * strength_modifier;
        }
    }

    return push_vector;
}

PackedStringArray SoftCollider::_get_configuration_warnings() const
{
    PackedStringArray warnings = PackedStringArray();
    CollisionShape2D *shape_node = nullptr;

    for (int i = 0; i < get_child_count(); i++)
    {
        Node *child = get_child(i);
        shape_node = Object::cast_to<CollisionShape2D>(child);
        if (shape_node)
        {
            break;
        }
    }

    if (!shape_node)
    {
        warnings.push_back("SoftCollider requires a CollisionShape2D child node.");
        return warnings;
    }
    Ref<Shape2D> shape = shape_node->get_shape();
    if (!shape.is_valid())
    {
        warnings.push_back("SoftCollider requires a collision shape.");
        return warnings;
    }
    if (!Object::cast_to<CircleShape2D>(*shape))
    {
        warnings.push_back("SoftCollider requires a CircleShape2D. Please change the collision shape.");
        return warnings;
    }

    return warnings;
}

void SoftCollider::_process(double delta)
{
    Vector2 push_vector = get_push_vector();
    Vector2 current_position = get_global_position();
    Vector2 new_position = current_position + push_vector * delta;

    set_global_position(new_position);
}