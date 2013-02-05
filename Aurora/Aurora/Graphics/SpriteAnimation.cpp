#include <Aurora/Graphics/SpriteAnimation.h>

namespace Aurora
{
	namespace Graphics
	{
		SpriteAnimation::SpriteAnimation(std::string imageName)
		{
			_imageName = imageName;

			_currentFrame = 0;
			_framesCount = 0;

			_frameTime = 0.0f;
			_timeCounter = 0.0f;
			_animationTime = 0.0f;
		}

		void SpriteAnimation::SetPosition(float x,float y)
		{
			_position.set(x,y);
		}

		void SpriteAnimation::SetScale(float x,float y)
		{
			_scale.set(x,y);

			for (unsigned int i = 0; i < _spriteFrames.size();i++)
			{
				_spriteFrames[i]->Scale(x,y);
			}
		}

		void SpriteAnimation::SetAnimationTime(float animationTime)
		{
			_animationTime = animationTime;

			if(_framesCount > 0)
			{
				_frameTime = _animationTime / (float)_framesCount;
			}
		}

		void SpriteAnimation::AddFrame(int x,int y,int width,int height)
		{
			Sprite* newSprite = new Sprite(_imageName.c_str(),x,y,width,height);
			_spriteFrames.push_back(newSprite);

			_framesCount++;

			//frame count
			_frameTime = _animationTime / (float)_framesCount;
		}

		void SpriteAnimation::Update(float dt)
		{
			_timeCounter += dt;

			if (_timeCounter > _frameTime)
			{
				_timeCounter = 0;
				_currentFrame++;

				if (_currentFrame >= _framesCount)
				{
					_currentFrame = 0;
				}
			}
		}
	}
}