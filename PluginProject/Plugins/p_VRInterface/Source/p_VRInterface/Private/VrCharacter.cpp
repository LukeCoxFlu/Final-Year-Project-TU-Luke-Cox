// Fill out your copyright notice in the Description page of Project Settings.


#include "VrCharacter.h"
#include "MotionControllerComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "grabComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PawnMovementComponent.h"


// Sets default values
AVrCharacter::AVrCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//Initialising the character capsule and the
	characterCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Character Capsule"));
	characterCapsule->InitCapsuleSize(characterWidth, characterHeight);
	SetRootComponent(characterCapsule);
	characterCapsule->SetSimulatePhysics(true);
	characterCapsule->SetAngularDamping(1000000000);
	characterCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Character Camera"));
	characterCamera->SetupAttachment(characterCapsule);

	//Setting up left Controller and hand
	{
		leftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Left Controller"));
		leftController->SetupAttachment(characterCapsule);
		leftController->SetRelativeLocationAndRotation(FVector::ZeroVector, FQuat::Identity);
		leftController->SetRelativeScale3D(FVector::OneVector);
		leftController->MotionSource = "LEFT";
	}

	//Setting Up right controller and hand
	{
		rightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("Right Controller"));
		rightController->SetupAttachment(characterCapsule);
		rightController->SetRelativeLocationAndRotation(FVector::ZeroVector, FQuat::Identity);
		rightController->SetRelativeScale3D(FVector::OneVector);
		rightController->MotionSource = "RIGHT";
	}

	//Initialising hands
	leftHand = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Left Hand Skel Mesh"));
	leftHand->SetupAttachment(leftController);
	leftHand->SetRelativeLocationAndRotation(FVector::ZeroVector, FQuat::Identity);
	leftHand->SetRelativeScale3D(FVector::OneVector);

	rightHand = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Right Hand Skel Mesh"));
	rightHand->SetupAttachment(rightController);
	rightHand->SetRelativeLocationAndRotation(FVector::ZeroVector, FQuat::Identity);
	rightHand->SetRelativeScale3D(FVector::OneVector);
	
	//Setting collisions and constraints
	characterCapsule->SetCollisionProfileName(TEXT("Pawn"));
	characterCapsule->BodyInstance.bLockXRotation = true;
	characterCapsule->BodyInstance.bLockYRotation = true;


}


FRotator AVrCharacter::getRotationOfController(FName controllerName)
{
	if (controllerName == "LEFT") return leftController->GetRelativeRotation();
	else if (controllerName == "RIGHT") return rightController->GetRelativeRotation();
	return FRotator::ZeroRotator;

}

FVector AVrCharacter::getLocationOfController(FName controllerName)
{
	if (controllerName == "LEFT") return leftController->GetComponentLocation();
	else if (controllerName == "RIGHT") return rightController->GetComponentLocation();
	return FVector(0,0,0);
}

FVector AVrCharacter::NewtonianCurve(FVector InitialPos, FVector InitialVelocity, FVector Force, int T)
{
	//Newtonian projectile formula to form the arc of the teleport cast
	return InitialPos + (InitialVelocity * T) + ((Force * T * T) / 2);
}

void AVrCharacter::teleport(FArcProfile profile, bool isDebug)
{
	FHitResult hit;
	FVector initalPosition;
	FVector initalVelocity;

	switch (movementHand)
	{
	case vrMovementHandedness::LEFT:
		initalPosition = leftController->GetComponentLocation();
		initalVelocity = leftController->GetForwardVector();
		break;
	case vrMovementHandedness::RIGHT:
		initalPosition = rightController->GetComponentLocation();
		initalVelocity = rightController->GetForwardVector();
		break;
	default:
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("No Movement Handedness has been set"));
		break;
	}

	initalVelocity = initalVelocity * profile.speed;

	bool didHit = arcLineTrace(profile.resolutionAndMaxTime, initalPosition, initalVelocity, FVector(0, 0, profile.gravity), hit, isDebug, 2.0f);

	if (didHit)
	{
		//Makse sure hit surface is facing up
		FVector impactNormal = hit.ImpactNormal;
		impactNormal.Normalize();


		//Teleport to that place + z offset
		if (impactNormal.Z > 0.8f)
		{
			float height = characterCapsule->GetScaledCapsuleHalfHeight();
			FVector newLocation = hit.ImpactPoint + FVector(0, 0, height);
			SetActorLocation(newLocation);
		}

	}
}

void AVrCharacter::teleportHold()
{
	teleportButtonHeld = true;
}

