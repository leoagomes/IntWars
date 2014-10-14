#include "AIMesh.h"
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include "Logger.h"
#include "Object.h"
#include "Champion.h"
#include "RAFManager.h"

#define FRACPOS(x)		((x)-((int)(x)))


// TODO: Clean this junk up

AIMesh::AIMesh() :fileStream(0), heightMap(0), loaded(false)
{
}


AIMesh::~AIMesh()
{
   if (heightMap!=0)
      delete[] heightMap;
}

bool AIMesh::load(std::string inputFile)
{
   // Old direct file loading code
	/*std::ifstream t_FileStream(inputFile, std::ios::binary);
	t_FileStream.seekg(0, std::ios::end);
	std::streamsize size = t_FileStream.tellg();
	t_FileStream.seekg(0, std::ios::beg);

	buffer.resize((unsigned)size);

	if (t_FileStream.read(buffer.data(), size))
	{
		fileStream = (__AIMESHFILE*)buffer.data();
		outputMesh(AIMESH_TEXTURE_SIZE, AIMESH_TEXTURE_SIZE);
		return true;
	}*/

   // LEVELS/Map1/AIPath.aimesh
   loaded = false;
   buffer.clear();
   if (RAFManager::getInstance()->readFile(inputFile, buffer)) // Our file exists. Load it.
   {
      for (int i = 0; i < AIMESH_TEXTURE_SIZE; i++) // For every scanline for the triangle rendering
      {
         scanlineLowest[i].u = scanlineLowest[i].v = scanlineLowest[i].x = scanlineLowest[i].y = scanlineLowest[i].z = 1e10f; // Init to zero
         scanlineHighest[i].u = scanlineHighest[i].v = scanlineHighest[i].x = scanlineHighest[i].y = scanlineHighest[i].z = -1e10f;
      }

      heightMap = new float[AIMESH_TEXTURE_SIZE * AIMESH_TEXTURE_SIZE]; // Shows occupied or not
      xMap = new float[AIMESH_TEXTURE_SIZE * AIMESH_TEXTURE_SIZE]; // Shows x
      yMap = new float[AIMESH_TEXTURE_SIZE * AIMESH_TEXTURE_SIZE]; // Shows y
      fileStream = (__AIMESHFILE*)buffer.data();
      outputMesh(AIMESH_TEXTURE_SIZE, AIMESH_TEXTURE_SIZE);

      //writeFile(heightMap, AIMESH_TEXTURE_SIZE, AIMESH_TEXTURE_SIZE);

      std::cout << "Opened AIMesh file for this map." << std::endl;
      loaded = true;
      return true;
   }
   std::cout << "Failed opening AIMesh file for this map." << std::endl;
   return false;
}

