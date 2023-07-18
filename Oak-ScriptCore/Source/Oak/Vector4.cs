namespace Oak
{
    public struct Vector4
    {
        public float x, y, z, w;

        public static Vector4 Zero => new Vector4(0.0f);

        public Vector4(float scalar)
        {
            x = scalar;
            y = scalar;
            z = scalar;
            w = scalar;
        }

        public Vector4(float t_x, float t_y, float t_z, float t_w)
        {
            x = t_x;
            y = t_y;
            z = t_z;
            w = t_w;
        }

        public Vector4(Vector3 t_xyz, float t_w)
        {
            x = t_xyz.x;
            y = t_xyz.y;
            z = t_xyz.z;
            w = t_w;
        }

        public Vector2 XY
        {
            get => new Vector2(x, y);
            set
            {
                x = value.x;
                y = value.y;
            }
        }

        public Vector3 XYZ
        {
            get => new Vector3(x, y, z);
            set
            {
                x = value.x;
                y = value.y;
                z = value.z;
            }
        }

        public static Vector4 operator +(Vector4 v1, Vector4 v2)
        {
            return new Vector4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
        }

        public static Vector4 operator *(Vector4 vector, float scalar)
        {
            return new Vector4(vector.x * scalar, vector.y * scalar, vector.z * scalar, vector.w * scalar);
        }

    }
}
