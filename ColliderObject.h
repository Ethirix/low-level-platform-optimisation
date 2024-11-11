#pragma once

#include "Vector3.h"
#include <list>
#include <GL/glut.h>
#include "globals.h"

class ColliderObject
{
public:
    Vector3 Position;
    Vector3 Size;
    Vector3 Velocity;
    Vector3 Colour;

    // if two colliders collide, push them away from each other
    void ResolveCollision(ColliderObject* a, ColliderObject* b) {
        Vector3 normal = { a->Position.X - b->Position.X, a->Position.Y - b->Position.Y, a->Position.Z - b->Position.Z };
        //float length = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);

        // Normalize the normal vector
        normal.Normalise();

        float relativeVelocityX = a->Velocity.X - b->Velocity.X;
        float relativeVelocityY = a->Velocity.Y - b->Velocity.Y;
        float relativeVelocityZ = a->Velocity.Z - b->Velocity.Z;

        // Compute the relative velocity along the normal
        float impulse = relativeVelocityX * normal.X + relativeVelocityY * normal.Y + relativeVelocityZ * normal.Z;

        // Ignore collision if objects are moving away from each other
        if (impulse > 0) {
            return;
        }

        // Compute the collision impulse scalar
        float e = 0.01f; // Coefficient of restitution (0 = inelastic, 1 = elastic)
        float dampening = 0.9f; // Dampening factor (0.9 = 10% energy reduction)
        float j = -(1.0f + e) * impulse * dampening;

        // Apply the impulse to the colliders' velocities
        a->Velocity.X += j * normal.X;
        a->Velocity.Y += j * normal.Y;
        a->Velocity.Z += j * normal.Z;
        b->Velocity.X -= j * normal.X;
        b->Velocity.Y -= j * normal.Y;
        b->Velocity.Z -= j * normal.Z;
    }

    // are two colliders colliding?
    bool CheckCollision(const ColliderObject* a, const ColliderObject* b) {
        return (std::abs(a->Position.X - b->Position.X) * 2 < (a->Size.X + b->Size.X)) &&
            (std::abs(a->Position.Y - b->Position.Y) * 2 < (a->Size.Y + b->Size.Y)) &&
            (std::abs(a->Position.Z - b->Position.Z) * 2 < (a->Size.Z + b->Size.Z));
    }

    // draw the physics object
    void Draw() {
        glPushMatrix();
        glTranslatef(Position.X, Position.Y, Position.Z);
        GLfloat diffuseMaterial[] = { Colour.X, Colour.Y, Colour.Z, 1.0f };
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseMaterial);
        glScalef(Size.X, Size.Y, Size.Z);
        glRotatef(-90, 1, 0, 0);
        DrawMesh();
        //glutSolidTeapot(1);
        //glutSolidCone(1, 1, 10, 10);
        glPopMatrix();
    }

    virtual void DrawMesh() {};

    virtual ~ColliderObject() = default;

    void Update(std::list<ColliderObject*>* colliders, const float& deltaTime)
    {
        const float floorY = 0.0f;
        // Update velocity due to gravity
        Velocity.Y += GRAVITY * deltaTime;

        // Update position based on velocity
        Position.X += Velocity.X * deltaTime;
        Position.Y += Velocity.Y * deltaTime;
        Position.Z += Velocity.Z * deltaTime;

        // Check for collision with the floor
        if (Position.Y - Size.Y / 2.0f < floorY) {
            Position.Y = floorY + Size.Y / 2.0f;
            float dampening = 0.7f;
            Velocity.Y = -Velocity.Y * dampening;
        }

        // Check for collision with the walls
        if (Position.X - Size.X / 2.0f < MIN_X || Position.X + Size.X / 2.0f > MAX_X) {
            Velocity.X = -Velocity.X;
        }
        if (Position.Z - Size.Z / 2.0f < MIN_Z || Position.Z + Size.Z / 2.0f > MAX_Z) {
            Velocity.Z = -Velocity.Z;
        }

        // Check for collisions with other colliders
        for (ColliderObject* other : *colliders) {
            if (this == other) continue;
            if (CheckCollision(this, other)) {
                ResolveCollision(this, other);
                break;
            }
        }
    }
};

