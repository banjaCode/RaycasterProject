#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <iostream>
#include <stdio.h>
#include <math.h>
#define PI  3.1415926535
#define PI2 PI/2
#define PI3 (3*PI/2)

// Override base class with your custom functionality
class Example : public olc::PixelGameEngine
{
	float px, py, pdx, pdy, pa = PI2;

	void Drawplayer()
	{
		FillRect(px, py, 16, 16, olc::YELLOW);
		DrawLine(px + 8, py + 8, px + 8 + pdx * 20, py + 8 + pdy * 20, olc::DARK_GREEN);
	}

	const static int mapX = 8, mapY = 8, mapS = 64;
	int map[mapS] =
	{
		 1,1,1,1,1,1,1,1,
		 1,0,1,0,0,0,0,1,
		 1,0,1,0,0,0,0,1,
		 1,0,1,0,0,0,0,1,
		 1,0,0,0,0,0,0,1,
		 1,0,0,0,0,1,0,1,
		 1,0,0,0,0,0,0,1,
		 1,1,1,1,1,1,1,1,
	};

	void DrawMap2D()
	{
		olc::Pixel color;
		int x, y, xo, yo;
		for (y = 0; y < mapY; y++)
		{
			for (x = 0; x < mapX;x++)
			{
				if (map[y * mapX + x] == 1) { color = olc::WHITE; }
				else { color = olc::BLACK;; }
				xo = x * mapS; yo = y * mapS;
				FillRect(xo + 1, yo + 1, mapS - 1, mapS - 1, color);
			}
		}
	}

	void buttons() {
		if (GetKey(olc::Key::A).bHeld) { px += cos(pa - PI2) * 5; py += sin(pa - PI2) * 5; }
		if (GetKey(olc::Key::D).bHeld) { px += cos(pa + PI2) * 5; py += sin(pa + PI2) * 5; }
		if (GetKey(olc::Key::W).bHeld) { px += pdx; py += pdy; }
		if (GetKey(olc::Key::S).bHeld) { px -= pdx; py -= pdy; }

		if (GetKey(olc::Key::LEFT).bHeld) { pa -= 0.1; if (pa < 0)         pa += 2 * PI;	pdx = cos(pa) * 5; pdy = sin(pa) * 5;}
		if (GetKey(olc::Key::RIGHT).bHeld) { pa += 0.1; if (pa > 2 * PI)   pa -= 2 * PI;	pdx = cos(pa) * 5; pdy = sin(pa) * 5;}
	}

	void backgroundColor(olc::Pixel color) {
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), color);
	}

	void DrawRays2D()
	{
		int r, mx, my, mp, dof; float  rx, ry, ra, xo, yo;
		ra = pa;
		
		//-- check horizontal lines--
		dof = 0;
		float aTan = -1 / tan(ra);
		if (ra > PI) //looking up
		{
			ry = (((int)py >> 6) << 6) - 0.0001; 
			rx = (py - ry) * aTan + px; yo = -64; 
			xo = -yo * aTan; 
		} 

		else if (ra < PI) //looking down
		{
			ry = (((int)py >> 6) << 6) + 64;
			rx = (py - ry) * aTan + px; yo = 64; 
			xo = -yo * aTan; 
		}  

		else if(ra == 0 || ra == PI) //looking straight left or right
		{
			rx = px; ry = py; 
			dof = 8; 
		} 

		while (dof < 8)
		{
				mx = (int)(rx) >> 6; my = (int)(ry) >> 6; mp = my * mapX + mx;
				if (mp <= (mapX * mapY) && map[mp] == 1) { break; }
				else { rx += xo; ry += yo; dof += 1; }
		} 
		if (dof < 8) {

		}
		DrawLine(px + 8, py + 8, rx, ry, olc::DARK_RED);
		//-- check horizontal lines--
		dof = 0;
		float nTan = -tan(ra);

		if (ra > PI2 && ra < PI3) //looking left
		{ 
			rx = (((int)px >> 6) << 6) - 0.0001; 
			ry = (px - rx) * nTan + py; 
			xo = -64; yo = -xo * nTan; 
		} 

		else if (ra < PI2 || ra > PI3) //looking right
		{
			rx = (((int)px >> 6) << 6) + 64;     
			ry = (px - rx) * nTan + py; 
			xo = 64; yo = -xo * nTan; 
		} 

		else if (ra == 0 || ra == PI) //looking straight up or down
		{
			rx = px; ry = py; 
			dof = 8; 
		} 

		while (dof < 8)
		{
			mx = (int)(rx) >> 6; my = (int)(ry) >> 6; mp = my * mapX + mx;
			if (mp > 0 && mp <= (mapX * mapY) && map[mp] == 1) { dof = 8; }
			else { rx += xo; ry += yo; dof += 1; }
		}



		  DrawLine(px + 8, py + 8, rx, ry, olc::DARK_RED);
		
	}
	/*
	void DrawLinePro(int x, int y, int x2, int y2, int width, olc::Pixel color) 
	{
		int dy1;
		int dx1 = 1;
		int xt = 1;
		int k1;
		if (!(x == x2))
		{
			k1 = (y2 - y) / (x2 - x);
		}
		else (k1 = 0);
		if (k1 == 0 && !(x == x2)) { k1 = 1; xt = 0; }

		for (int i = 0; i < width; i++) 
		{
			dy1 = k1 * i;
			dx1 = xt * i;
			DrawLine(x + dx1, y - dy1, x2 + dx1, y2 - dy1, color);
		}
	}
	*/

public:
	Example()
	{
		// Name your application
		sAppName = "Example";
	}
public:
	bool OnUserCreate() override
	{
		backgroundColor(olc::DARK_GREY);
		px = 300; py = 300;
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		backgroundColor(olc::DARK_GREY);
		DrawMap2D();
		Drawplayer();
		DrawRays2D();
		buttons();
		DrawLinePro(200, 200, 400, 400, 20, olc::BLUE);
		return true;
	}
};
int main()
{
	Example demo;
	if (demo.Construct(1024, 512, 1, 1))
		demo.Start();
	return 0;
}
