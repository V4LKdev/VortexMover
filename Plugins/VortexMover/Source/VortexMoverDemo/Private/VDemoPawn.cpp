// Fill out your copyright notice in the Description page of Project Settings.


#include "VDemoPawn.h"

#include "VDemoPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Core/VortexInputProducer.h"
#include "Core/VortexMoverComponent.h"
#include "Mover/Public/DefaultMovementSet/CharacterMoverComponent.h"

AVDemoPawn::AVDemoPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	RootComponent = Capsule;
	Capsule->SetCollisionProfileName(TEXT("Pawn"));
	Capsule->SetCapsuleHalfHeight(88.f);
	Capsule->SetCapsuleRadius(34.f);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(TEXT("NoCollision"));

	MoverComponent = CreateDefaultSubobject<UVortexMoverComponent>(TEXT("VortexMoverComponent"));
	InputProducer = CreateDefaultSubobject<UVortexInputProducer>(TEXT("VortexInputProducer"));
	
	bReplicates = true;
	SetReplicatingMovement(false);
}

void AVDemoPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ensureMsgf(MoverComponent && Capsule && MeshComponent, TEXT("VDemoPawn is missing required components"));
	MoverComponent->SetUpdatedComponent(Capsule);
	MoverComponent->SetPrimaryVisualComponent(MeshComponent);
	MoverComponent->SmoothingMode = EMoverSmoothingMode::VisualComponentOffset;

	ensureMsgf(InputProducer, TEXT("VDemoPawn is missing InputProducer"));
	MoverComponent->InputProducer = InputProducer;
	InputProducer->Initialize(this);
}

void AVDemoPawn::OnRep_Controller()
{
	Super::OnRep_Controller();

	BindControllerInput();
}

void AVDemoPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	BindControllerInput();
}

void AVDemoPawn::BindControllerInput()
{
	ClearInputHandles();

	if (AVDemoPlayerController* PC = Cast<AVDemoPlayerController>(GetController()))
	{
		DemoController = PC;

		LookInputDelegateHandle = PC->OnInputLook.AddWeakLambda(this, [this](const FInputActionValue& Value)
		{
			if (IsValid(InputProducer))
			{
				InputProducer->OnLook(Value);
			}
		});

		MoveInputDelegateHandle = PC->OnInputMove.AddWeakLambda(this, [this](const FInputActionValue& Value)
		{
			if (IsValid(InputProducer))
			{
				InputProducer->OnMove(Value);
			}
		});

		JumpInputDelegateHandle = PC->OnInputJump.AddWeakLambda(this, [this](const FInputActionValue& Value)
		{
			if (IsValid(InputProducer))
			{
				InputProducer->OnJump(Value);
			}
		});

		CrouchInputDelegateHandle = PC->OnInputCrouch.AddWeakLambda(this, [this](const FInputActionValue& Value)
		{
			if (IsValid(InputProducer))
			{
				InputProducer->OnCrouch(Value);
			}
		});
	}
}

void AVDemoPawn::BeginPlay()
{
	Super::BeginPlay();
}


void AVDemoPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AVDemoPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearInputHandles();

	Super::EndPlay(EndPlayReason);
}

void AVDemoPawn::UnPossessed()
{
	ClearInputHandles();

	Super::UnPossessed();
}

void AVDemoPawn::ClearInputHandles()
{
	if (DemoController.IsValid())
	{
		DemoController->OnInputLook.Remove(LookInputDelegateHandle);
		DemoController->OnInputMove.Remove(MoveInputDelegateHandle);
		DemoController->OnInputJump.Remove(JumpInputDelegateHandle);
		DemoController->OnInputCrouch.Remove(CrouchInputDelegateHandle);
		DemoController = nullptr;
	}

	LookInputDelegateHandle.Reset();
	MoveInputDelegateHandle.Reset();
	JumpInputDelegateHandle.Reset();
	CrouchInputDelegateHandle.Reset();

	if (IsValid(InputProducer))
	{
		InputProducer->ResetCachedInput();
	}
}
