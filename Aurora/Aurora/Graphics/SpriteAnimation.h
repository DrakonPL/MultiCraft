#ifndef SPRITEANIMATION_H
#define SPRITEANIMATION_H

#include <Aurora/Graphics/Sprite.h>
#include <Aurora/Math/Vector2.h>
#include <string>
#include <vector>

namespace Aurora
{
	namespace Graphics
	{
		class RenderManager;

		class SpriteAnimation
		{
		private:

			std::string _imageName;

			int _currentFrame;
			int _framesCount;

			float _frameTime;
			float _animationTime;
			float _timeCounter;

			Math::Vector2 _position;
			Math::Vector2 _scale;

			std::vector<Sprite*> _spriteFrames;

		public:

			SpriteAnimation(std::string imageName);

			void SetPosition(float x,float y);
			void SetScale(float x,float y);
			void SetAnimationTime(float animationTime);

			void AddFrame(int x,int y,int width,int height);
			void Update(float dt);

			friend class RenderManager;
		};
	}
}

#endif
