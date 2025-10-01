// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/VortexInputDataTypes.h"

void FVortexInputCmd::SetMoveInput(const FVector& InMoveInput)
{
	// like the examples, limit the precision that we store, so that it matches what is NetSerialized (2 decimal place of precision).
	MoveInput.X = FMath::RoundToFloat(InMoveInput.X * 100.0) / 100.0;
	MoveInput.Y = FMath::RoundToFloat(InMoveInput.Y * 100.0) / 100.0;
	MoveInput.Z = FMath::RoundToFloat(InMoveInput.Z * 100.0) / 100.0;
}

FMoverDataStructBase* FVortexInputCmd::Clone() const
{
	FVortexInputCmd* CopyPtr = new FVortexInputCmd(*this);
	return CopyPtr;
}

bool FVortexInputCmd::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	SerializePackedVector<100, 30>(MoveInput, Ar); // Changes to this also need to be reflected in SetMoveInput
	SerializeFixedVector<1, 16>(OrientationInput, Ar);
	ControlRotation.SerializeCompressedShort(Ar);

	Ar.SerializeBits(&bJumpPressed, 1);
	Ar.SerializeBits(&bJumpJustPressed, 1);
	Ar.SerializeBits(&bCrouchPressed, 1);

	bOutSuccess = true;
	return true;
}

void FVortexInputCmd::ToString(FAnsiStringBuilderBase& Out) const
{
	Super::ToString(Out);

	Out.Appendf("MoveInput: X=%.2f Y=%.2f Z=%.2f\n", MoveInput.X, MoveInput.Y, MoveInput.Z);
	Out.Appendf("OrientationInput: X=%.2f Y=%.2f Z=%.2f\n", OrientationInput.X, OrientationInput.Y, OrientationInput.Z);
	Out.Appendf("ControlRot: Pitch=%.2f Yaw=%.2f Roll=%.2f\n", ControlRotation.Pitch, ControlRotation.Yaw, ControlRotation.Roll);
	Out.Appendf("bJumpPressed: %d bJumpJustPressed: %d bCrouchPressed: %d\n", bJumpPressed ? 1 : 0, bJumpJustPressed ? 1 : 0, bCrouchPressed ? 1 : 0);
}

bool FVortexInputCmd::ShouldReconcile(const FMoverDataStructBase& AuthorityState) const
{
	const FVortexInputCmd& TypedAuthority = static_cast<const FVortexInputCmd&>(AuthorityState);
	return *this != TypedAuthority;
}

void FVortexInputCmd::Interpolate(const FMoverDataStructBase& From, const FMoverDataStructBase& To, float Pct)
{
	const FVortexInputCmd* FromState = static_cast<const FVortexInputCmd*>(&From);
	const FVortexInputCmd* ToState = static_cast<const FVortexInputCmd*>(&To);

	const FVortexInputCmd& ClosestInputs = Pct < 0.5f ? *FromState : *ToState;
	bJumpJustPressed = ClosestInputs.bJumpJustPressed;
	bJumpPressed = ClosestInputs.bJumpPressed;
	bCrouchPressed = ClosestInputs.bCrouchPressed;

	SetMoveInput(FMath::Lerp(FromState->GetMoveInput(), ToState->GetMoveInput(), Pct));
	OrientationInput = FMath::Lerp(FromState->OrientationInput, ToState->OrientationInput, Pct);
	ControlRotation = FMath::Lerp(FromState->ControlRotation, ToState->ControlRotation, Pct);
}

void FVortexInputCmd::Merge(const FMoverDataStructBase& From)
{
	const FVortexInputCmd& TypedFrom = static_cast<const FVortexInputCmd&>(From);
	bJumpJustPressed |= TypedFrom.bJumpJustPressed;
	bJumpPressed |= TypedFrom.bJumpPressed;
	bCrouchPressed |= TypedFrom.bCrouchPressed;
}

void FVortexInputCmd::Decay(float DecayAmount)
{
	DecayAmount *= 0.25f; // TODO: make this configurable or a cvar

	MoveInput *= (1.0f - DecayAmount);

	// Single use inputs
	bJumpJustPressed = FMath::IsNearlyZero(DecayAmount) ? bJumpJustPressed : false;
}
