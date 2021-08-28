#include "Kodgen/CodeGen/PropertyCodeGen.h"

using namespace kodgen;

PropertyCodeGen::PropertyCodeGen(std::string const&	propertyName, EEntityType eligibleEntityMask) noexcept:
	_propertyName{propertyName},
	_eligibleEntityMask{eligibleEntityMask}
{
}

bool PropertyCodeGen::initialize(CodeGenEnv& /* env */) noexcept
{
	return true;
}

bool PropertyCodeGen::shouldGenerateCode(EntityInfo const& entity, Property const& property, uint8 /* propertyIndex */) const noexcept
{
	return property.name == _propertyName && (entity.entityType && _eligibleEntityMask);
}

bool PropertyCodeGen::shouldIterateOnNestedEntities(EntityInfo const& entity) const noexcept
{
	switch (entity.entityType)
	{
		case EEntityType::Namespace:
			return NamespaceInfo::nestedEntityTypes && _eligibleEntityMask;

		case EEntityType::Struct:
			[[fallthrough]];
		case EEntityType::Class:
			return StructClassInfo::nestedEntityTypes && _eligibleEntityMask;

		case EEntityType::Enum:
			return EnumInfo::nestedEntityTypes && _eligibleEntityMask;

		default:
			//EnumValue
			//Functions
			//Methods
			//Fields
			//Variables
			return false;
	}
}

ETraversalBehaviour PropertyCodeGen::callVisitorOnEntity(EntityInfo const* entity, CodeGenEnv& env, std::function<ETraversalBehaviour(ICodeGenerator&, EntityInfo const*, CodeGenEnv&, void const*)> visitor) noexcept
{
	assert(visitor != nullptr);

	if (entity != nullptr)
	{
		//Call the visitor if the entity type is contained in the _eligibleEntities mask
		if (_eligibleEntityMask && entity->entityType)
		{
			AdditionalData data;

			//Execute the visitor on each property contained in the entity
			for (uint8 i = 0; i < entity->properties.size(); i++)
			{
				data.propertyIndex = i;
				data.property = &entity->properties[i];

				if (shouldGenerateCode(*entity, *data.property, data.propertyIndex))
				{
					if (visitor(*this, entity, env, &data) == ETraversalBehaviour::AbortWithFailure)
					{
						return ETraversalBehaviour::AbortWithFailure;
					}
				}
			}
		}

		return shouldIterateOnNestedEntities(*entity) ? ETraversalBehaviour::Recurse : ETraversalBehaviour::Continue;
	}
	else
	{
		//Entity is nullptr, call the visitor with the nullptr entity and no data
		return visitor(*this, nullptr, env, nullptr);
	}
}

ETraversalBehaviour PropertyCodeGen::generateCode(EntityInfo const* entity, CodeGenEnv& env, std::string& inout_result, void const* data) noexcept
{
	AdditionalData const* additionalData = reinterpret_cast<AdditionalData const*>(data);
	
	if (additionalData != nullptr)
	{
		return generateCode(entity, additionalData->property, additionalData->propertyIndex, env, inout_result) ? ETraversalBehaviour::Recurse : ETraversalBehaviour::AbortWithFailure;
	}
	else
	{
		return generateCode(entity, nullptr, 0u, env, inout_result) ? ETraversalBehaviour::Recurse : ETraversalBehaviour::AbortWithFailure;
	}
}