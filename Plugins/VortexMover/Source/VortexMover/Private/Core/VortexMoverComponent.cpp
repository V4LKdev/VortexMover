// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/VortexMoverComponent.h"

#include "Modes/SimpleWalking.h"

UVortexMoverComponent::UVortexMoverComponent()
{
	MovementModes.Add("Simple_Walking", CreateDefaultSubobject<USimpleWalking>(TEXT("SimpleWalkingMode")));

	StartingMovementMode = "Simple_Walking";
}
