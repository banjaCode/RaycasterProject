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

class Example : public olc::PixelGameEngine
{
	// player X, player Y, player delta X, player delta y, player angle
	float px, py, pdx, pdy, pWidth, pa = PI;

	// draws player on 2d map
	void Drawplayer()
	{
		FillRect(px - pWidth / 2, py - pWidth / 2, pWidth, pWidth, olc::YELLOW);
		DrawLinePro(px, py, px + pdx * 10, py + pdy * 10, 5, olc::YELLOW);
	}

	// map layout
	const static int mapX = 8, mapY = 8, mapS = 64;
	int map[mapS] =
	{
		 1,1,1,1,1,1,1,1,
		 1,0,1,0,0,0,0,1,
		 1,0,1,0,0,1,0,1,
		 1,0,1,0,0,0,0,1,
		 1,0,0,0,0,0,0,0,
		 1,0,1,1,0,1,0,1,
		 1,0,0,0,0,0,0,1,
		 1,1,1,1,1,1,1,1,
	};

	//draws 2d map
	void DrawMap2D()
	{
		olc::Pixel color;
		//x offset, y offset
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
		float tc = GetElapsedTime();
		float mv = tc * 100;    //movement speed
		float rv = tc * 100;    //rotational speed
		// vertical and horisontal movement
		if (GetKey(olc::Key::A).bHeld) { px += cos(pa - PI2) * mv; py += sin(pa - PI2) * mv; }
		if (GetKey(olc::Key::D).bHeld) { px += cos(pa + PI2) * mv; py += sin(pa + PI2) * mv; }
		if (GetKey(olc::Key::W).bHeld) { px += pdx; py += pdy; }
		if (GetKey(olc::Key::S).bHeld) { px -= pdx; py -= pdy; }
		// rotational movement
		if (GetKey(olc::Key::LEFT).bHeld) { pa -= 0.05 * rv; if (pa < 0)         pa += 2 * PI; }
		if (GetKey(olc::Key::RIGHT).bHeld) { pa += 0.05 * rv; if (pa > 2 * PI)   pa -= 2 * PI; }
		pdx = cos(pa) * mv; pdy = sin(pa) * mv; // sets pdx and pdy depending on player angle
	}

	void DrawRays2D()
	{
		// dof = depth off field, changes how far the ray goes
		int r, mx, my, mp, dof; float  rx, ry, ra, xo, yo, disT;
		ra = pa - DR * 30; if (ra < 0) { ra += 2 * PI; } if (ra > 2 * PI) { ra -= 2 * PI; }
		for (r = 0; r < 1024; r++)
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
				mx = (int)(rx) >> 6; my = (int)(ry) >> 6; mp = my * mapX + mx; // bestämmer en rays position i v�rt 8 * 8 rutn�t
				if (mp > 0 && mp <= (mapX * mapY) && map[mp] == 1) { hx = rx; hy = ry; disH = Dist(px, py, hx, hy, ra); break; }  // Om platsen �r en etta sparar rx och ry och l�ngd p� ray
				else { rx += xo; ry += yo; dof += 1; } // om platsen �r noll, skicka vidare rayen till n�sta position i rutnätet
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
			// set ray distans depending on which is longer
			if (disV < disH) { rx = vx; ry = vy; disT = disV; }
			else if (disH < disV) { rx = hx; ry = hy; disT = disH; }

			DrawLine(px, py, rx, ry, olc::DARK_RED);

			//-- Draw 3D walls --
			float ca = pa - ra; if (ca < 0) { ca += 2 * PI; } if (ca > 2 * PI) { ca -= 2 * PI; } disT = disT * cos(ca);  // best�mmer distans till v�gg
			float lineH = (mapS * 320) / disT;  //Line Height
			float lineO = 160 - lineH / 2;                                        //Line Offset
			float alphaV = 255 * 64 / disT; if (disT < 64) { alphaV = 255; FillRect(r * 1 + 512, lineO, 1, lineH, olc::Pixel(colorV[0], colorV[1], colorV[2], alphaV - kontrast));
			//PaintTextures(r, lineO, lineH, alphaV);

			ra += DR / (256 / 15); if (ra < 0) { ra += 2 * PI; } if (ra > 2 * PI) { ra -= 2 * PI; }
		}
	}

	void PaintTextures(int r, float lineO, float lineH,int alphaV) {
		int const size = (8);
		int texture[size*size] =
		{
			 1,1,1,1,1,1,1,1,
			 1,0,0,0,0,0,0,1,
			 1,0,0,0,0,1,0,1,
			 1,0,0,0,0,0,0,1,
			 1,0,0,0,0,0,0,1,
			 1,0,0,0,0,0,0,1,
			 1,0,0,0,0,0,0,1,
			 1,1,1,1,1,1,1,1,
		};
		int pixelHight = lineH / size;
		for (int i = 0; i <= size; i++) {
			int coulor = 233;
			if (i > 4) {
				coulor = 100;
			}
			FillRect(r * 1 + 512, lineO+(pixelHight*i), 1, pixelHight, olc::Pixel(coulor, 22, 100, alphaV));
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
		FillRect(0, 0, 512, 512 ,olc::Pixel (255,255,255,100));
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
	if (demo.Construct(1536, 512, 1, 1))
		demo.Start();
	return 0;
}