bool AIMesh::outputMesh(unsigned width, unsigned height)
{
   mapHeight = mapWidth = 0;
	for (unsigned i = 0; i < width*height; i++) heightMap[i] = -99999.99f; // Clear the map

   lowX = 9e9, lowY = 9e9, highX = 0, highY = 0, lowestZ = 9e9; // Init triangle

	for (unsigned i = 0; i < fileStream->triangle_count; i++) 
   // Need to find the absolute values.. So we can map it to AIMESH_TEXTURE_SIZExAIMESH_TEXTURE_SIZE instead of 13000x15000
	{
      // Triangle low X check
		if (fileStream->triangles[i].Face.v1[0] < lowX)
			lowX = fileStream->triangles[i].Face.v1[0];
		if (fileStream->triangles[i].Face.v2[0] < lowX)
			lowX = fileStream->triangles[i].Face.v2[0];
		if (fileStream->triangles[i].Face.v3[0] < lowX)
			lowX = fileStream->triangles[i].Face.v3[0];		

      // Triangle low Y check
		if (fileStream->triangles[i].Face.v1[2] < lowY)
			lowY = fileStream->triangles[i].Face.v1[2];
		if (fileStream->triangles[i].Face.v2[2] < lowY)
			lowY = fileStream->triangles[i].Face.v2[2];
		if (fileStream->triangles[i].Face.v3[2] < lowY)
			lowY = fileStream->triangles[i].Face.v3[2];

      // Triangle high X check
		if (fileStream->triangles[i].Face.v1[0] > highX)
			highX = fileStream->triangles[i].Face.v1[0];
		if (fileStream->triangles[i].Face.v2[0] > highX)
			highX = fileStream->triangles[i].Face.v2[0];
		if (fileStream->triangles[i].Face.v3[0] > highX)
			highX = fileStream->triangles[i].Face.v3[0];
		
      // Triangle high Y check
		if (fileStream->triangles[i].Face.v1[2] > highY)
			highY = fileStream->triangles[i].Face.v1[2];
		if (fileStream->triangles[i].Face.v2[2] > highY)
			highY = fileStream->triangles[i].Face.v2[2];
		if (fileStream->triangles[i].Face.v3[2] > highY)
			highY = fileStream->triangles[i].Face.v3[2];
	}

   mapWidth = (highX - lowX);
   mapHeight = (highY - lowY);
   
   // If the width or width larger?
   if ((highY - lowY) < (highX - lowX))
   {
      highX = 1.0f / (highX - lowX)*width; // We're wider than we're high, map on width
      highY = highX; // Keep aspect ratio Basically, 1 y should be 1 x.
      lowY = 0; // Though we need to project this in the middle, no offset
   }
   else
   {
      highY = 1.0f / (highY - lowY)*height; // We're higher than we're wide, map on width
      highX = highY; // Keep aspect ratio Basically, 1 x should be 1 y.
      // lowX = 0; // X is already in the middle? ??????
   }

   for (unsigned i = 0; i <fileStream->triangle_count; i++) // For every triangle
	{
      Triangle t_Triangle; // Create a triangle that is warped to heightmap coordinates
      t_Triangle.Face.v1[0] = (fileStream->triangles[i].Face.v1[0] - lowX)*highX;
      t_Triangle.Face.v1[1] = fileStream->triangles[i].Face.v1[1];
      t_Triangle.Face.v1[2] = (fileStream->triangles[i].Face.v1[2] - lowY)*highY;

      t_Triangle.Face.v2[0] = (fileStream->triangles[i].Face.v2[0] - lowX)*highX;
      t_Triangle.Face.v2[1] = fileStream->triangles[i].Face.v2[1];
      t_Triangle.Face.v2[2] = (fileStream->triangles[i].Face.v2[2] - lowY)*highY;

      t_Triangle.Face.v3[0] = (fileStream->triangles[i].Face.v3[0] - lowX)*highX;
      t_Triangle.Face.v3[1] = fileStream->triangles[i].Face.v3[1];
      t_Triangle.Face.v3[2] = (fileStream->triangles[i].Face.v3[2] - lowY)*highY;

      /*
      // Draw just the wireframe.
      drawLine(t_Triangle.Face.v1[0], t_Triangle.Face.v1[2], t_Triangle.Face.v2[0], t_Triangle.Face.v2[2], t_Info, width, height);
      drawLine(t_Triangle.Face.v2[0], t_Triangle.Face.v2[2], t_Triangle.Face.v3[0], t_Triangle.Face.v3[2], t_Info, width, height);
      drawLine(t_Triangle.Face.v3[0], t_Triangle.Face.v3[2], t_Triangle.Face.v1[0], t_Triangle.Face.v1[2], t_Info, width, height);
      */

      // Draw this triangle onto the heightmap using an awesome triangle drawing function
      drawTriangle(t_Triangle, heightMap, width, height);
	}

	//writeFile(t_Info, width, height);
	return true;
}

float AIMesh::getY(float argX, float argY)
{   
   if (loaded)
   {
      argX = (int)((argX - lowX)*highX); argY = (int)((argY - lowY)*highY);


      if ((argX >= 0.0f && argX < AIMESH_TEXTURE_SIZE) && (argY >= 0.0f && argY < AIMESH_TEXTURE_SIZE))
      {
         int pos = (int)((argX) + (argY)*AIMESH_TEXTURE_SIZE);
         if (pos<AIMESH_TEXTURE_SIZE*AIMESH_TEXTURE_SIZE)
            return heightMap[pos];
      }
   }
   return -99999.99f;
}

Vector2 AIMesh::TranslateToTextureCoordinate(Vector2 vector)
{
   if (loaded)
   {
      vector.X = (int)((vector.X - lowX)*highX);
      vector.Y = (int)((vector.Y - lowY)*highY);
   }
   return vector;
}

