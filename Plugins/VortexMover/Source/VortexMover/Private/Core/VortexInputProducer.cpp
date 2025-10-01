// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/VortexInputProducer.h"

#include "InputActionValue.h"
#include "VortexMoverCVars.h"
#include "VortexMoverLogChannels.h"
#include "Core/VortexInputDataTypes.h"

void UVortexInputProducer::Initialize(APawn* InOwnerPawn)
{
	ensure(InOwnerPawn);
	OwnerPawn = InOwnerPawn;
	ResetCachedInput();
}

void UVortexInputProducer::ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& InputCmdResult)
{
	FVortexInputCmd& Cmd = InputCmdResult.InputCollection.FindOrAddMutableDataByType<FVortexInputCmd>();
	
	// Only produce for locally-controlled pawns with a controller (client-side).
	if (!OwnerPawn.IsValid() || !OwnerPawn->IsLocallyControlled())
	{
		static const FVortexInputCmd EmptyInput;
		Cmd = EmptyInput;
		return;
	}
	
	Cmd.ControlRotation = OwnerPawn->GetControlRotation();

	const FVector FinalDirectionalIntent = Cmd.ControlRotation.RotateVector(CachedMove);
	Cmd.SetMoveInput(FinalDirectionalIntent);

	// TODO: this is facing intent, not input, and the LookInput cached value is actually not being used here in this class at all, could remove later
	Cmd.OrientationInput = Cmd.ControlRotation.Vector().GetSafeNormal();
	
	Cmd.bJumpPressed = bJumpPressed;
	Cmd.bJumpJustPressed = bJumpJustPressed;
	Cmd.bCrouchPressed = bCrouchPressed;

	
	const int32 DebugLevel = VortexMoverCVars::IsInputDebugEnabled();
	if (DebugLevel >= 2) {LogOnChange(); }
	else if (DebugLevel >= 1) { LogPerFrame(); }

	// Clear single-use inputs
	bJumpJustPressed = false;
}

void UVortexInputProducer::OnMove(const FInputActionValue& Value)
{
	const FVector MovementVector = Value.Get<FVector>();
	CachedMove.X = FMath::Clamp(MovementVector.X, -1.0f, 1.0f);
	CachedMove.Y = FMath::Clamp(MovementVector.Y, -1.0f, 1.0f);
	CachedMove.Z = FMath::Clamp(MovementVector.Z, -1.0f, 1.0f);
}

void UVortexInputProducer::OnLook(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();
	CachedLook.Yaw = FMath::Clamp(LookVector.X, -1.0f, 1.0f);
	CachedLook.Pitch = FMath::Clamp(LookVector.Y, -1.0f, 1.0f);
}

void UVortexInputProducer::OnJump(const FInputActionValue& Value)
{
	const bool bNewPressed = Value.Get<bool>();
	bJumpJustPressed = bNewPressed && !bJumpPressed;
	bJumpPressed = bNewPressed;
}

void UVortexInputProducer::OnCrouch(const FInputActionValue& Value)
{
	bCrouchPressed = Value.Get<bool>();
}

void UVortexInputProducer::ResetCachedInput()
{
	CachedMove = FVector::ZeroVector;
	CachedLook = FRotator::ZeroRotator;
	bJumpPressed = false;
	bJumpJustPressed = false;
	bCrouchPressed = false;

	PrevMove = FVector::ZeroVector;
	PrevLook = FRotator::ZeroRotator;
	bPrevJumpPressed = false;
	bPrevCrouchPressed = false;
}

void UVortexInputProducer::LogPerFrame() const
{
	UE_LOG(LogVortexMoverInput, Log, TEXT("[Produce] Move(%.2f,%.2f) Look(%.2f,%.2f) Jump(P:%d JP:%d) Crouch(P:%d)"),
	CachedMove.X, CachedMove.Y,
	CachedLook.Pitch, CachedLook.Yaw,
	bJumpPressed ? 1 : 0,
	bJumpJustPressed ? 1 : 0,
	bCrouchPressed ? 1 : 0);
}

void UVortexInputProducer::LogOnChange()
{
	const bool bMoveChanged = !CachedMove.Equals(PrevMove, KINDA_SMALL_NUMBER);
	const bool bLookChanged = !CachedLook.Equals(PrevLook, KINDA_SMALL_NUMBER);

	if (bMoveChanged)
	{
		UE_LOG(LogVortexMoverInput, Log, TEXT("[Change] Move -> (%.3f, %.3f)"), CachedMove.X, CachedMove.Y);
	}
	if (bLookChanged)
	{
		UE_LOG(LogVortexMoverInput, Log, TEXT("[Change] Look -> (%.3f, %.3f)"), CachedLook.Pitch, CachedLook.Yaw);
	}
	if (bJumpPressed != bPrevJumpPressed)
	{
		UE_LOG(LogVortexMoverInput, Log, TEXT("[Change] Jump -> %s%s"),
			bJumpPressed ? TEXT("Pressed") : TEXT("Released"),
			bJumpJustPressed ? TEXT(" (JustPressed)") : TEXT(""));
	}
	if (bCrouchPressed != bPrevCrouchPressed)
	{
		UE_LOG(LogVortexMoverInput, Log, TEXT("[Change] Crouch -> %s"), bCrouchPressed ? TEXT("Pressed") : TEXT("Released"));
	}

	PrevMove = CachedMove;
	PrevLook = CachedLook;
	bPrevJumpPressed = bJumpPressed;
	bPrevCrouchPressed = bCrouchPressed;
}
