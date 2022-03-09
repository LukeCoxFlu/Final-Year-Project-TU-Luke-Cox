// Fill out your copyright notice in the Description page of Project Settings.


#include "VrCharacter.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values
AVrCharacter::AVrCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	characterCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Character Capsule"));
	characterCapsule->InitCapsuleSize(40, characterHeight);
	SetRootComponent(characterCapsule);
	characterCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Character Camera"));
	characterCamera->SetupAttachment(characterCapsule);
	
	
	


}

// Called when the game starts or when spawned
void AVrCharacter::BeginPlay()
{
	Super::BeginPlay();
	

	setHeight();
	
}

// Called every frame
void AVrCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector movementDirection = movementDirectionY + movementDirectionX;
	movementDirection.Normalize();
	if (!movementDirection.IsZero())
	{
		const FVector newLoc = GetActorLocation() + (movementDirection * DeltaTime * movementSpeed);
		SetActorLocation(newLoc);
	}

}

// Called to bind functionality to input
void AVrCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MovementY", this, &AVrCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MovementX", this, &AVrCharacter::MoveRight);

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

void AVrCharacter::MoveForward(float value)
{
	FVector Forward = characterCamera->GetForwardVector() * FMath::Clamp(value, -1.0f, 1.0f);
	movementDirectionY = FVector(Forward.X, Forward.Y, 0);
}

void AVrCharacter::MoveRight(float value)
{
	FVector Right = characterCamera->GetRightVector() * FMath::Clamp(value, -1.0f, 1.0f);
	movementDirectionX = FVector(Right.X, Right.Y, 0);
}