void AVrCharacter::teleportEvent()
{
	teleportButtonHeld = false;
	if (currentAutoMovementType == vrMovementType::TELEPORTING)
	{
		teleport(m_arcProfile, true);
	}
}

bool AVrCharacter::arcLineTrace(int resolution, FVector initialLocation, FVector initialVelocity, FVector acceleration, FHitResult& hitResult, bool isDebug, float debugLinetime = 2.0f)
{
	FHitResult hit;
	FCollisionQueryParams TraceParams;

	//Creates an arc of line traces to get a place to teleport to
	for (int T = 0; T < resolution; T++)
	{
		FVector currentPosition = NewtonianCurve(initialLocation, initialVelocity, acceleration, T);
		FVector nextPosition = NewtonianCurve(initialLocation, initialVelocity, acceleration, T+1);

		bool didHit = GetWorld()->LineTraceSingleByChannel(hit, currentPosition, nextPosition, ECC_Visibility, TraceParams);

		if (didHit)
		{
			if(isDebug) DrawDebugLine(GetWorld(), currentPosition, nextPosition, FColor::Green, false, debugLinetime);
			hitResult = hit;
			return true;
		}
		else
		{
			if (isDebug) DrawDebugLine(GetWorld(), currentPosition, nextPosition, FColor::Red, false, debugLinetime);
		}
	}
	return false;
}

void AVrCharacter::teleportPreview(FArcProfile profile, bool isDebug, float deltaTime)
{
	//Same as the teleport function however it only shows the line that would be used if you let go of the button
	FHitResult hit;
	FVector initalPosition;
	FVector initalVelocity;

	switch (movementHand)
	{
	case vrMovementHandedness::LEFT:
		initalPosition = leftController->GetComponentLocation();
		initalVelocity = leftController->GetForwardVector();
		break;
	case vrMovementHandedness::RIGHT:
		initalPosition = rightController->GetComponentLocation();
		initalVelocity = rightController->GetForwardVector();
		break;
	default:
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("No Movement Handedness has been set"));
		break;
	}
	initalVelocity = initalVelocity * profile.speed;
	arcLineTrace(profile.resolutionAndMaxTime, initalPosition, initalVelocity, FVector(0, 0, profile.gravity), hit, isDebug, deltaTime*2);
}


// Called when the game starts or when spawned
void AVrCharacter::BeginPlay()
{
	Super::BeginPlay();
	leftHandOffset = leftHand->GetRelativeLocation();
	rightHandOffset = rightHand->GetRelativeLocation();
	setHeight();
	
}

// Called every frame
void AVrCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Snap turning cooldown code
	if (hasTurned)
	{
		enlapsedTimeSinceLastTurn += DeltaTime;
		if (enlapsedTimeSinceLastTurn >= snapTurnCooldown)
		{
			enlapsedTimeSinceLastTurn = 0;
			hasTurned = false;
		}
	}

	//Gliding code
	if (!(canClimb && (leftControllerClimbing || rightControllerClimbing)))
	{
		if (canFly)
		{
			//Checks if the player is falling
			if (characterCapsule->GetPhysicsLinearVelocity().Y < 0.2)
			{
				//Checks that the two controllers are far enough away
				float distanceFromEachController = FVector::Distance(leftController->GetComponentLocation(), rightController->GetComponentLocation());
				if (distanceFromEachController >= activationDistance)
				{
					FVector vController = (leftController->GetComponentLocation() - rightController->GetComponentLocation()).GetAbs();
					vController.Normalize();
					//Checks if the controllers are horizontal enough
					if (vController.Z < horizontalTolerance)
					{
						glide();
					}
					else
					{
						stopGliding();
					}
				}
				else
				{
					stopGliding();
				}
			}
			else
			{
				stopGliding();
			}
		}
	}

	//Automated movement system 
	//Most functions here can be called from blueprints to allow for further customisation
	switch (currentAutoMovementType)
	{
	case vrMovementType::STATIC:
		break;
	case vrMovementType::FREEMOVEHEAD:
		//Checking if the player is not climbing atm
		if(!(canClimb && (leftControllerClimbing || rightControllerClimbing))) 
			VrSmoothMovement(InputComponent->GetAxisValue(vrBindings.MovementXAxis), InputComponent->GetAxisValue(vrBindings.MovementYAxis), DeltaTime, false);
		break;
	case vrMovementType::FREEMOVECTRL:
		//Checking if the player is not climbing atm
		if (!(canClimb && (leftControllerClimbing || rightControllerClimbing)))
			VrSmoothMovement(InputComponent->GetAxisValue(vrBindings.MovementXAxis), InputComponent->GetAxisValue(vrBindings.MovementYAxis), DeltaTime, true);
		break;
	case vrMovementType::TELEPORTING:
		//Checking if the player is not climbing atm
		if (!(canClimb && (leftControllerClimbing || rightControllerClimbing)))
		{
			if (teleportButtonHeld)
			{
				teleportPreview(m_arcProfile, true, DeltaTime);
			}
		}
		break;
	case vrMovementType::NONE:
		break;
	default:
		break;
	}

	//Climbing

	if (canClimb)
	{
		if (leftControllerClimbing)
		{
			leftHand->SetWorldLocation(leftControllerOldLocation);
			FVector movementVec = leftControllerOldLocation - leftController->GetComponentLocation();
			SetActorLocation(GetActorLocation() + movementVec);
		}
		else if (rightControllerClimbing)
		{
			rightHand->SetWorldLocation(rightControllerOldLocation);
			FVector movementVec = rightControllerOldLocation - rightController->GetComponentLocation();
			SetActorLocation(GetActorLocation() + movementVec);
		}
	}
}

