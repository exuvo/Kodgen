/**
*	Copyright (c) 2020 Julien SOYSOUVANH - All Rights Reserved
*
*	This file is part of the Kodgen library project which is released under the MIT License.
*	See the README.md file for full license details.
*/

#pragma once

#include <vector>

#include <clang-c/Index.h>

#include "Kodgen/InfoStructures/StructClassInfo.h"
#include "Kodgen/InfoStructures/EnumInfo.h"
#include "Kodgen/InfoStructures/FunctionInfo.h"
#include "Kodgen/InfoStructures/VariableInfo.h"

namespace kodgen
{
	class NamespaceInfo : public EntityInfo
	{
		public:
			static constexpr EEntityType	nestedEntityTypes = EEntityType::Namespace | EEntityType::Variable | EEntityType::Function | StructClassInfo::nestedEntityTypes;

			/** Nested namespaces. */
			std::vector<NamespaceInfo>		namespaces;

			/** Nested structs. */
			std::vector<StructClassInfo>	structs;

			/** Nested classes. */
			std::vector<StructClassInfo>	classes;

			/** Nested enums. */
			std::vector<EnumInfo>			enums;

			/** Nested functions. */
			std::vector<FunctionInfo>		functions;

			/** Nested variables. */
			std::vector<VariableInfo>		variables;

			NamespaceInfo(CXCursor const&			cursor,
						  std::vector<Property>&&	properties)	noexcept;

			/**
			*	@brief Call a visitor function on a namespace and each nested entity of the provided type(s).
			* 
			*	@param entityMask	All types of entities the visitor function should be called on.
			*	@param visitor		Function to call on entities.
			*/
			template <typename Functor, typename = std::enable_if_t<std::is_invocable_v<Functor, EntityInfo const&>>>
			void foreachEntityOfType(EEntityType entityMask, Functor visitor) const noexcept
			{
				assert(entityType == EEntityType::Namespace);

				//Call visitor on this namespace if mask matches
				if (entityMask && entityType)
				{
					visitor(*this);
				}

				//Propagate call on nested entities
				if (entityMask && NamespaceInfo::nestedEntityTypes)	//EEntityType::Namespace is already included in NamespaceInfo::nestedEntityTypes
				{
					for (NamespaceInfo const& namespace_ : namespaces)
					{
						namespace_.foreachEntityOfType(entityMask, visitor);
					}
				}

				if (entityMask && StructClassInfo::nestedEntityTypes)	//EEntityType::Class and EEntityType::Struct are already included in StructClassInfo::nestedEntityTypes
				{
					for (StructClassInfo const& class_ : classes)
					{
						class_.foreachEntityOfType(entityMask, visitor);
					}

					for (StructClassInfo const& struct_ : structs)
					{
						struct_.foreachEntityOfType(entityMask, visitor);
					}
				}
				
				if ((entityMask && EEntityType::Enum) || (entityMask && EnumInfo::nestedEntityTypes))
				{
					for (EnumInfo const& enum_ : enums)
					{
						enum_.foreachEntityOfType(entityMask, visitor);
					}
				}

				if (entityMask && EEntityType::Function)
				{
					for (FunctionInfo const& function : functions)
					{
						visitor(function);
					}
				}

				if (entityMask && EEntityType::Variable)
				{
					for (VariableInfo const& variable : variables)
					{
						visitor(variable);
					}
				}
			}

			/**
			*	@brief Refresh the outerEntity field of all nested entities. Internal use only.
			*/
			void	refreshOuterEntity()	noexcept;
	};
}