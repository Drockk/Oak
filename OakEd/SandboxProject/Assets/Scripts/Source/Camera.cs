using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Oak;

namespace Sandbox
{
	public class Camera : Entity
	{
		public Entity OtherEntity;

		public float DistanceFromPlayer = 5.0f;

		private Entity m_Player;

		void OnCreate()
		{
			m_Player = FindEntityByName("Player");
		}

		void OnUpdate(float ts)
		{
			if (m_Player != null)
				Translation = new Vector3(m_Player.Translation.Xy, DistanceFromPlayer);

			float speed = 1.0f;
			Vector3 velocity = Vector3.Zero;

			if (Input.IsKeyDown(KeyCode.Up))
				velocity.y = 1.0f;
			else if (Input.IsKeyDown(KeyCode.Down))
				velocity.y = -1.0f;

			if (Input.IsKeyDown(KeyCode.Left))
				velocity.x = -1.0f;
			else if (Input.IsKeyDown(KeyCode.Right))
				velocity.x = 1.0f;

			velocity *= speed;

			Vector3 translation = Translation;
			translation += velocity * ts;
			Translation = translation;
		}

	}
}
