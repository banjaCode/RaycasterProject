#define OLC_PGE_APPLICATION
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
		int foW;
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

	struct rgb_color 
	{
		int r, g, b;
		string name;
	};
    //generate textures



	rgb_color color[8] =
	{
		{191, 69, 43, "red"},
		{224, 202, 22 , "yellow"},
		{55, 216, 22 , "green"},
		{22, 216, 203, "sky"},
		{216, 22, 93 , "evning"},



		{60, 58, 58, "dark_grey"},
		{122, 122, 122, "light_grey"},
	    {85, 81, 81, "grey"}
	};
	
	int coulorArray[3][64 * 64];
	
	int textur[3][16 * 16]
	{
		{
            1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
            0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
			1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
            0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
			1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
            0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
			1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
            0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
			1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
			0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
			1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
			0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
			1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
			0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
			1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,
			0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,
		},
		{
			1,3,5,3,2,3,1,4,5,3,4,2,4,2,4,2,
			4,3,4,2,4,2,5,3,5,3,4,2,4,2,2,0,
			0,0,0,0,0,0,0,0,0,4,4,4,4,4,4,4,
			2,2,2,2,4,3,2,3,3,2,4,2,4,2,4,5,
			2,2,2,2,4,3,2,3,3,2,4,2,4,2,4,5,
			4,3,4,2,4,2,5,3,5,3,4,2,4,2,2,0,
			4,3,4,2,4,2,5,3,5,3,4,2,4,2,2,0,
			1,3,5,3,2,3,1,4,5,3,4,2,4,2,4,2,
			4,3,4,2,4,2,5,3,5,3,4,2,4,2,2,0,
			4,3,4,2,4,2,5,3,5,3,4,2,4,2,2,0,
			1,3,5,3,2,3,1,4,5,3,4,2,4,2,4,2,
			1,3,5,3,2,3,1,4,5,3,4,2,4,2,4,2,
			1,3,5,3,2,3,1,4,5,3,4,2,4,2,4,2,
			2,2,2,2,4,3,2,3,3,2,4,2,4,2,4,5,
			2,2,2,2,4,3,2,3,3,2,4,2,4,2,4,5,
			2,2,2,2,4,3,2,3,3,2,4,2,4,2,4,5,
		},
		{ //id 3 CobledStone
			6,6,6,7,6,6,6,6,6,7,6,6,6,7,6,6,
			6,6,6,7,6,6,6,6,7,6,6,6,7,6,6,6,
			6,6,6,7,7,6,6,6,7,6,6,6,7,5,6,6,
			7,6,7,6,5,7,7,7,6,7,7,6,6,7,7,7,
			6,7,6,6,6,7,6,6,6,6,7,7,7,6,6,6,
			6,5,7,6,6,7,6,6,6,6,7,6,6,7,6,6,
			6,6,7,6,6,6,7,7,7,7,6,6,6,6,7,6,
			6,6,7,7,6,7,5,6,6,6,7,6,6,6,7,6,
			6,7,6,6,7,7,6,6,6,6,7,7,6,6,7,6,
			7,6,7,6,6,6,7,6,6,7,6,6,7,6,7,7,
			6,6,7,6,6,6,7,7,7,6,6,6,6,7,5,6,
			6,6,6,7,6,7,6,6,5,7,6,6,6,7,6,6,
			7,7,5,7,7,7,6,6,6,6,7,7,7,6,6,6,
			6,6,7,6,6,6,7,6,6,7,6,6,5,7,6,7,
			6,6,7,6,6,6,6,7,7,7,6,6,6,6,7,6,
			6,6,6,7,6,6,6,6,6,7,6,6,6,6,7,6,
		}
	};


	// [ Map layout ]
	/*
	const static int mapWidth = 8, mapHeight = 8, mapS = 64;
	int map[mapS] =
	{
		 1,1,1,1,2,1,1,1,
		 2,0,2,0,0,0,0,2,
		 1,0,1,0,0,0,0,1,
		 1,0,0,0,2,0,1,2,
		 1,0,1,0,2,0,0,2,
		 2,0,0,0,0,0,0,1,
		 1,0,1,0,0,0,0,2,
		 1,1,1,2,1,3,3,1,
	};
	*/
	
	const static int mapS = 64 / 2;
	const static int mapWidth = 16, mapHeight = 16, mapS2 = 256;
	int map[mapS2] =
	{
		 1,1,3,3,3,3,3,3,2,1,2,1,1,1,1,2,
		 2,0,3,0,0,0,0,3,2,0,2,0,0,0,0,2,
		 1,0,3,0,0,0,0,3,0,0,2,0,0,0,0,2,
		 1,0,0,0,3,0,3,3,2,0,2,0,0,0,0,2,
		 1,0,1,0,2,0,0,2,0,0,2,0,0,0,0,2,
		 2,0,0,0,0,0,0,0,0,0,2,0,0,0,0,2,
		 1,0,1,0,0,0,0,0,0,0,2,0,0,0,0,2,
		 1,1,1,2,0,0,1,1,2,0,2,0,0,0,0,2,
		 1,1,1,1,2,0,1,0,2,0,2,0,0,0,0,2,
		 2,0,2,0,0,0,0,2,2,0,0,0,0,0,0,2,
		 1,0,1,0,0,0,0,1,0,0,0,0,0,0,0,2,
		 1,0,0,0,2,0,1,2,2,0,2,0,0,0,0,2,
		 1,0,1,0,2,0,0,2,0,0,2,0,0,0,0,2,
		 2,0,0,0,0,0,0,1,2,0,2,0,0,0,0,2,
		 1,0,1,0,0,0,0,0,0,0,2,0,0,0,0,2,
		 1,1,1,2,1,2,1,1,2,1,2,1,1,1,1,2,
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

		// ritar en cirkel runt spelaren och kollar om någon vägg rör cirkeln
		for (int i = 0; i < 360; i++) 
		{
			borderPos.x = player.width / 2 * cos(360 / 10 * DR * i);
			borderPos.y = player.width / 2 * sin(360 / 10 * DR * i);

			mapPosCoordinates.x = ((int)(player.previous_x + borderPos.x) >> 5) << 5;
			mapPosCoordinates.y = ((int)(player.previous_y + borderPos.y) >> 5) << 5;
			mapPos = mapPosCoordinates.y * mapWidth / 32 + mapPosCoordinates.x / 32;

			if (player.x + borderPos.x < mapPosCoordinates.x && map[mapPos - 1] > 0) { player.x = player.previous_x; }
			if (player.x + borderPos.x > mapPosCoordinates.x + 32 && map[mapPos + 1] > 0) { player.x = player.previous_x; }

			if (player.y + borderPos.y < mapPosCoordinates.y && map[mapPos - 16] > 0) { player.y = player.previous_y; }
			if (player.y + borderPos.y > mapPosCoordinates.y + 32 && map[mapPos + 16] > 0) { player.y = player.previous_y; }
		}
	}

	// [ cast rays and draw 3D world ]
	void DrawRays2D()
	{
		// dof = depth off field, changes how far the ray goes
		int mapPosX, mapPosY, mapPos, doF, kontrast = 0; 
		float  rayPosX, rayPosY, rayAngle, xOffset, yOffset, disT;

		int rayCast = 1024;
		int mp2, mp3;

		

		for (int r = 0; r < rayCast; r++)
		{
		    float rayAngleIncrease = atan(-tan(player.foW * DR / 2) + (tan(player.foW * DR / 2) / (rayCast / 2)) * r);
			rayAngle = player.angle + rayAngleIncrease;  if (rayAngle < 0) { rayAngle += 2 * PI; } if (rayAngle > 2 * PI) { rayAngle -= 2 * PI; }
			
			//--  CHECK HORIZONTAL LINES --
			doF = 0;
			float disH = 1000000, hx = player.x, hy = player.y;
			float aTan = -1 / tan(rayAngle);	

			bool lookingUp = false;

			if (rayAngle > PI) //looking up
			{ 
			rayPosY = floor(player.y / 32) * 32;
			rayPosX = (player.y - rayPosY) * aTan + player.x;
			yOffset = -32; xOffset = -yOffset * aTan; 

			lookingUp = true;
			}

			else if (rayAngle < PI)	//looking down
			{
				rayPosY = floor(player.y / 32) * 32 + 32;
				rayPosX = (player.y - rayPosY) * aTan + player.x;
				yOffset = 32; xOffset = -yOffset * aTan; 
			} 

			else if (rayAngle == 0 || rayAngle == PI) 	//looking straight left or right
			{
				rayPosX = player.x;
				rayPosY = player.y;
				doF = 32; 
			}

			while (doF < 32) 			
			{
				if (lookingUp) { mapPosY = (int)(rayPosY - 0.0001) >> 5; }
				else { mapPosY = (int)(rayPosY) >> 5; }

				mapPosX = (int)(rayPosX) >> 5; mapPos = mapPosY * mapWidth + mapPosX;
				if (mapPos > 0 && mapPos <= (mapWidth * mapHeight) && map[mapPos] > 0) 
				{ 
					mp2 = mapPos;
					hx = rayPosX; hy = rayPosY; 
					disH = Pyth(player.x, player.y, hx, hy); 
					break; 
				} 
				else { rayPosX += xOffset; rayPosY += yOffset; doF += 1; }
			}

			//-- check vertical lines--
			doF = 0;
			float disV = 1000000, vx = player.x, vy = player.y;
			bool lookingLeft = false;

			float nTan = -tan(rayAngle);

			if (rayAngle > PI2 && rayAngle < PI3) // Looking left
			{
				rayPosX = floor(player.x / 32) * 32.0000; 
				rayPosY = (player.x - rayPosX) * nTan + player.y;
				xOffset = -32; yOffset = -xOffset * nTan;
				lookingLeft = true;
			}

			else if (rayAngle < PI2 || rayAngle > PI3) // Looking right
			{
				rayPosX = floor(player.x / 32) * 32 + 32;
				rayPosY = (player.x - rayPosX) * nTan + player.y;
				xOffset = 32; yOffset = -xOffset * nTan;
			} 

			else if (rayAngle == 0 || rayAngle == PI) //looking straight up or down
			{
				rayPosX = player.x; 
				rayPosY = player.y;
				doF = 32; 
			}

			while (doF < 32)
			{
				if (lookingLeft) { mapPosX = (int)(rayPosX - 0.0001) >> 5; }
				else 
				{
					mapPosX = (int)(rayPosX) >> 5;
				}
				mapPosY = (int)(rayPosY) >> 5; mapPos = mapPosY * mapWidth + mapPosX;
				if (mapPos > 0 && mapPos <= (mapWidth * mapHeight) && map[mapPos] > 0) 
				{ 
					mp3 = mapPos;
					vx = rayPosX; vy = rayPosY;
					disV = Pyth(player.x, player.y, vx, vy);
					break; 
				}
				else { rayPosX += xOffset; rayPosY += yOffset; doF += 1; }
			}
		
			if (disV < disH)  // set ray distans depending on which is longer
			{
				rayPosX = vx; rayPosY = vy; disT = disV; kontrast = 10;
				mapPos = mp3;
			}
			else
			{
				rayPosX = hx; rayPosY = hy; disT = disH;
				kontrast = 10;
				mapPos = mp2;
			}

			DrawLine(player.x, player.y, rayPosX, rayPosY, olc::DARK_RED);

			//-- DRAW 3D WORLD --
			float ca = player.angle - rayAngle; if (ca < 0) { ca += 2 * PI; } if (ca > 2 * PI) { ca -= 2 * PI; } disT = disT * cos(ca);  // best�mmer distans till v�gg
			float lineH = mapS * (mapS / ((tan(player.foW / 2 * DR) * mapS) / (rayCast / 2))) / disT;                                  	 //Line Height (OBS: ändra * 64 ifall mapp ändras)
			float lineOffset = wall.angle - lineH / (2 + wall.offset);                                                                   //Line Offset
			float alphaV = 255 / (disT * 0.0085 + 1);

			//PaintTextures(r * 1 + 512, lineOffset, lineH, rayPosX ,rayPosY, mapPos, alphaV, kontrast);
			PaintTexture1(r * 1 + 512, lineOffset, lineH, rayPosX, rayPosY, mapPos, alphaV, kontrast, coulorArray);
		}
	}

	void PaintTextures(int lineX, float lineOffset, float lineH, float rayPosX, float rayPosY, int mapPos, float alphaV, int kontrast) {

		int const size = 16;

		int mapPosX = floor((mapPos - (mapPos / mapWidth) * mapWidth) * 32);
		int mapPosY = floor((mapPos / mapHeight) * 32);
		float textureX = rayPosX - mapPosX;
		float textureY = rayPosY - mapPosY;
		int facing = 0, column = 0;


		if (rayPosY == mapPosY && rayPosX != mapPosX && rayPosX != mapPosX + mapS) //uppifrån id 1
		{
			facing = 1;
			column = 1.00000 / (size * size) * (rayPosX - mapPosX) * size * (size * size / mapS);
		}
		else if (rayPosY == mapPosY + mapS && rayPosX != mapPosX + mapS && rayPosX != mapPosX) //nedifrån id 2
		{
			facing = 2;
			column = 1.00000 / (size * size) * (mapS - (rayPosX - mapPosX)) * size * (size * size / mapS);
		}
		if (rayPosX == mapPosX && rayPosY != mapPosY && rayPosY != mapPosY + mapS) //höger id 3
		{
			facing = 3;
			column = 1.00000 / (size * size) * (mapS - (rayPosY - mapPosY)) * size * (size * size / mapS);
		}
		else if (rayPosX == mapPosX + mapS && rayPosY != mapPosY + mapS && rayPosY != mapPosY) //Anta vänster id 4, alla övriga testade
		{
			facing = 4;
			column = 1.00000 / (size * size) * (rayPosY - mapPosY) * size * (size * size / mapS);
		}
		if (column >= size) {  column = size - 1; }

		float pixelHeight = lineH / size;

		if (facing != 0) {
			for (int i = 0; i < size; i++) {

				int index = textur[map[mapPos] - 1][(i * size) + column];

				FillRect(lineX, lineOffset + (pixelHeight * i), 1, pixelHeight + 2, olc::Pixel(color[index].r, color[index].g, color[index].b, alphaV - kontrast));
			}
		}
	}

	

	int** GenerateTextures(string path) 
	{
		olc::Sprite* sprpointer;
		std::unique_ptr<olc::Sprite> sprTile;
		sprTile = std::make_unique<olc::Sprite>(path);
		olc::Pixel currentpixel;
		int currentr;
		int currentg;
		int currentb;
		const int size = 64;
		int coulorArray[3][size * size]
		{
			{//r

			},
			{//g

			},
			{ //b

			}
		};
		for (int c; c < 64; c++)
		{
			for (int r; r < 64; r++)
			{
				sprpointer = sprTile.get();
				currentpixel = (*sprpointer).GetPixel(c, r);
				coulorArray[1][c*size + r] = currentpixel.r;
				coulorArray[2][c * size + r] = currentpixel.g;
				coulorArray[3][c * size + r] = currentpixel.b;
			}
		}
		return coulorArray;
	}

	void PaintTextures2(int lineX, float lineOffset, float lineH, float rayPosX, float rayPosY, int mapPos, float alphaV, int kontrast, int Aarray[3][64 * 64]) {

		int const size = 64;

		int mapPosX = floor((mapPos - (mapPos / mapWidth) * mapWidth) * 32);
		int mapPosY = floor((mapPos / mapHeight) * 32);
		float textureX = rayPosX - mapPosX;
		float textureY = rayPosY - mapPosY;
		int facing = 0, column = 0;






		if (rayPosY == mapPosY && rayPosX != mapPosX && rayPosX != mapPosX + mapS) //uppifrån id 1
		{
			facing = 1;
			column = 1.00000 / (size * size) * (rayPosX - mapPosX) * size * (size * size / mapS);
		}
		else if (rayPosY == mapPosY + mapS && rayPosX != mapPosX + mapS && rayPosX != mapPosX) //nedifrån id 2
		{
			facing = 2;
			column = 1.00000 / (size * size) * (mapS - (rayPosX - mapPosX)) * size * (size * size / mapS);
		}
		if (rayPosX == mapPosX && rayPosY != mapPosY && rayPosY != mapPosY + mapS) //höger id 3
		{
			facing = 3;
			column = 1.00000 / (size * size) * (mapS - (rayPosY - mapPosY)) * size * (size * size / mapS);
		}
		else if (rayPosX == mapPosX + mapS && rayPosY != mapPosY + mapS && rayPosY != mapPosY) //Anta vänster id 4, alla övriga testade
		{
			facing = 4;
			column = 1.00000 / (size * size) * (rayPosY - mapPosY) * size * (size * size / mapS);
		}
		if (column >= size) { column = size - 1; }

		float pixelHeight = lineH / size;


		//Aarray[3][64 * 64]


		if (facing != 0) {
			for (int i = 0; i < size; i++) {

				int index = textur[map[mapPos] - 1][(i * size) + column];
				FillRect(lineX, lineOffset + (pixelHeight * i), 1, pixelHeight + 2, olc::Pixel(Aarray[1][column * i], Aarray[2][column * i], Aarray[3][column * i], alphaV - kontrast));
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
		player.x = 288; player.y = 128; player.width = 4;
		player.angle = PI2;
		player.movement_speed = 100;
		player.walk_animation_speed = 1;
		player.walk_animation_waveLength = 4;
		player.foW = 60;

		GenerateTextures("../../RaycasterProject/RaycasterProject/W3d_protoredbrick1.png");



		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		backgroundColor(olc::DARK_BLUE);
		FillRect(512, (ScreenHeight() + 2* wall.angle - 480) / 2, ScreenWidth(), ScreenHeight()*4, olc::DARK_GREEN);
		FillRect(0, 0, 512, 512 ,olc::Pixel (255 ,255 ,255 ,100));
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
	if (demo.Construct(512 * 3, 512, 1, 1))
		demo.Start();
	return 0;
}
