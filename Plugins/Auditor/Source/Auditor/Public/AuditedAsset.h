#pragma once

UENUM(BlueprintType)
enum class EAuditedAsset : uint8
{
	None = 0,
	Texture,
	Material,
	MaterialInstance,
	PhysicsAsset,
	PhysicalMaterial,
	SkeletalMesh,
	StaticMesh,
	Blueprint,
	Widget,
	ActorComponent,
	AnimationBlueprint,
	BlueprintInterface,
	CurveTable,
	DataTable,
	Enum,
	Structure,
	NiagaraEmitter,
	NiagaraSystem,
	NiagaraFunction,
	Rig,
	Skeleton,
	AnimationMontage,
	AnimationSequence,
	BlendSpace,
	LevelSequence
};