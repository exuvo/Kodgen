#include "Kodgen/CodeGen/ICodeGenerator.h"

using namespace kodgen;

int32 ICodeGenerator::getGenerationOrder() const noexcept
{
	return 0;
}

uint8 ICodeGenerator::getIterationCount() const noexcept
{
	return 1u;
}