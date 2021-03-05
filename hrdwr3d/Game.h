#pragma once

#include <D3D11.h>
#include <D3DX11.h>
#include <iostream>
#include <string>
#include <xnamath.h>

class Game
{	
public:
	bool Initialise();
	void Update(float dt);
	void Render();
	
};