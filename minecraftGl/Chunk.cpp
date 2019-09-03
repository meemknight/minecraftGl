#include "Chunk.h"

void Chunk::removeNeighboursLinkage()
{
	if(neighbours[CN::front] != nullptr)
	{
		neighbours[CN::front]->neighbours[CN::back] = nullptr;
	}

	if (neighbours[CN::back] != nullptr)
	{
		neighbours[CN::back]->neighbours[CN::front] = nullptr;
	}

	if (neighbours[CN::left] != nullptr)
	{
		neighbours[CN::left]->neighbours[CN::right] = nullptr;
	}

	if (neighbours[CN::right] != nullptr)
	{
		neighbours[CN::right]->neighbours[CN::left] = nullptr;
	}
}
