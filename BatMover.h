#pragma once
#include <SFML/Graphics.hpp>
#include <Bat.h>
namespace Entities {
	class BatMover
	{
	
	public:
		BatMover(Entities::position& batPosition, Entities::speed& batSpeed);
		void update();
	private:
		Entities::position& batPosition;
		Entities::speed& batSpeed;
	};
}


