#pragma once

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ENamingConventionTestResult : uint8
{
	None	= 0b00000000, //0
	Prefix	= 0b00000001, //1
	Suffix	= 0b00000010, //2

	Conforms = Prefix + Suffix //3
};
ENUM_CLASS_FLAGS(ENamingConventionTestResult);