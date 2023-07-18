namespace Oak
{
    public class Input
    {
        public static bool IsKeyDown(KeyCode keycode)
        {
            return InternalCalls.input_IsKeyDown(keycode);
        }
    }
}