Vector2 AIMesh::TranslateToRealCoordinate(Vector2 vector)
{
   if (loaded)
   {
      vector.X = (vector.X/highX)+lowX;
      vector.Y = (vector.Y/highY)+lowY;
   }
   return vector;
}

/*
#include <chrono>
bool tellme = false;
auto times = std::clock();*/

// Blatantly copy the line function
float AIMesh::castRaySqr(Vector2 origin, Vector2 direction, bool inverseRay)
{
   /*if (tellme)
      CORE_WARNING("Casting Ray from %f, %f in direction %f,%f", origin.X, origin.Y, direction.X, direction.Y);*/

   origin = TranslateToTextureCoordinate(origin);
   //direction = origin + (direction*2.0f); // Make sure it's long enough to reach a new square
   direction = TranslateToTextureCoordinate(direction); // Translate it to correct stuff 

   float x1 = origin.X;
   float y1 = origin.Y;
   float x2 = direction.X;
   float y2 = direction.Y;

   /*if (tellme)
      CORE_WARNING("The ray will fire from %f, %f to %f,%f", x1, y1, x2, y2);*/

   if ((x1 < 0) || (y1 < 0) || (x1 >= AIMESH_TEXTURE_SIZE) || (y1 >= AIMESH_TEXTURE_SIZE))
      return 0.0f; // Outside of map, collide immediately

   Vector2 delta = direction - origin;
   delta = delta.Normalize();

   while (isWalkable(x1, y1) != inverseRay) // The result needs to be the invert of inverseRay. if inverseRay is a boolean that 
                                            // tells us what the boolean of isWalkable needs to be. If not, keep traversing.
   {
      // BUG: Keeps traversing. Gotta fix it, kinda busy.
      //if (tellme) CORE_WARNING("(%f, %f), (%f, %f), y: %f\n", x1, y1, dx, dy, getY(x1, y1));
      x1 += delta.X, y1 += delta.Y;
   }

   /*if (tellme)
      CORE_WARNING("The ray ended at %f, %f. w: %s iR: %s y: %f", x1, y1, (isWalkable(x1, y1)) ? ("true") : ("false"), (inverseRay) ? ("true") : ("false"), getY(x1, y1));
   tellme = false;*/
   return (TranslateToRealCoordinate(Vector2(x1, y1)) - TranslateToRealCoordinate(origin)).SqrLength();
}

bool AIMesh::isAnythingBetween(Object* a, Object* b)
{
   Vector2 direction = (b->getPosition() - a->getPosition());
  
   Vector2 a_pos = a->getPosition();// +direction*a->getCollisionRadius(); // TODO: add the collisionRadius for towers.  I disabled this for more precision on non-towers.
   
   
   /*if (std::clock() - times > 4000 && (dynamic_cast<Champion*>(a) != 0 || dynamic_cast<Champion*>(b) != 0))
   {
      tellme = true;
   }*/

   float raydist = castRaySqr(a_pos, b->getPosition());// < distance*distance)
   bool raydistb = (raydist < direction.SqrLength());

   /*if (std::clock() - times > 4000)
   {
      if (dynamic_cast<Champion*>(a) != 0)
      {
         CORE_WARNING("Champion position: (%f, %f)", a->getPosition().X, a->getPosition().Y);
         CORE_WARNING("Minion position: (%f, %f)", b->getPosition().X, b->getPosition().Y);
         CORE_WARNING("Translated c: (%f, %f)", this->TranslateToTextureCoordinate(a->getPosition()).X, this->TranslateToTextureCoordinate(a->getPosition()).Y);
         CORE_WARNING("Translated m: (%f, %f)", this->TranslateToTextureCoordinate(b->getPosition()).X, this->TranslateToTextureCoordinate(b->getPosition()).Y);
         CORE_WARNING("dir: (%f, %f)", direction.Normalize().X, direction.Normalize().Y);
         if (raydistb)
            CORE_WARNING("raydistb is true. Something is in the way.");
         else CORE_WARNING("raydistb is false. Nothing is there.");
         times = std::clock();
      }
      else if (dynamic_cast<Champion*>(b) != 0)
      {
         CORE_WARNING("Champion position: (%f, %f)", b->getPosition().X, b->getPosition().Y);
         CORE_WARNING("Minion position: (%f, %f)", a->getPosition().X, a->getPosition().Y);
         CORE_WARNING("Translated Champ: (%f, %f)", this->TranslateToTextureCoordinate(b->getPosition()).X, this->TranslateToTextureCoordinate(b->getPosition()).Y);
         CORE_WARNING("Translated minion: (%f, %f)", this->TranslateToTextureCoordinate(a->getPosition()).X, this->TranslateToTextureCoordinate(a->getPosition()).Y);
         CORE_WARNING("dir: (%f, %f)", direction.Normalize().X, direction.Normalize().Y);
         if (raydistb)
            CORE_WARNING("raydistb is true. Something is in the way.");
         else CORE_WARNING("raydistb is false. Nothing is there.");
         times = std::clock();
      }
   }*/

   return raydistb;
}

