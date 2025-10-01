// Fill out your copyright notice in the Description page of Project Settings.


#include "VDemoPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "VDemoInputData.h"

void AVDemoPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!IsLocalController())
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	ensureMsgf(Subsystem, TEXT("VDemoPlayerController was unable to get an EnhancedInputLocalPlayerSubsystem"));
	
	if (InputMappingData && InputMappingData->IMC)
	{
		Subsystem->AddMappingContext(InputMappingData->IMC, 0);
	}

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	ensureMsgf(EIC, TEXT("VDemoPlayerController was unable to get an EnhancedInputComponent"));
	
	if (InputMappingData)
	{
		if (InputMappingData->MoveAction)
		{
			EIC->BindAction(InputMappingData->MoveAction, ETriggerEvent::Triggered, this, &ThisClass::OnMove);
		}
		if (InputMappingData->LookAction)
		{
			EIC->BindAction(InputMappingData->LookAction, ETriggerEvent::Triggered, this, &ThisClass::OnLook);
		}
		if (InputMappingData->JumpAction)
		{
			EIC->BindAction(InputMappingData->JumpAction, ETriggerEvent::Triggered, this, &ThisClass::OnJump);
		}
		if (InputMappingData->CrouchAction)
		{
			EIC->BindAction(InputMappingData->CrouchAction, ETriggerEvent::Triggered, this, &ThisClass::OnCrouch);
		}
	}
}

void AVDemoPlayerController::OnLook(const FInputActionValue& Value)
{
	OnInputLook.Broadcast(Value);

	const FVector2D AxisValue = Value.Get<FVector2D>();
	
	AddYawInput(AxisValue.X * 1.0f);
	AddPitchInput(AxisValue.Y * 1.0f); // Temporary simple implementation
}

void AVDemoPlayerController::OnMove(const FInputActionValue& Value)
{
	OnInputMove.Broadcast(Value);
}

void AVDemoPlayerController::OnJump(const FInputActionValue& Value)
{
	OnInputJump.Broadcast(Value);
}

void AVDemoPlayerController::OnCrouch(const FInputActionValue& Value)
{
	OnInputCrouch.Broadcast(Value);
}
