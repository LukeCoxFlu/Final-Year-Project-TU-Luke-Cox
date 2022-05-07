// Fill out your copyright notice in the Description page of Project Settings.

//TO DO - 
//Make free movement move with head and controller
//Make teleportation
//Make Snap turn
//Make flying and climbing



#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "MotionControllerComponent.h"
#include "MovementEnumStruct.h"
#include "grabComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "VrCharacter.generated.h"



UCLASS(Category = "VR_Plugin")
class P_VRINTERFACE_API AVrCharacter : public APawn
{
	GENERATED_BODY()

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VrDefault", meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* characterCapsule = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VrDefault", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* characterCamera = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VrDefault", meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* leftController = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VrDefault", meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* rightController = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VrDefault", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* leftHand = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VrDefault", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* rightHand = nullptr;

	FVector leftHandOffset = FVector::ZeroVector;
	FVector rightHandOffset = FVector::ZeroVector;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VrDefault", meta = (AllowPrivateAccess = "true"))
	float characterHeight = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VrDefault", meta = (AllowPrivateAccess = "true"))
	float characterWidth = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VrMovement", meta = (AllowPrivateAccess = "true"))
	float movementSpeed = 400.0f;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turning", meta = (AllowPrivateAccess = "true"))
	vrTurningType turningType = vrTurningType::NONE;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turning", meta = (AllowPrivateAccess = "true"))
	float smoothTurnSpeed = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turning", meta = (AllowPrivateAccess = "true"))
	float snapTurnAmount = 45;

	//cool down on snap turn, in seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Turning", meta = (AllowPrivateAccess = "true"), meta = (ToolTip = "Snap turn cooldown in seconds"))
	float snapTurnCooldown = 0.5f;
	bool hasTurned = false;
	float enlapsedTimeSinceLastTurn = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VrAutoMovement", meta = (AllowPrivateAccess = "true"), meta = (ToolTip = "Automatic movement type, set to none to turn off"))
	vrMovementType currentAutoMovementType = vrMovementType::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VrAutoMovement", meta = (AllowPrivateAccess = "true"), meta = (ToolTip = "Key Binding Names for each peice of automatic functionality"))
	FVRBindables vrBindings;

	//Choose which hand to draw the tp and movement forward vector i.e. the domant movement hand
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VrAutoMovement", meta = (AllowPrivateAccess = "true"), meta = (ToolTip = "Which hand will movement be directed from"))
	vrMovementHandedness movementHand = vrMovementHandedness::LEFT;

	//Arc range and other things to do with teleporting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Teleporting", meta = (AllowPrivateAccess = "true"), meta = (ToolTip = "Teleport Arc variables"))
	FArcProfile m_arcProfile;

	bool teleportButtonHeld = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab", meta = (AllowPrivateAccess = "true"))
	bool canGrab = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab", meta = (AllowPrivateAccess = "true"))
	bool grabToggle = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab", meta = (AllowPrivateAccess = "true"), meta = (ToolTip = "The radius around the hand at which things can be grabbed"))
	float grabRadius = 8.0f;

	UgrabComponent* leftGrab = nullptr;
	UgrabComponent* rightGrab = nullptr;

	bool hasGrabbedL;
	bool hasGrabbedR;

	//Currently grabbing item in the left hand
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab", meta = (AllowPrivateAccess = "true"))
	AActor* grabbedActorLeft = nullptr;

	//Currently grabbing item in the right hand
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab", meta = (AllowPrivateAccess = "true"))
	AActor* grabbedActorRight = nullptr;

	//Grabbing to climb
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climbing", meta = (AllowPrivateAccess = "true"))
	bool canClimb = false;

	bool leftControllerClimbing = false;
	bool rightControllerClimbing = false;

	FVector leftControllerOldLocation;
	FVector rightControllerOldLocation;

	//Flying
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying", meta = (AllowPrivateAccess = "true"))
	bool canFly = false;

	bool currentlyGlding = false;

	//The Distance between each controller that will activate gliding
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying", meta = (AllowPrivateAccess = "true"))
	int activationDistance = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying", meta = (AllowPrivateAccess = "true"), meta = (ToolTip = "How far off horizontal do you have to hold your arms to fly"))
	float horizontalTolerance = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying", meta = (AllowPrivateAccess = "true"), meta = (ToolTip = "How fast do you fall while gliding"))
	float fallSpeed = -0.4;

public:
	// Sets default values for this pawn's properties
	AVrCharacter();

	UFUNCTION(BlueprintCallable, Category = "Player Height")
	void setHeight();


	UFUNCTION(BlueprintCallable, Category = "Player Height")
	void setNewHeight(float height);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void VrSmoothMovement(float xAxis, float yAxis, float DeltaTime, bool useControllerForward);

	void nonBPTurn(float AxisValue);

	UFUNCTION(BlueprintCallable, Category = "Turning", meta = (ToolTip = "Axis dictate direction of turning"))
	void Turn(float AxisValue, vrTurningType _turningType);

	UFUNCTION(BlueprintCallable, Category = "Misc")
	FRotator getRotationOfController(FName controllerName);

	UFUNCTION(BlueprintCallable, Category = "Misc")
	FVector getLocationOfController(FName controllerName);

	UFUNCTION(BlueprintCallable, Category = "Teleporting", meta = (ToolTip = "Arc line trace for use in teleportation"))
	bool arcLineTrace(int resolution, FVector initialLocation, FVector initialVelocity, FVector acceleration, FHitResult& hitResult, bool isDebug, float debugLinetime);
	FVector NewtonianCurve(FVector InitialPos, FVector InitialVelocity, FVector Force, int T);

	UFUNCTION(BlueprintCallable, Category = "Teleporting")
	void teleport(FArcProfile profile, bool isDebug);
	UFUNCTION(BlueprintCallable, Category = "Teleporting", meta = (ToolTip = "teleport display preview of next teleport"))
	void teleportHold();

	void teleportEvent();
	void teleportPreview(FArcProfile profile, bool isDebug, float deltaTime);

	//Events for auto grabbing
	void leftHandGrab();
	void rightHandGrab();
	void leftHandRelease();
	void rightHandRelease();

	UFUNCTION(BlueprintCallable, Category = "Grabbing")
	bool grab(vrMovementHandedness handed);
	UFUNCTION(BlueprintCallable, Category = "Grabbing")
	void release(vrMovementHandedness handed);

	void glide();
	void stopGliding();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;




};
