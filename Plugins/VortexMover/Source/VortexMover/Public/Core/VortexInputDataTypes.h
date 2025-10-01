// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MoverTypes.h"
#include "VortexInputDataTypes.generated.h"

/**
 * 
 */
USTRUCT()
struct FVortexInputCmd : public FMoverDataStructBase
{
    GENERATED_BODY()
public:
    void SetMoveInput(const FVector& InMoveInput);
    const FVector& GetMoveInput() const { return MoveInput; }

protected:
    // Movement input in X-Y plane
    FVector MoveInput;
    
public:
    // Look/camera rotation input
    FVector OrientationInput;

    FRotator ControlRotation;

    // Jump inputs
    bool bJumpPressed;
    bool bJumpJustPressed;
    
    // Crouch input
    bool bCrouchPressed;

    FVortexInputCmd()
        : MoveInput(ForceInitToZero)
        , OrientationInput(ForceInitToZero)
        , ControlRotation(ForceInitToZero)
        , bJumpPressed(false)
        , bJumpJustPressed(false)
        , bCrouchPressed(false)
    {
    }
    virtual ~FVortexInputCmd() {}

    bool operator==(const FVortexInputCmd& Other) const
    {
        return MoveInput == Other.MoveInput
            && OrientationInput == Other.OrientationInput
            && ControlRotation == Other.ControlRotation
            && bJumpPressed == Other.bJumpPressed
            && bJumpJustPressed == Other.bJumpJustPressed
            && bCrouchPressed == Other.bCrouchPressed;
    }

    bool operator!=(const FVortexInputCmd& Other) const { return !(*this == Other); }

    virtual FMoverDataStructBase* Clone() const override;
    virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;
    virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct(); }
    virtual void ToString(FAnsiStringBuilderBase& Out) const override;
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override { Super::AddReferencedObjects(Collector); }
    virtual bool ShouldReconcile(const FMoverDataStructBase& AuthorityState) const override;
    virtual void Interpolate(const FMoverDataStructBase& From, const FMoverDataStructBase& To, float Pct) override;
    virtual void Merge(const FMoverDataStructBase& From) override;
    virtual void Decay(float DecayAmount) override;
};

template<>
struct TStructOpsTypeTraits<FVortexInputCmd> : public TStructOpsTypeTraitsBase2<FVortexInputCmd>
{
    enum
    {
        WithNetSerializer = true,
        WithCopy = true
    };
};