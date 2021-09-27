#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <iostream>
#include <stdio.h>
#include <math.h>
#define PI  3.14159265358979323846
#define PI2 PI/2
#define PI3 (3*PI/2)
#define DR 0.0174532925

using namespace std;
// Override base class with your custom functionality
class Example : public olc::PixelGameEngine
{
	float px, py, pdx, pdy, pWidth, pa = PI;

	void Drawplayer()
	{
		FillRect(px - pWidth / 2, py - pWidth / 2, pWidth, pWidth, olc::YELLOW);
		DrawLinePro(px, py, px + pdx * 10, py + pdy * 10, 5, olc::YELLOW);
	}

	const static int mapX = 8, mapY = 8, mapS = 64;
	int map[mapS] =
	{
		 1,1,1,1,1,1,1,1,
		 1,0,1,0,0,0,0,1,
		 1,0,1,0,0,1,0,1,
		 1,0,1,0,0,0,0,1,
		 1,0,0,0,0,0,0,1,
		 1,0,1,1,0,1,0,1,
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
		// vertical and horisontal movement
		if (GetKey(olc::Key::A).bHeld) { px += cos(pa - PI2) * 2; py += sin(pa - PI2) * 2; }
		if (GetKey(olc::Key::D).bHeld) { px += cos(pa + PI2) * 2; py += sin(pa + PI2) * 2; }
		if (GetKey(olc::Key::W).bHeld) { px += pdx; py += pdy; }
		if (GetKey(olc::Key::S).bHeld) { px -= pdx; py -= pdy; }
		// rotational movement
		if (GetKey(olc::Key::LEFT).bHeld) { pa -= 0.05; if (pa < 0)         pa += 2 * PI; }
		if (GetKey(olc::Key::RIGHT).bHeld) { pa += 0.05; if (pa > 2 * PI)   pa -= 2 * PI; }
		pdx = cos(pa) * 2; pdy = sin(pa) * 2;
	}

	void DrawRays2D()
	{
		int r, mx, my, mp, dof; float  rx, ry, ra, xo, yo, disT;
		ra = pa - DR * 40; if (ra < 0) { ra += 2 * PI; } if (ra > 2 * PI) { ra -= 2 * PI; }
		for (r = 0; r < 80; r++)
		{
			//-- check horizontal lines--
			dof = 0;
			float disH = 1000000, hx = px, hy = py;
			float aTan = -1 / tan(ra);
			if (ra > PI) { ry = (((int)py >> 6) << 6) - 0.0001; rx = (py - ry) * aTan + px; yo = -64; xo = -yo * aTan; } //looking up
			else if (ra < PI) { ry = (((int)py >> 6) << 6) + 64; rx = (py - ry) * aTan + px; yo = 64; xo = -yo * aTan; } //looking down
			else if (ra == 0 || ra == PI) { rx = px; ry = py; dof = 8; } //looking straight left or right

			while (dof < 8)
			{
				mx = (int)(rx) >> 6; my = (int)(ry) >> 6; mp = my * mapX + mx;
				if (mp > 0 && mp <= (mapX * mapY) && map[mp] == 1) { hx = rx; hy = ry; disH = Dist(px, py, hx, hy, ra); break; }
				else { rx += xo; ry += yo; dof += 1; }
			}

			//-- check vertical lines--
			dof = 0;
			float disV = 1000000, vx = px, vy = py;
			float nTan = -tan(ra);

			if (ra > PI2 && ra < PI3) { rx = (((int)px >> 6) << 6) - 0.0001; ry = (px - rx) * nTan + py; xo = -64; yo = -xo * nTan; } //looking left
			else if (ra < PI2 || ra > PI3) { rx = (((int)px >> 6) << 6) + 64; ry = (px - rx) * nTan + py; xo = 64; yo = -xo * nTan; } //looking right
			else if (ra == 0 || ra == PI) { rx = px; ry = py; dof = 8; } //looking straight up or down

			while (dof < 8)
			{
				mx = (int)(rx) >> 6; my = (int)(ry) >> 6; mp = my * mapX + mx;
				if (mp > 0 && mp <= (mapX * mapY) && map[mp] == 1) { vx = rx; vy = ry; disV = Dist(px, py, vx, vy, ra); break; }
				else { rx += xo; ry += yo; dof += 1; }
			}
			
			if (disV < disH) { rx = vx; ry = vy; disT = disV; }
			else if (disH < disV) { rx = hx; ry = hy; disT = disH; }
			DrawLinePro(px, py, rx, ry, 5, olc::DARK_RED);

			//-- Draw 3D walls --
			float ca = pa - ra; if (ca < 0) { ca += 2 * PI; } if (ca > 2 * PI) { ca -= 2 * PI; } disT = disT * cos(ca);
			float lineH = (mapS * 360) / disT; if (lineH > 320) { lineH = 320; }   //Line Height
			float lineO = 160 - lineH / 2;                                         //Line Offset
			FillRect(r*8+530, lineO, 8, lineH + lineO, olc::Pixel(233, 22, 100, rand() % 100));

			ra += DR; if (ra < 0) { ra += 2 * PI; } if (ra > 2 * PI) { ra -= 2 * PI; }
		}
	}


	void backgroundColor(olc::Pixel color) {
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), color);
	}


	float Dist(float ax, float ay, float bx, float by, float ang)
	{
		return sqrt(pow((bx - ax), 2) + pow((by - ay), 2));
	}


	void DrawLinePro(double lineX, double lineY, double lineX2, double lineY2, int width, olc::Pixel color)
	{
		int inX, inY, inX2, inY2, wLineX, wLineY, wLineX2, wLineY2;
		double dy, dx;
		double lineK = (lineY2 - lineY) / (lineX2 - lineX);
		double lineA = atan(lineK);
		dy = sin(PI2 - lineA); dx = -cos(PI2 - lineA);

		for (int i = 0; i < width / 2; i++)
		{
			DrawLine(lineX + (dx * i), lineY + (dy * i), lineX2 + (dx * i), lineY2 + (dy * i), color);
			DrawLine(lineX - (dx * i), lineY - (dy * i), lineX2 - (dx * i), lineY2 - (dy * i), color);
		}
	}


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
		px = 300; py = 300; pWidth = 16;
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		backgroundColor(olc::DARK_BLUE);
		DrawMap2D();
		DrawRays2D();
		Drawplayer();
		buttons();
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
