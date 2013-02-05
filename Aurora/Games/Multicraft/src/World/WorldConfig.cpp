#include "WorldConfig.h"

int WorldConfig::WorldViewX = 8;
int WorldConfig::WorldViewY = 3;
int WorldConfig::WorldViewZ = 8;

int WorldConfig::WorldMinY = 0;
int WorldConfig::WorldMaxY = 128;

int WorldConfig::ChunkCacheView = 9;

int WorldConfig::ChunkCacheCapacity = (WorldViewX + WorldViewX + 3) * (WorldViewZ + WorldViewZ + 3) * (WorldViewY + WorldViewY + 3);

float WorldConfig::WorldGravity = 9.8f;