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
	struct Vector2D
	{
		float x;
		float y;
	};
	struct Player_Values
	{
		float x, y;														//spelarens position
		float previous_x, previous_y;									//spelarens position förra framen
		float delta_x, delta_y;											//spelarens hastighet
		float width;													//spelare bred för kolision 
		float moved_Distance = 0;										// x value of sin in walking animation
		float xSensitivity = 1, ySensitivity = 500;						// sensitivity of mouse
		float movement_speed;											// movment speed
		bool walk_animation = false;									//desides if the walking animation is on
		float walk_animation_speed, walk_animation_waveLength;			//  1: speed of sin wave in walking animation  2:amplituden på sin vågen
		float angle;													// angle of the caracter
	};
	struct Wall_Values 
	{
		float offset;
	};

	// player X, player Y, player delta X, player delta y, player angle
	
	Vector2D pMouseCoordinates;
	Player_Values player;
	Wall_Values wall;
		float lineYV = 0;
	// [ Draw player on 2d map ]
	void Drawplayer()
	{
		FillRect(player.x - player.width / 2, player.y - player.width / 2, player.width, player.width, olc::YELLOW);
	}

	// [ Map layout ]
	const static int mapWidth = 8, mapHeight = 8, mapS = 64;
	int map[mapS] =
	{
		 1,1,1,1,3,1,1,1,
		 2,0,1,0,0,0,0,2,
		 1,0,1,0,0,1,0,1,
		 1,0,1,3,0,2,0,3,
		 1,0,0,0,0,0,0,3,
		 3,0,0,1,0,1,0,1,
		 1,0,0,1,0,0,0,2,
		 1,1,1,2,1,2,1,1,
	};

	// [ Draws 2D map ]
	void DrawMap2D()
	{
		olc::Pixel color;
		//x offset, y offset
		int x, y, xo, yo;
		for (y = 0; y < mapHeight; y++)
		{
			for (x = 0; x < mapWidth;x++)
			{
				if (map[y * mapWidth + x] > 0) { color = olc::WHITE; }
				else { color = olc::BLACK;; }
				xo = x * mapS; yo = y * mapS;
				FillRect(xo + 3, yo + 3, mapS - 3, mapS - 3, color);
			}
		}
	}

	// [ Button controls ]
	void buttons() {
		// Time Correctiion
		float tc = GetElapsedTime();
		player.previous_x = player.x;
		player.previous_y = player.y;

		// -- VERTICAL AND HORIZONTAL MOVEMENT --
		player.movement_speed = 100;

	    // sets pdx and pdy depending on player angle
		player.delta_x = cos(player.angle) * player.movement_speed * tc;
		player.delta_y = sin(player.angle) * player.movement_speed * tc; 

		if (GetKey(olc::Key::A).bHeld) { player.x += cos(player.angle - PI2) * player.movement_speed * tc; player.y += sin(player.angle - PI2) * player.movement_speed * tc; player.walk_animation = true; }
		if (GetKey(olc::Key::D).bHeld) { player.x += cos(player.angle + PI2) * player.movement_speed * tc; player.y += sin(player.angle + PI2) * player.movement_speed * tc; player.walk_animation = true; }
		if (GetKey(olc::Key::W).bHeld) { player.x += player.delta_x; player.y += player.delta_y; player.walk_animation = true; }
		if (GetKey(olc::Key::S).bHeld) { player.x -= player.delta_x; player.y -= player.delta_y; player.walk_animation = true; }

		// Animation for walking
		if (player.walk_animation == true) {
			player.walk_animation_speed = 1;
			player.walk_animation_waveLength = 4;
			lineYV = (sin(DR * player.moved_Distance - PI2) + 1) / player.walk_animation_waveLength;
			player.moved_Distance += player.walk_animation_speed * 1000 * tc;
			player.walk_animation = false;
		}

		// -- ROTATIONAL MOVEMENT --
		Vector2D rotational_Movement_Speed;
		rotational_Movement_Speed.x = abs(pMouseCoordinates.x - GetMouseX()) * tc * player.xSensitivity;
		rotational_Movement_Speed.y = abs(pMouseCoordinates.y - GetMouseY()) * tc * player.ySensitivity;

		if (( pMouseCoordinates.x > GetMouseX() && GetMouseX() > 512 && GetMouseX() < ScreenWidth() ) || GetKey(olc::Key::LEFT).bHeld)
		{ 
			player.angle -= rotational_Movement_Speed.x; if (player.angle < DR)         player.angle += 2 * PI;
		} 
		if (pMouseCoordinates.x < GetMouseX() && GetMouseX() > 512 && GetMouseX() < ScreenWidth() || GetKey(olc::Key::RIGHT).bHeld)
		{ 
			player.angle += rotational_Movement_Speed.x; if (player.angle > 359 * DR)   player.angle -= 2 * PI;
		}

		if ((pMouseCoordinates.y > GetMouseY() && GetMouseY() < ScreenHeight() && GetMouseY() > 0) || GetKey(olc::Key::DOWN).bHeld) { 
			wall.offset += rotational_Movement_Speed.y; if (wall.offset > 160 + 500) { wall.offset = 160 + 500; }
		}
		if ((pMouseCoordinates.y < GetMouseY() && GetMouseY() < ScreenHeight() && GetMouseY() > 0) || GetKey(olc::Key::UP).bHeld) {
			wall.offset -= rotational_Movement_Speed.y; if (wall.offset < 160 - 500) { wall.offset = 160 - 500; }
		}

		// saves mouse coordinates
		pMouseCoordinates = { (float)GetMouseX(), (float)GetMouseY() };

		// -- SET WORLD LIMITS -- 
		float wallLimit = player.width / 2;
		int mapPosX = (int)player.previous_x >> 6, mapPosY = (int)player.previous_y >> 6, mapPos = mapPosY * 8 + mapPosX;
		float leftLength = abs(player.x - mapPosX * 64), rigthLength = abs(player.x - (mapPosX + 1) * 64);
		float upLength = abs(player.y - mapPosY * 64), downLength = abs(player.y - (mapPosY + 1) * 64);

		if (leftLength < wallLimit && map[mapPos - 1] > 0) { player.x = player.previous_x; }
		if (rigthLength < wallLimit && map[mapPos + 1] > 0) { player.x = player.previous_x; }
		if (upLength < wallLimit && map[mapPos - mapWidth] > 0) { player.y = player.previous_y; }
		if (downLength < wallLimit && map[mapPos + mapWidth] > 0) { player.y = player.previous_y; }
	}

	// [ cast rays and draw 3D world ]
	void DrawRays2D()
	{
		// dof = depth off field, changes how far the ray goes
		int r, mx, my, mp, dof, color, color2, color1, kontrast; float  rx, ry, ra, xo, yo, disT;
		int colorV[3];

		ra = player.angle - DR * 30;
		if (ra < 0) { ra += 2 * PI; }
		if (ra > 2 * PI) { ra -= 2 * PI; }

		for (r = 0; r < 1024; r++)
		{
			//--  CHECK HORIZONTAL LINES --
			dof = 0;
			float disH = 1000000, hx = player.x, hy = player.y;
			float aTan = -1 / tan(ra);	

			if (ra > PI) //looking up
			{ 
			ry = (((int)player.y >> 6) << 6) - 0.0001;
			rx = (player.y - ry) * aTan + player.x;
			yo = -64; xo = -yo * aTan; 
			}

			else if (ra < PI)	//looking down
			{
				ry = (((int)player.y >> 6) << 6) + 64;
				rx = (player.y - ry) * aTan + player.x;
				yo = 64; xo = -yo * aTan; 
			} 

			else if (ra == 0 || ra == PI) 	//looking straight left or right
			{
				rx = player.x;
				ry = player.y;
				dof = 8; 
			}

			while (dof < 8) 			
			{
				mx = (int)(rx) >> 6; my = (int)(ry) >> 6; mp = my * mapWidth + mx;
				if (mp > 0 && mp <= (mapWidth * mapHeight) && map[mp] > 0) 
				{ 
					hx = rx; hy = ry; 
					disH = Pyth(player.x, player.y, hx, hy); 
					color1 = map[mp];
					break; 
				} 
				else { rx += xo; ry += yo; dof += 1; }
			}

			//-- check vertical lines--
			dof = 0;
			float disV = 1000000, vx = player.x, vy = player.y;
			float nTan = -tan(ra);

			if (ra > PI2 && ra < PI3) // Looking left
			{
				rx = (((int)player.x >> 6) << 6) - 0.0001; 
				ry = (player.x - rx) * nTan + player.y;
				xo = -64; yo = -xo * nTan; 
			}

			else if (ra < PI2 || ra > PI3) // Looking right
			{
				rx = (((int)player.x >> 6) << 6) + 64;
				ry = (player.x - rx) * nTan + player.y;
				xo = 64; yo = -xo * nTan;
			} 

			else if (ra == 0 || ra == PI) //looking straight up or down
			{
				rx = player.x; 
				ry = player.y;
				dof = 8; 
			}

			while (dof < 8)
			{
				mx = (int)(rx) >> 6; my = (int)(ry) >> 6; mp = my * mapWidth + mx;
				if (mp > 0 && mp <= (mapWidth * mapHeight) && map[mp] > 0) 
				{ 
					vx = rx; vy = ry;
					disV = Pyth(player.x, player.y, vx, vy);
					color2 = map[mp]; 
					break; 
				}
				else { rx += xo; ry += yo; dof += 1; }
			}
		
			if (disV < disH)  // set ray distans depending on which is longer
			{
				rx = vx; ry = vy; disT = disV; kontrast = 10; color = color2;
			}
			else if (disH < disV) { rx = hx; ry = hy; disT = disH; kontrast = 0; color = color1;}

			if (color == 1) { colorV[0] = 255; colorV[1] = 22; colorV[2] = 100; }
			else if (color == 2) { color = color2; colorV[0] = 100; colorV[1] = 255; colorV[2] = 160; }
			else { colorV[0] = 100; colorV[1] = 255; colorV[2] = 230; }

			DrawLine(player.x, player.y, rx, ry, olc::DARK_RED);

			//-- DRAW 3D WORLD --
			float ca = player.angle - ra; if (ca < 0) { ca += 2 * PI; } if (ca > 2 * PI) { ca -= 2 * PI; } disT = disT * cos(ca);  // best�mmer distans till v�gg
			float lineH = (mapS * 320) / disT;                                                                                  //Line Height
			float lineO = wall.offset - lineH / (2 + lineYV);                                                                        //Line Offset
			float alphaV = 255 * 64 / disT; if (disT < 64) { alphaV = 255; }

			FillRect(r * 1 + 512, lineO, 1, lineH, olc::Pixel(colorV[0], colorV[1], colorV[2], alphaV - kontrast));
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


	float Pyth(float ax, float ay, float bx, float by)
	{
		return sqrt(pow((bx - ax), 2) + pow((by - ay), 2));
	}


	void DrawLinePro(double lineX, double lineY, double lineX2, double lineY2, int width, olc::Pixel color)
	{
		double dy, dx;
		double lineK = (lineY2 - lineY) / (lineX2 - lineX);
		double lineA = atan(lineK);
		dy = sin(PI2 - lineA); dx = -cos(PI2 - lineA);

		int xLimit = (width / 2);
		if (xLimit * 2 < width) { DrawLine(lineX,lineY, lineX2, lineY2, color); }

		for (int i = 0; i < xLimit; i++)
		{
			DrawLine(lineX + (dx * i), lineY + (dy * i), lineX2 + (dx * i), lineY2 + (dy * i), color);
			DrawLine(lineX - (dx * i), lineY - (dy * i), lineX2 - (dx * i), lineY2 - (dy * i), color);
		}
	}


public:
	Example()
	{
		// Name your application
		sAppName = "Raycaster";
	}
public:
	bool OnUserCreate() override
	{
		backgroundColor(olc::DARK_GREY);
		player.x = 300; player.y = 300; player.width = 16;
		player.angle = PI;
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		backgroundColor(olc::DARK_BLUE);
		FillRect(512, (ScreenHeight() + 2* wall.offset - 480) / 2, ScreenWidth(), ScreenHeight()*4, olc::DARK_GREEN);
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