bool AIMesh::isAnythingBetween(Vector2 a, Vector2 b)
{
   return (castRaySqr(a, b) < (b - a).SqrLength());
}



bool AIMesh::writeFile(float *pixelInfo, unsigned width, unsigned height)
{
#define MIN(a,b) (((a)>(b))?(b):(a))
#define MAX(a,b) (((a)>(b))?(a):(b))
   std::ofstream imageFile("test.tga", std::ios::out | std::ios::binary);
   if (!imageFile) return false;

   // The image header
   unsigned char header[18] = { 0 };
   header[2] = 1;  // truecolor
   header[12] = width & 0xFF;
   header[13] = (width >> 8) & 0xFF;
   header[14] = height & 0xFF;
   header[15] = (height >> 8) & 0xFF;
   header[16] = 24;  // bits per pixel

   imageFile.write((const char*)header, 18);

   //for (int y = 0; y < height; y++)
   for (int y = height - 1; y >= 0; y--)
      for (int x = 0; x < width; x++)
      {
         /* blue */  imageFile.put((char)MAX(MIN(pixelInfo[(y * height) + x]+(255-185),255.0f), 0.0f));
         /* green */ imageFile.put(((float)(y) / 1024.0f)*256.0f);
         /* red */   imageFile.put(((float)(x) / 1024.0f)*256.0f);
      }

   // The file footer. This part is totally optional.
   static const char footer[26] =
      "\0\0\0\0"  // no extension area
      "\0\0\0\0"  // no developer directory
      "TRUEVISION-XFILE"  // Yep, this is a TGA file
      ".";
   imageFile.write(footer, 26);

   imageFile.close();
   return true;
}

void AIMesh::drawTriangle(Triangle triangle, float *heightInfo, unsigned width, unsigned height)
{
#define MIN(a,b) (((a)>(b))?(b):(a))
#define MAX(a,b) (((a)>(b))?(a):(b))
   // The heart of the rasterizer

   Vertex tempVertex[3];

   tempVertex[0].x = triangle.Face.v1[0];
   tempVertex[0].y = triangle.Face.v1[2];
   tempVertex[0].z = triangle.Face.v1[1];

   tempVertex[1].x = triangle.Face.v2[0];
   tempVertex[1].y = triangle.Face.v2[2];
   tempVertex[1].z = triangle.Face.v2[1];

   tempVertex[2].x = triangle.Face.v3[0];
   tempVertex[2].y = triangle.Face.v3[2];
   tempVertex[2].z = triangle.Face.v3[1];

   fillScanLine(tempVertex[0], tempVertex[1]);
   fillScanLine(tempVertex[1], tempVertex[2]);
   fillScanLine(tempVertex[2], tempVertex[0]);

   float tempWidth = width;
   float tempHeight = height;
   // target width and width

   int startY = (int)MIN(tempVertex[0].y, MIN(tempVertex[1].y, tempVertex[2].y));
   int endY = (int)MAX(tempVertex[0].y, MAX(tempVertex[1].y, tempVertex[2].y));
   // Get the scanline where we start drawing and where we stop.

   endY = MIN(endY, height - 1);
   startY = MAX(0, startY);

   for (int y = startY; y <= endY; y++) // for each scanline
   {
      if (scanlineLowest[y].x<scanlineHighest[y].x) // If we actually have something filled in this scanline
      {
         int yw = y * height;

         float z = scanlineLowest[y].z;
         float u = scanlineLowest[y].u;
         float v = scanlineLowest[y].v;
         // The start of the Z, U, and V coordinate.

         float deltaX = 1.f / (scanlineHighest[y].x - scanlineLowest[y].x);
         // Interpolation over X (change in X between the two, then reverse it so it's usable as multiplication
         // in divisions

         float deltaZ = (scanlineHighest[y].z - scanlineLowest[y].z) * deltaX;
         float deltaU = (scanlineHighest[y].u - scanlineLowest[y].u) * deltaX;
         float deltaV = (scanlineHighest[y].v - scanlineLowest[y].v) * deltaX;
         // The interpolation in Z, U and V in respect to the interpolation of X	

         // Sub-texel correction
         int x = (int)scanlineLowest[y].x;
         int tx = x + 1;
         int distInt = (int)(scanlineHighest[y].x) - (int)(scanlineLowest[y].x);
         if (distInt > 0.0f)
         {
            u += (((float)(tx)) - tx) * deltaU;
            v += (((float)(tx)) - tx) * deltaV;
            z += (((float)(tx)) - tx) * deltaZ;
         }

         if (!(scanlineHighest[y].x<0 || x >= height))
         for (int i = 0; x<(int)scanlineHighest[y].x; i++, x++) // for each piece of the scanline
         {
            if (x >= height) break; // If we're out of screen, break out this loop

            if (x<0)
            {
               int inverseX = abs(x);
               z += deltaZ * inverseX;
               u += deltaU * inverseX;
               v += deltaV * inverseX;
               x = 0;
            }        


            {
               // Get the point on the texture that we need to draw. It basically picks a pixel based on the uv.

               //a_Target->GetRenderTarget()->Plot(x, tempHeight - y, 255);
               heightInfo[x + (y)* height] = z;
               xMap[x + (y)* height] = scanlineLowest[y].x + deltaX*i;
               yMap[x + (y)* height] = scanlineLowest[y].y;
               if (z < lowestZ) lowestZ = z;
            }

            z += deltaZ;
            u += deltaU;
            v += deltaV;
            // interpolate
         }
      }

      scanlineLowest[y].x = 1e10f;
      scanlineHighest[y].x = -1e10f;
   }
}

