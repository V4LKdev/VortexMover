// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "VDemoPawn.generated.h"

class AVDemoPlayerController;
class UCapsuleComponent;
class UVortexMoverComponent;
class UVortexInputProducer;
class UCharacterMoverComponent;

/**
 * AVDemoPawn
 *
 * -Uses VortexMoverComponent for movement
 * -Uses VortexInputProducer to produce input from its controller
 */
UCLASS()
class VORTEXMOVERDEMO_API AVDemoPawn : public APawn
{
	GENERATED_BODY()

public:
	AVDemoPawn();
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostInitializeComponents() override;
	virtual void OnRep_Controller() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCapsuleComponent> Capsule;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UVortexMoverComponent> MoverComponent;
	// Instanced Input Producer UObject that caches input on the game thread for the MoverComponent to consume on demand
	UPROPERTY(VisibleAnywhere, Instanced)
	TObjectPtr<UVortexInputProducer> InputProducer;

private:
	void BindControllerInput();
	
	FDelegateHandle LookInputDelegateHandle;
	FDelegateHandle MoveInputDelegateHandle;
	FDelegateHandle JumpInputDelegateHandle;
	FDelegateHandle CrouchInputDelegateHandle;
	TWeakObjectPtr<AVDemoPlayerController> DemoController;
	void ClearInputHandles();
};
