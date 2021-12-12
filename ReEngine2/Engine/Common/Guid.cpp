#include "Guid.h"

#include <random>

static std::random_device randomDevice;
static std::mt19937_64 randomEngine(randomDevice());
static std::uniform_int_distribution<uint64> uniformDistribution;

Guid::Guid()
	: _id(uniformDistribution(randomEngine))
{
}

Guid::Guid(uint64 guid)
	: _id(guid)
{

}