void AVrCharacter::leftHandGrab()
{
	if (grabToggle)
	{
		if (hasGrabbedL)
		{
			release(vrMovementHandedness::LEFT);
			hasGrabbedL = false;
		}
		else
		{
			hasGrabbedL = grab(vrMovementHandedness::LEFT);
		}
	}
	else
	{
		grab(vrMovementHandedness::LEFT);
	}
}

void AVrCharacter::rightHandGrab()
{
	if (grabToggle)
	{
		if (hasGrabbedR)
		{
			release(vrMovementHandedness::RIGHT);
			hasGrabbedR = false;
		}
		else
		{
			hasGrabbedR = grab(vrMovementHandedness::RIGHT);
		}
	}
	else
	{
		grab(vrMovementHandedness::RIGHT);
	}
}

void AVrCharacter::leftHandRelease()
{
	if (!grabToggle)
	{
		//if there is an item being grabbed then...

		release(vrMovementHandedness::LEFT);
	}
}

void AVrCharacter::rightHandRelease()
{
	if (!grabToggle)
	{
		//if there is an item being grabbed then...
		release(vrMovementHandedness::RIGHT);
		
	}
}

bool AVrCharacter::grab(vrMovementHandedness handed)
{
	//Gets the correct motion controller
	UMotionControllerComponent* motionController = (handed == vrMovementHandedness::LEFT) ? leftController: rightController;
	FVector Start = motionController->GetComponentLocation();
	FVector End = Start;

	TArray<AActor*> ActorsToIgnore;

	ActorsToIgnore.Add(this);

	TArray<FHitResult> HitArray;

	//Checks in a radius around the hand for a grabable object
	bool hitBool = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Start, End, grabRadius, UEngineTypes::ConvertToTraceType(ECC_Camera),
		false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitArray, true, FLinearColor::Gray, FLinearColor::Blue, 1.0f);

	for (FHitResult hit : HitArray)
	{
		AActor* actor = hit.Actor.Get();
		//Checks that the actor grabbed has a grab component
		UgrabComponent* comp = Cast<UgrabComponent>(actor->GetComponentByClass(UgrabComponent::StaticClass()));

		if (comp != nullptr)
		{
			//if you can climb this actor then
			if (canClimb && comp->thisObjectIsClimbable())
			{
				switch (handed)
				{
				case vrMovementHandedness::LEFT:
					leftControllerClimbing = true;
					rightControllerClimbing = false;
					rightHand->SetRelativeLocation(rightHandOffset);
					characterCapsule->SetSimulatePhysics(false);
					characterCapsule->SetCollisionProfileName("NoCollisions");


					leftControllerOldLocation = Start;
					break;
				case vrMovementHandedness::RIGHT:
					leftControllerClimbing = false;
					rightControllerClimbing = true;
					leftHand->SetRelativeLocation(leftHandOffset);
					characterCapsule->SetSimulatePhysics(false);
					characterCapsule->SetCollisionProfileName("NoCollisions");
					rightControllerOldLocation = Start;
					break;
				default:
					break;
				}
			}
			else
			{
				//Grabbing object
				if (comp != nullptr)
				{
					hitBool = true;
		
					comp->getGrabbed(motionController);

					switch (handed)
					{
					case vrMovementHandedness::LEFT:
						//Setting actor grabbed for use in blueprints
						grabbedActorLeft = actor;
						if (rightGrab == comp)
						{
							if (!comp->multipleGrabPoints)
							{
								rightGrab = nullptr;
								hasGrabbedR = false;
							}
						}
						leftGrab = comp;
						break;
					case vrMovementHandedness::RIGHT:
						grabbedActorRight = actor;
						if (leftGrab == comp)
						{
							if (!comp->multipleGrabPoints)
							{
								leftGrab = nullptr;
								hasGrabbedL = false;
							}
						}
						rightGrab = comp;
						break;
					default:
						break;
					}
					break;
				}
				else
				{
					hitBool = false;
				}
			}
		}
		
	}


	return hitBool;
}

