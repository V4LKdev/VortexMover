// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MoverTypes.h"
#include "VortexInputDataTypes.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FVortexInputCmd : public FMoverDataStructBase
{
    GENERATED_BODY()

    // Movement input in X-Y plane
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Vortex|Input")
    FVector2D MoveInput = FVector2D::ZeroVector;

    // Look/camera rotation input
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Vortex|Input")
    FVector2D LookInput = FVector2D::ZeroVector;

    // Jump inputs
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Vortex|Input")
    bool bJumpPressed = false;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Vortex|Input")
    bool bJumpJustPressed = false;
    
    // Crouch input
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Vortex|Input")
    bool bCrouchPressed = false;


    virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override
    {
        bOutSuccess = true;
        
        // Quantize Vector2D inputs to 8-bit signed integers (-127 to 127)
        if (Ar.IsSaving())
        {
            // Clamp inputs to valid range [-1.0, 1.0] and quantize
            int8 MoveX = FMath::Clamp(FMath::RoundToInt(MoveInput.X * 127.0f), -127, 127);
            int8 MoveY = FMath::Clamp(FMath::RoundToInt(MoveInput.Y * 127.0f), -127, 127);
            int8 LookX = FMath::Clamp(FMath::RoundToInt(LookInput.X * 127.0f), -127, 127);
            int8 LookY = FMath::Clamp(FMath::RoundToInt(LookInput.Y * 127.0f), -127, 127);
            
            Ar << MoveX;
            Ar << MoveY;
            Ar << LookX;
            Ar << LookY;
            
            // Pack boolean flags into a single byte for efficiency
            uint8 PackedBools = 0;
            PackedBools |= bJumpPressed ? (1 << 0) : 0;
            PackedBools |= bJumpJustPressed ? (1 << 1) : 0;
            PackedBools |= bCrouchPressed ? (1 << 2) : 0;
            
            Ar << PackedBools;
        }
        else
        {
            // Reading data
            int8 MoveX, MoveY, LookX, LookY;
            Ar << MoveX;
            Ar << MoveY;
            Ar << LookX;
            Ar << LookY;
            
            // Convert back to floating point normalized values
            MoveInput.X = FMath::Clamp(MoveX / 127.0f, -1.0f, 1.0f);
            MoveInput.Y = FMath::Clamp(MoveY / 127.0f, -1.0f, 1.0f);
            LookInput.X = FMath::Clamp(LookX / 127.0f, -1.0f, 1.0f);
            LookInput.Y = FMath::Clamp(LookY / 127.0f, -1.0f, 1.0f);
            
            // Unpack boolean flags
            uint8 PackedBools;
            Ar << PackedBools;
            
            bJumpPressed = (PackedBools & (1 << 0)) != 0;
            bJumpJustPressed = (PackedBools & (1 << 1)) != 0;
            bCrouchPressed = (PackedBools & (1 << 2)) != 0;
        }
        
        return true;
    }
    
    virtual FMoverDataStructBase* Clone() const override
    {
        return new FVortexInputCmd(*this);
    }
    
    virtual UScriptStruct* GetScriptStruct() const override
    {
        return StaticStruct();
    }
    
    virtual void ToString(FAnsiStringBuilderBase& Out) const override
    {
        Out.Appendf("MoveInput:(%f,%f) LookInput:(%f,%f) Jump:%s JustJump:%s Crouch:%s",
            MoveInput.X, MoveInput.Y,
            LookInput.X, LookInput.Y,
            bJumpPressed ? "true" : "false",
            bJumpJustPressed ? "true" : "false",
            bCrouchPressed ? "true" : "false");
    }


    virtual bool ShouldReconcile(const FMoverDataStructBase& AuthorityState) const override
    {
        // Cast to specific type
        const FVortexInputCmd& OtherState = static_cast<const FVortexInputCmd&>(AuthorityState);
    
        // Check if inputs differ significantly
        if (FMath::Abs(MoveInput.X - OtherState.MoveInput.X) > 0.01f ||
            FMath::Abs(MoveInput.Y - OtherState.MoveInput.Y) > 0.01f ||
            bJumpPressed != OtherState.bJumpPressed ||
            bJumpJustPressed != OtherState.bJumpJustPressed ||
            bCrouchPressed != OtherState.bCrouchPressed)
        {
            return true;
        }
    
        // Look input doesn't need reconciliation as it's visual only
        return false;
    }

    virtual void Merge(const FMoverDataStructBase& From) override
    {
        // For inputs, we typically want to take the most recent values
        // This is called when merging input from previous frames
        const FVortexInputCmd& OtherInput = static_cast<const FVortexInputCmd&>(From);
    
        // Only merge non-zero inputs to preserve intent
        if (!MoveInput.IsZero())
        {
            // Keep current input if it's non-zero
        }
        else
        {
            MoveInput = OtherInput.MoveInput;
        }
    
        // For boolean inputs, we might want to track edge cases
        if (!bJumpJustPressed)
        {
            bJumpJustPressed = OtherInput.bJumpJustPressed;
        }
    }
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