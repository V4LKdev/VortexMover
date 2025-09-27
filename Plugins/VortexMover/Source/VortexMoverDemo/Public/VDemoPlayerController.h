// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VDemoPlayerController.generated.h"

class UVDemoInputData;
struct FInputActionValue;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInputLookSignature, const FInputActionValue& /*Value*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInputMoveSignature, const FInputActionValue& /*Value*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInputJumpSignature, const FInputActionValue& /*Value*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInputCrouchSignature, const FInputActionValue& /*Value*/);

/**
 * AVDemoPlayerController
 *
 * -Owns input mapping context and action bindings
 * -Broadcasts input action events for the possessed pawn to consume
 */
UCLASS()
class VORTEXMOVERDEMO_API AVDemoPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	
	FOnInputLookSignature OnInputLook;
	FOnInputMoveSignature OnInputMove;
	FOnInputJumpSignature OnInputJump;
	FOnInputCrouchSignature OnInputCrouch;
	
protected:
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UVDemoInputData> InputMappingData;

private:
	UFUNCTION()
	void OnLook(const FInputActionValue& Value);
	UFUNCTION()
	void OnMove(const FInputActionValue& Value);
	UFUNCTION()
	void OnJump(const FInputActionValue& Value);
	UFUNCTION()
	void OnCrouch(const FInputActionValue& Value);
};