void AIMesh::fillScanLine(Vertex vertex1, Vertex vertex2)
{
   // Fills a scanline structure with information about the triangle on this y scanline.

   if (vertex1.y > vertex2.y)
      return fillScanLine(vertex2, vertex1);
   // We need to go from low to high so switch if the other one is higher

   if (vertex2.y < 0 || vertex1.y >= AIMESH_TEXTURE_SIZE)
      return;
   // There's nothing on this line

   Vertex deltaPos;
   
   deltaPos.x = vertex2.x - vertex1.x;
   deltaPos.y = vertex2.y - vertex1.y;
   deltaPos.z = vertex2.z - vertex1.z;

   float tempWidth = AIMESH_TEXTURE_SIZE;
   float tempHeight = AIMESH_TEXTURE_SIZE;

   float t_DYResp = deltaPos.y == 0 ? 0 : 1.f / deltaPos.y;
   int startY = (int)vertex1.y, endY = (int)vertex2.y;

   float x = vertex1.x;
   float z = vertex1.z;

   float deltaX = deltaPos.x * t_DYResp;
   float deltaZ = deltaPos.z * t_DYResp;

   float t_Inc = 1.f - FRACPOS(vertex1.y);

   // subpixel correction
   startY++;
   x += deltaX * t_Inc;
   z += deltaZ * t_Inc;

   if (startY < 0)
   {
      x -= deltaX * startY;
      z -= deltaZ * startY;
      startY = 0;
   }

   // Small fix
   if (endY >= AIMESH_TEXTURE_SIZE) endY = AIMESH_TEXTURE_SIZE - 1;

   // For each scanline that this triangle uses
   for (int y = startY; y <= endY; y++)
   {
      if (x<scanlineLowest[y].x) // If the x is lower than our lowest x
      {
         scanlineLowest[y].x = (x); // Fill the scanline struct with our info
         scanlineLowest[y].y = (float)y;
         scanlineLowest[y].z = z;
      }
      if (x>scanlineHighest[y].x) // If the x is higher than our highest x
      {
         scanlineHighest[y].x = (x); // Fill the scanline struct with our info
         scanlineHighest[y].y = (float)y;
         scanlineHighest[y].z = z;
      }

      // Interpolate
      // Or going to the part of the triangle on the next scanline
      x += deltaX;
      z += deltaZ;
   }
}