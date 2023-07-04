using System;

namespace Oak
{
    public struct Vector2
    {
        public float x; public float y;
        public static Vector2 Zero => new Vector2(0.0f);

        public Vector2(float scalar)
        {
            x = scalar;
            y = scalar;
        }

        public Vector2(float t_x, float t_y)
        {
            x = t_x; y = t_y;
        }

        public static Vector2 operator+(Vector2 v1, Vector2 v2)
        {
            return new Vector2(v1.x + v2.x, v1.y + v2.y);
        }

        public static Vector2 operator*(Vector2 v1, float scalar)
        {
            return new Vector2(v1.x * scalar, v1.y * scalar);
        }

        public float LengthSquared()
        {
            return x * x + y * y;
        }

        public float Length()
        {
            return (float)Math.Sqrt(LengthSquared());
        }
    }
}
