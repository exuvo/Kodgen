#include "Kodgen/CodeGen/PropertyCodeGen.h"

using namespace kodgen;

PropertyCodeGen::PropertyCodeGen(EEntityType eligibleEntityMask) noexcept:
	_eligibleEntityMask{eligibleEntityMask}
{
}

bool PropertyCodeGen::initialize(CodeGenEnv& /* env */) noexcept
{
	return true;
}

bool PropertyCodeGen::shouldIterateOnNestedEntities(EntityInfo const& entity) const noexcept
{
	switch (entity.entityType)
	{
		case EEntityType::Namespace:
			return entityTypeOverlap(NamespaceInfo::nestedEntityTypes, _eligibleEntityMask);

		case EEntityType::Struct:
			[[fallthrough]];
		case EEntityType::Class:
			return entityTypeOverlap(StructClassInfo::nestedEntityTypes, _eligibleEntityMask);

		case EEntityType::Enum:
			return entityTypeOverlap(EnumInfo::nestedEntityTypes, _eligibleEntityMask);

		default:
			//EnumValue
			//Functions
			//Methods
			//Fields
			//Variables
			return false;
	}
}

ETraversalBehaviour PropertyCodeGen::generateCode(EntityInfo const& entity, CodeGenEnv& env, std::function<ETraversalBehaviour(ICodeGenerator&, EntityInfo const&, CodeGenEnv&, void const*)> visitor) noexcept
{
	assert(visitor != nullptr);

	//Call the visitor if the entity type is contained in the _eligibleEntities mask
	if (entityTypeOverlap(_eligibleEntityMask, entity.entityType))
	{
		AdditionalData data;

		//Execute the visitor on each property contained in the entity
		for (uint8 i = 0; i < entity.propertyGroup.properties.size(); i++)
		{
			data.propertyIndex = i;
			data.property = &entity.propertyGroup.properties[i];

			if (shouldGenerateCode(entity, *data.property, data.propertyIndex))
			{
				if (visitor(*this, entity, env, &data) == ETraversalBehaviour::AbortWithFailure)
				{
					return ETraversalBehaviour::AbortWithFailure;
				}
			}
		}
	}

	return shouldIterateOnNestedEntities(entity) ? ETraversalBehaviour::Recurse : ETraversalBehaviour::Continue;
}

ETraversalBehaviour PropertyCodeGen::generateCodeInterface(EntityInfo const& entity, CodeGenEnv& env, std::string& inout_result, void const* data) noexcept
{
	AdditionalData const* additionalData = reinterpret_cast<AdditionalData const*>(data);

	return generateCode(entity, *additionalData->property, additionalData->propertyIndex, env, inout_result) ? ETraversalBehaviour::Recurse : ETraversalBehaviour::AbortWithFailure;
}