void AVrCharacter::release(vrMovementHandedness handed)
{
	//Handleing release if the released controller is climbing
	if (canClimb && (leftControllerClimbing || rightControllerClimbing))
	{

		bool haveFoundVault = false;
		FVector vaultLocation;

		//Check if the released hand should vault
		if (handed == vrMovementHandedness::LEFT ? !rightControllerClimbing : !leftControllerClimbing)
		{
			FVector Start = (handed == vrMovementHandedness::LEFT) ? leftHand->GetComponentLocation() : rightHand->GetComponentLocation();
			FVector End = Start;

			TArray<AActor*> ActorsToIgnore;

			ActorsToIgnore.Add(this);

			FHitResult hit;

			//Getting a point on the area that has been released 
			bool hitBool = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, grabRadius, UEngineTypes::ConvertToTraceType(ECC_Camera),
				false, ActorsToIgnore, EDrawDebugTrace::ForDuration, hit, true, FLinearColor::Green, FLinearColor::Red, 1.0f);

			if (hitBool)
			{
				//Makse sure hit surface is facing up
				FVector impactNormal = hit.ImpactNormal;
				impactNormal.Normalize();

				//If the normal of the impact point is facing up
				if (impactNormal.Z > 0.8f)
				{
					//Teleport on top of it
					float height = characterCapsule->GetScaledCapsuleHalfHeight() * 2;
					vaultLocation = hit.ImpactPoint + FVector(0, 0, height * 0.8f);
					SetActorLocation(vaultLocation);
					characterCapsule->SetPhysicsLinearVelocity(FVector::ZeroVector);
				}
			}
		}
		//setting each hand back to its parent
		switch (handed)
		{
		case vrMovementHandedness::LEFT:
			leftControllerClimbing = false;
			leftHand->SetRelativeLocation(leftHandOffset);
			characterCapsule->SetSimulatePhysics(true);
			characterCapsule->SetCollisionProfileName("Pawn");
			break;
		case vrMovementHandedness::RIGHT:
			rightControllerClimbing = false;
			rightHand->SetRelativeLocation(rightHandOffset);
			characterCapsule->SetSimulatePhysics(true);
			characterCapsule->SetCollisionProfileName("Pawn");
			break;
		default:
			break;
		}

		//doing a second vault check, to remove any residual velocity
		if (haveFoundVault)
		{
			SetActorLocation(vaultLocation);
			characterCapsule->SetPhysicsLinearVelocity(FVector::ZeroVector);
			rightHand->SetRelativeLocation(rightHandOffset);
			leftHand->SetRelativeLocation(leftHandOffset);
		}
	
	}

	//Releasing object from hand even if the other hand is climbing
	if((handed == vrMovementHandedness::LEFT) ? !leftControllerClimbing : !rightControllerClimbing)
	{
		//Releases the component that its currently grabbing in one hand
		switch (handed)
		{
		case vrMovementHandedness::LEFT:

			if (leftGrab != nullptr)
			{
				leftGrab->releaseComponent(leftController);
			}

			if (grabbedActorLeft != nullptr)
			{
				grabbedActorLeft = nullptr;
			}
			break;
		case vrMovementHandedness::RIGHT:
			if (rightGrab != nullptr)
			{
				rightGrab->releaseComponent(rightController);
			}

			if (grabbedActorRight != nullptr)
			{
				grabbedActorRight = nullptr;
			}
			break;
		default:
			break;
		}
	}
}

void AVrCharacter::stopGliding()
{
	characterCapsule->SetEnableGravity(true);
	currentlyGlding = false;
}

