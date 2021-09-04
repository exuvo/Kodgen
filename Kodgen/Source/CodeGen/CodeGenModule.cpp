#include "Kodgen/CodeGen/CodeGenModule.h"

#include <algorithm>

#include "Kodgen/CodeGen/PropertyCodeGen.h"
#include "Kodgen/CodeGen/CodeGenEnv.h"

using namespace kodgen;

bool CodeGenModule::initialize(CodeGenEnv& env) noexcept
{
	return true;
}

ETraversalBehaviour CodeGenModule::generateCode(EntityInfo const* entity, CodeGenEnv& env, std::string& inout_result, void const* /* data */) noexcept
{
	return generateCode(entity, env, inout_result);
}

void CodeGenModule::addPropertyCodeGen(PropertyCodeGen& propertyCodeGen) noexcept
{
	_propertyCodeGenerators.push_back(&propertyCodeGen);
}

bool CodeGenModule::removePropertyCodeGen(PropertyCodeGen const& propertyCodeGen) noexcept
{
	auto it = std::find(_propertyCodeGenerators.cbegin(), _propertyCodeGenerators.cend(), &propertyCodeGen);

	if (it != _propertyCodeGenerators.cend())
	{
		_propertyCodeGenerators.erase(it);

		return true;
	}

	return false;
}

ETraversalBehaviour CodeGenModule::callVisitorOnEntity(EntityInfo const* entity, CodeGenEnv& env, std::function<ETraversalBehaviour(ICodeGenerator&, EntityInfo const*, CodeGenEnv&, void const*)> visitor) noexcept
{
	assert(visitor != nullptr);

	return visitor(*this, entity, env, nullptr);
}

std::vector<PropertyCodeGen*> const& CodeGenModule::getPropertyCodeGenerators() const noexcept
{
	return _propertyCodeGenerators;
}