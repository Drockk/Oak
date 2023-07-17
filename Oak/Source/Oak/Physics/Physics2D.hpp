#pragma once

#include "Oak/Scene/Components.hpp"

#include "box2d/b2_body.h"

namespace oak::utils
{
    inline b2BodyType rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType t_bodyType)
    {
        switch (t_bodyType)
        {
        case Rigidbody2DComponent::BodyType::Static:
            return b2_staticBody;
        case Rigidbody2DComponent::BodyType::Dynamic:
            return b2_dynamicBody;
        case Rigidbody2DComponent::BodyType::Kinematic:
            return b2_kinematicBody;
        }

        OAK_CORE_ASSERT(false, "Unknown body type");
        return b2_staticBody;
    }

    inline Rigidbody2DComponent::BodyType rigidbody2DTypeFromBox2DBody(b2BodyType t_bodyType)
    {
        switch (t_bodyType)
        {
        case b2_staticBody:
            return Rigidbody2DComponent::BodyType::Static;
        case b2_dynamicBody:
            return Rigidbody2DComponent::BodyType::Dynamic;
        case b2_kinematicBody:
            return Rigidbody2DComponent::BodyType::Kinematic;
        }

        OAK_CORE_ASSERT(false, "Unknown body type");
        return Rigidbody2DComponent::BodyType::Static;
    }
}