void AVrCharacter::glide()
{
	if (!currentlyGlding)
	{
		currentlyGlding = true;
		characterCapsule->SetPhysicsLinearVelocity(FVector(characterCapsule->GetPhysicsLinearVelocity().X, characterCapsule->GetPhysicsLinearVelocity().Y, fallSpeed));
		characterCapsule->SetEnableGravity(false);
	}
	else
	{
		if (characterCapsule->GetPhysicsLinearVelocity().Y >= -0.5)
		{
			stopGliding();
		}
	}
}
// Called to bind functionality to input
void AVrCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAxis(vrBindings.MovementXAxis);
	InputComponent->BindAxis(vrBindings.MovementYAxis);

	InputComponent->BindAxis(vrBindings.TurnAxis, this, &AVrCharacter::nonBPTurn);

	InputComponent->BindAction(vrBindings.Teleport,IE_Released, this, &AVrCharacter::teleportEvent);
	InputComponent->BindAction(vrBindings.Teleport, IE_Pressed, this, &AVrCharacter::teleportHold);

	if (canGrab)
	{
		InputComponent->BindAction(vrBindings.LGrab, IE_Pressed, this, &AVrCharacter::leftHandGrab);
		InputComponent->BindAction(vrBindings.RGrab, IE_Pressed, this, &AVrCharacter::rightHandGrab);
		InputComponent->BindAction(vrBindings.LGrab, IE_Released, this, &AVrCharacter::leftHandRelease);
		InputComponent->BindAction(vrBindings.RGrab, IE_Released, this, &AVrCharacter::rightHandRelease);
	}
}


void AVrCharacter::setHeight()
{
	characterCapsule->SetCapsuleHalfHeight(characterHeight, true);
	characterCamera->SetRelativeLocation(FVector(0, 0, characterHeight));
}

void AVrCharacter::setNewHeight(float height)
{
	characterHeight = height;
	characterCapsule->SetCapsuleHalfHeight(height, true);
	characterCamera->SetRelativeLocation(FVector(0, 0, height));

}

void AVrCharacter::VrSmoothMovement(float xAxis, float yAxis, float DeltaTime, bool useControllerForward)
{

	FVector Forward;
	FVector Right;
	if (useControllerForward)
	{
		switch (movementHand)
		{
		case vrMovementHandedness::LEFT:
			Forward = leftController->GetForwardVector();
			Right = leftController->GetRightVector();
			break;
		case vrMovementHandedness::RIGHT:
			Forward = rightController->GetForwardVector();
			Right = rightController->GetRightVector();
			break;
		default:
			Forward = leftController->GetForwardVector();
			Right = leftController->GetRightVector();
			break;
		}
	}
	else
	{
		Forward = characterCamera->GetForwardVector();
		Right = characterCamera->GetRightVector();
	}


	Forward.Normalize();
	Forward = Forward * FMath::Clamp(yAxis, -1.0f, 1.0f);
	Forward = FVector(Forward.X, Forward.Y, 0);

	Right.Normalize();
	Right = Right * FMath::Clamp(xAxis, -1.0f, 1.0f);
	Right = FVector(Right.X, Right.Y, 0);

	//Combine the forward and right vectors and multiplying it by the speed to get the movement vector.
	FVector movementDirection = Forward + Right;
	if (!movementDirection.IsZero())
	{
		const FVector newLoc = GetActorLocation() + (movementDirection * DeltaTime * movementSpeed);
		SetActorLocation(newLoc);
	}
}

void AVrCharacter::nonBPTurn(float AxisValue)
{
	
	switch (turningType)
	{
	case vrTurningType::NONE:
		break;
	case vrTurningType::SMOOTH:
		AddActorWorldRotation(FRotator(0, AxisValue * smoothTurnSpeed, 0));
		break;
	case vrTurningType::SNAP:
		if (!hasTurned)
		{
			if (abs(AxisValue) > 0.8)
			{
				if (AxisValue < 0)
				{
					AddActorWorldRotation(FRotator(0, -snapTurnAmount, 0));
				}
				else if (AxisValue > 0)
				{
					AddActorWorldRotation(FRotator(0, snapTurnAmount, 0));
				}
				hasTurned = true;
			}
		}
		break;
	default:
		break;
	}
	
}

void AVrCharacter::Turn(float AxisValue, vrTurningType _turningType)
{
	switch (_turningType)
	{
	case vrTurningType::NONE:
		break;
	case vrTurningType::SMOOTH:
		AddActorWorldRotation(FRotator(0, AxisValue * smoothTurnSpeed, 0));
		break;
	case vrTurningType::SNAP:
		if (!hasTurned)
		{
			if (abs(AxisValue) > 0.8)
			{
				if (AxisValue < 0)
				{
					AddActorWorldRotation(FRotator(0, -snapTurnAmount, 0));
				}
				else if (AxisValue > 0)
				{
					AddActorWorldRotation(FRotator(0, snapTurnAmount, 0));
				}
				hasTurned = true;
			}
		}
		break;
	default:
		break;
	}
}



