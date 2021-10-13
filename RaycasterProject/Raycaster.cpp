﻿#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <iostream>
#include <stdio.h>
#include <math.h>
#define PI  3.14159265358979323846
#define PI2 PI/2
#define PI3 PI + PI2
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
		float x, y;						                                //spelarens x och y position
		float angle;                                                    // angle of the caracter
		float previous_x, previous_y;									//spelarens position förra framen
		Vector2D velocity;          									//spelarens hastighet
		float velocity_angle;
		float width;													//spelare bred för kolision 
		float moved_Distance = 0;										// x value of sin in walking animation
		float xSensitivity = 0.5, ySensitivity = 200;						// sensitivity of mouse
		float movement_speed;											// movment speed
		bool walk_animation = false;									// desides if the walking animation is on
		float walk_animation_speed, walk_animation_waveLength;			//  1: speed of sin wave in walking animation  2:amplituden på sin vågen
	};
	struct Wall_Values 
	{
		float angle;
		float offset;
	};
	
	Vector2D pMouseCoordinates;
	Player_Values player;
	Wall_Values wall;

	// [ Draw player on 2d map ]
	void Drawplayer()
	{
		FillCircle(player.x, player.y, player.width/2, olc::YELLOW);
	}

	// [ Map layout ]
	const static int mapWidth = 8, mapHeight = 8, mapS = 64;
	int map[mapS] =
	{
		 1,1,1,1,3,1,1,1,
		 2,0,0,0,0,0,0,2,
		 1,0,0,0,0,0,0,1,
		 1,0,0,0,3,0,0,3,
		 1,0,0,0,3,0,0,3,
		 3,0,0,0,0,0,0,1,
		 1,0,0,0,0,0,0,2,
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
		player.velocity_angle = 0;

		if (GetKey(olc::Key::A).bHeld) { player.velocity_angle = - PI2;    player.walk_animation = true; }
		if (GetKey(olc::Key::D).bHeld) { player.velocity_angle = + PI2;    player.walk_animation = true; }
		if (GetKey(olc::Key::W).bHeld) { player.velocity_angle +=  0;      player.walk_animation = true; }
		if (GetKey(olc::Key::S).bHeld) { player.velocity_angle += PI;      player.walk_animation = true; }

		if (GetKey(olc::Key::W).bHeld && (GetKey(olc::Key::D).bHeld || GetKey(olc::Key::A).bHeld))
		{
			player.velocity_angle = player.velocity_angle / 2;
		}
		if (GetKey(olc::Key::S).bHeld && (GetKey(olc::Key::D).bHeld || GetKey(olc::Key::A).bHeld))
		{
			player.velocity_angle = (player.velocity_angle - PI) / -2 + PI;
		}

		if (GetKey(olc::Key::A).bHeld || GetKey(olc::Key::D).bHeld || GetKey(olc::Key::W).bHeld || GetKey(olc::Key::S).bHeld) 
		{

			player.velocity.x = cos(player.angle + player.velocity_angle) * player.movement_speed * tc;
			player.velocity.y = sin(player.angle + player.velocity_angle) * player.movement_speed * tc;
			player.x += player.velocity.x;
			player.y += player.velocity.y;
		}

		// Animation for walking
		if (player.walk_animation == true) {
			wall.offset = (sin(DR * player.moved_Distance - PI2) + 1) / player.walk_animation_waveLength;
			player.moved_Distance += player.walk_animation_speed * 1000 * tc;
			player.walk_animation = false;
		}
		//------------------------------------------------------------------------------------------------------------------

		// -- ROTATIONAL MOVEMENT --
		Vector2D rotational_Movement_Speed;
		rotational_Movement_Speed.x = abs(pMouseCoordinates.x - GetMouseX()) * tc * player.xSensitivity;
		rotational_Movement_Speed.y = abs(pMouseCoordinates.y - GetMouseY()) * tc * player.ySensitivity;

		// rotation i x-led
		if (( pMouseCoordinates.x > GetMouseX() && GetMouseX() > 512 && GetMouseX() < ScreenWidth()))
		{ 
			player.angle -= rotational_Movement_Speed.x;      if (player.angle < DR)      player.angle += 2 * PI;
		} 
		else if (GetKey(olc::Key::G).bHeld) { player.angle -= 5 * tc * player.xSensitivity;  if (player.angle < DR)      player.angle += 2 * PI; }

		if (pMouseCoordinates.x < GetMouseX() && GetMouseX() > 512 && GetMouseX() < ScreenWidth() || GetKey(olc::Key::RIGHT).bHeld)
		{ 
			player.angle += rotational_Movement_Speed.x;   if (player.angle > 359 * DR)   player.angle -= 2 * PI;
		}
		else if (GetKey(olc::Key::J).bHeld) { player.angle += 5 * tc * player.xSensitivity;  if (player.angle > 359 * DR)   player.angle -= 2 * PI;}

		// rotation i y-led
		if ((pMouseCoordinates.y > GetMouseY() && GetMouseY() < ScreenHeight() && GetMouseY() > 0) || GetKey(olc::Key::DOWN).bHeld) { 
			wall.angle += rotational_Movement_Speed.y; if (wall.angle > 160 + 500) { wall.angle = 160 + 500; }
		}
		else if (GetKey(olc::Key::Y).bHeld) { wall.angle += 5 * tc * player.ySensitivity;  if (wall.angle > 160 + 500) { wall.angle = 160 + 500; } }

		if ((pMouseCoordinates.y < GetMouseY() && GetMouseY() < ScreenHeight() && GetMouseY() > 0) || GetKey(olc::Key::UP).bHeld) {
			wall.angle -= rotational_Movement_Speed.y; if (wall.angle < 160 - 500) { wall.angle = 160 - 500; }
		}
		else if (GetKey(olc::Key::H).bHeld) { wall.angle -= 5 * tc * player.ySensitivity;  if (wall.angle > 160 + 500) { wall.angle = 160 + 500; } }

		// saves mouse coordinates
		pMouseCoordinates = { (float)GetMouseX(), (float)GetMouseY() };

		// -- SET WORLD LIMITS --  

		Vector2D borderPos, mapPosCoordinates;
		int mapPos;

		// ritar en cirkel runt spelaren och kollar om någon väg rör cirkeln
		for (int i = 0; i < 10; i++) 
		{
			borderPos.x = player.width / 2 * cos(36 * DR * i);
			borderPos.y = player.width / 2 * sin(36 * DR * i);

			mapPosCoordinates.x = ((int)(player.previous_x + borderPos.x) >> 6) << 6;
			mapPosCoordinates.y = ((int)(player.previous_y + borderPos.y) >> 6) << 6;
			mapPos = mapPosCoordinates.y * 8 / 64 + mapPosCoordinates.x / 64;

			if (player.x + borderPos.x < mapPosCoordinates.x && map[mapPos - 1] > 0) { player.x = player.previous_x; }
			if (player.x + borderPos.x > mapPosCoordinates.x + 64 && map[mapPos + 1] > 0) { player.x = player.previous_x; }

			if (player.y + borderPos.y < mapPosCoordinates.y && map[mapPos - 8] > 0) { player.y = player.previous_y; }
			if (player.y + borderPos.y > mapPosCoordinates.y + 64 && map[mapPos + 8] > 0) { player.y = player.previous_y; }
		}
	}

	// [ cast rays and draw 3D world ]
	void DrawRays2D()
	{
		// dof = depth off field, changes how far the ray goes
		int r, mx, my, mp, dof, color, color2, color1, kontrast; float  rx, ry, ra, xo, yo, disT;
		float foW = 60, rayAngleIncrease;
		int colorV[3];

		// Elias förlåt mig för dessa variablar men jag ska fixa dem senare

		for (r = 0; r < 1025; r++)
		{
		    rayAngleIncrease = atan(-tan(foW * DR / 2) + (tan(foW * DR / 2) / 512) * r);
			ra = player.angle + rayAngleIncrease;  if (ra < 0) { ra += 2 * PI; } if (ra > 2 * PI) { ra -= 2 * PI; }
			
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
			else if (color == 2) { colorV[0] = 100; colorV[1] = 255; colorV[2] = 160; }
			else { colorV[0] = 100; colorV[1] = 255; colorV[2] = 230; }

			DrawLine(player.x, player.y, rx, ry, olc::DARK_RED);

			//-- DRAW 3D WORLD --
			float ca = player.angle - ra; if (ca < 0) { ca += 2 * PI; } if (ca > 2 * PI) { ca -= 2 * PI; } disT = disT * cos(ca);  // best�mmer distans till v�gg
			float lineH = (mapS * 320) / disT;                                                                                  //Line Height
			float lineO = wall.angle - lineH / (2 + wall.offset);                                                                        //Line Offset
			float alphaV = 255 / (disT * 0.0085 + 1);

			FillRect(r * 1 + 512, lineO, 1, lineH, olc::Pixel(colorV[0], colorV[1], colorV[2], alphaV - kontrast));
			//PaintTextures(r* 1 + 512, lineO, lineH, alphaV - kontrast);
		}
	}

	void PaintTextures(int r , float lineO, float lineH,int alphaV, int colorV[3]) {
		int const size = (8);
		int texture[size*size] =
		{
			 1,1,1,1,1,1,1,1,
			 1,0,0,0,0,0,0,1,
			 1,1,0,0,0,1,0,1,
			 1,0,0,0,0,0,0,1,
			 1,0,0,0,0,0,0,1,
			 1,0,0,0,0,0,0,1,
			 1,0,0,0,0,0,0,1,
			 1,1,1,1,1,1,1,1,
		};

		float pixelHight = lineH / size;

		for (int i = 0; i < size; i++) {
			if ((texture[i * size + 1]) == 1) {
				int colour[3] = { colorV[0],colorV[1],colorV[3] };
				FillRect(r, lineO + (pixelHight * i), 1, pixelHight, olc::Pixel(colour[0], colour[1], colour[2], alphaV));
			}else if ((texture[i * size + 1]) == 0) {
				int coulor[3] = { colorV[1],colorV[0],colorV[3] };
				FillRect(r, lineO + (pixelHight * i), 1, pixelHight, olc::Pixel(coulor[0], coulor[1], coulor[2], alphaV));
			}
		}
	}


	void backgroundColor(olc::Pixel color) {
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), color);
	}


	float Pyth(float ax, float ay, float bx, float by)  //pythagoras sats
	{
		return sqrt(pow((bx - ax), 2) + pow((by - ay), 2));
	}

	Vector2D addVectors(Vector2D vektor1, Vector2D vektor2)
	{
		Vector2D resVector;
		resVector.x = vektor1.x + vektor2.x;
		resVector.y = vektor1.y + vektor2.y;
		return resVector;
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
		player.angle = PI2;
		player.movement_speed = 100;
		player.walk_animation_speed = 1;
		player.walk_animation_waveLength = 4;
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		backgroundColor(olc::DARK_BLUE);
		FillRect(512, (ScreenHeight() + 2* wall.angle - 480) / 2, ScreenWidth(), ScreenHeight()*4, olc::DARK_GREEN);
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
