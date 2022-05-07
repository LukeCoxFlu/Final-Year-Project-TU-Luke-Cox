// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Engine/Engine.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MovementEnumStruct.generated.h"



UENUM(BlueprintType, Category = "VR_Plugin")
enum class vrMovementType : uint8 {
	STATIC = 0 UMETA(DisplayName = "Static"),
	FREEMOVEHEAD = 1 UMETA(DisplayName = "Free Movement Head"),
	FREEMOVECTRL = 2 UMETA(DisplayName = "Free Movement Controller"),
	TELEPORTING = 3 UMETA(DisplayName = "Teleporting"),
	NONE = 4 UMETA(DisplayName = "Not Auto")
};

UENUM(BlueprintType, Category = "VR_Plugin")
enum class vrMovementHandedness : uint8 {
	LEFT = 0 UMETA(DisplayName = "Left"),
	RIGHT = 1 UMETA(DisplayName = "Right")
};

UENUM(BlueprintType, Category = "VR_Plugin")
enum class vrTurningType : uint8 {
	NONE = 0 UMETA(DisplayName = "No Manuel Turning Type"),
	SMOOTH = 1 UMETA(DisplayName = "Smooth Turn"),
	SNAP = 2 UMETA(DisplayName = "Snap Turn")
};

USTRUCT(BlueprintType, meta = (ToolTip = "Teleport Arc properties"))
struct FArcProfile
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arc Profile" , meta = (ToolTip = "How many line traces are done per arc"))
	int resolutionAndMaxTime = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arc Profile" , meta = (ToolTip = "How far do they go"))
	float speed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arc Profile", meta = (ToolTip = "How far does each one pull down"))
	float gravity = -5.0f;
};
//Used to set the binded events when using the automated movement and grab functionality
USTRUCT(BlueprintType, Category = "VR_Plugin")
struct FVRBindables
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Buttons")
	FName MovementXAxis = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Buttons")
	FName MovementYAxis = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Buttons")
	FName TurnAxis = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Buttons")
	FName Teleport = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Buttons")
	FName LGrab = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Buttons")
	FName RGrab = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Buttons")
	FName LeftInteract = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Action Buttons")
	FName RightInteract = "";
};