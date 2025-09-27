// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MoverSimulationTypes.h"
#include "UObject/Object.h"
#include "VortexInputProducer.generated.h"

struct FInputActionValue;

/**
 * UVortexInputProducer
 *
 * -Instanced UObject owned by a Pawn that caches input on the game thread for a MoverComponent to consume on demand
 * -Routed by the Pawn from its controller delegates
 * -Implements IMoverInputProducerInterface to produce input for the MoverComponent
 * -Thread safety: game thread only except ProduceInput which may be called on sim thread
 * -Replication: produce input only for locally controlled pawns
 */
UCLASS(EditInlineNew, DefaultToInstanced, CollapseCategories, meta = (DisplayName = "Vortex Input Producer"))
class VORTEXMOVER_API UVortexInputProducer : public UObject, public IMoverInputProducerInterface
{
	GENERATED_BODY()
public:
	// Initialize with owning pawn
	void Initialize(APawn* InOwnerPawn);

	// Mover pull: copy cached state into the input command collection.
	virtual void ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& InputCmdResult) override;

	// Routed by the Pawn from its controller delegates
	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	void OnJump(const FInputActionValue& Value);
	void OnCrouch(const FInputActionValue& Value);

	// Clear all cached/transient input (e.g., on unpossess)
	void ResetCachedInput();

private:
	TWeakObjectPtr<APawn> OwnerPawn;
	
	// Cached state (game thread)
	FVector2D CachedMove = FVector2D::ZeroVector;
	FVector2D CachedLook = FVector2D::ZeroVector;
	bool bJumpPressed = false;
	bool bJumpJustPressed = false;
	bool bCrouchPressed = false;

	// Previous values for change logging
	FVector2D PrevMove = FVector2D::ZeroVector;
	FVector2D PrevLook = FVector2D::ZeroVector;
	bool bPrevJumpPressed = false;
	bool bPrevCrouchPressed = false;

	// Debug
	void LogPerFrame() const;
	void LogOnChange();
};
