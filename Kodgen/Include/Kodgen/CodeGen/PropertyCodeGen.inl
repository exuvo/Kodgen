/**
*	Copyright (c) 2021 Julien SOYSOUVANH - All Rights Reserved
*
*	This file is part of the Kodgen library project which is released under the MIT License.
*	See the README.md file for full license details.
*/

inline bool PropertyCodeGen::entityTypeOverlap(EEntityType lhs, EEntityType rhs) noexcept
{
	return (lhs & rhs) != EEntityType::Undefined;
}

inline EEntityType PropertyCodeGen::getEligibleEntityMask() const noexcept
{
	return _eligibleEntityMask;
}

inline std::string const& PropertyCodeGen::getPropertyName() const noexcept
{
	return _propertyName;
}