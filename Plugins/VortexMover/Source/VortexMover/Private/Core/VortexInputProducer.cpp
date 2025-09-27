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
	// Only produce for locally-controlled pawns with a controller (client-side).
	if (!OwnerPawn.IsValid()|| !OwnerPawn.Get()->GetController() || !OwnerPawn->IsLocallyControlled())
	{
		return;
	}
	
	FVortexInputCmd& Cmd = InputCmdResult.InputCollection.FindOrAddMutableDataByType<FVortexInputCmd>();
	
	Cmd.MoveInput = CachedMove;
	Cmd.LookInput = CachedLook;
	Cmd.bJumpPressed = bJumpPressed;
	Cmd.bJumpJustPressed = bJumpJustPressed;
	Cmd.bCrouchPressed = bCrouchPressed;

	const int32 DebugLevel = VortexMoverCVars::IsInputDebugEnabled();
	if (DebugLevel >= 2) {LogOnChange(); }
	else if (DebugLevel >= 1) { LogPerFrame(); }

	bJumpJustPressed = false;
}

void UVortexInputProducer::OnMove(const FInputActionValue& Value)
{
	CachedMove = Value.Get<FVector2D>();
	CachedMove = CachedMove.ClampAxes(-1.0f, 1.0f);
}

void UVortexInputProducer::OnLook(const FInputActionValue& Value)
{
	CachedLook = Value.Get<FVector2D>();
	CachedLook = CachedLook.ClampAxes(-1.0f, 1.0f);
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
	CachedMove = FVector2D::ZeroVector;
	CachedLook = FVector2D::ZeroVector;
	bJumpPressed = false;
	bJumpJustPressed = false;
	bCrouchPressed = false;

	PrevMove = FVector2D::ZeroVector;
	PrevLook = FVector2D::ZeroVector;
	bPrevJumpPressed = false;
	bPrevCrouchPressed = false;
}

void UVortexInputProducer::LogPerFrame() const
{
	UE_LOG(LogVortexMoverInput, Log, TEXT("[Produce] Move(%.2f,%.2f) Look(%.2f,%.2f) Jump(P:%d JP:%d) Crouch(P:%d)"),
	CachedMove.X, CachedMove.Y,
	CachedLook.X, CachedLook.Y,
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
		UE_LOG(LogVortexMoverInput, Log, TEXT("[Change] Look -> (%.3f, %.3f)"), CachedLook.X, CachedLook.Y);
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
