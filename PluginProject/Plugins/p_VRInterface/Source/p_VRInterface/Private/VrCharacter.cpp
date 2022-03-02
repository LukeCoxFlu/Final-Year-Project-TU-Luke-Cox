// Fill out your copyright notice in the Description page of Project Settings.


#include "VrCharacter.h"

// Sets default values
AVrCharacter::AVrCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	SetRootComponent(RootSceneComponent);

	characterCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Character Capsule"));
	characterCamera = CreateAbstractDefaultSubobject<UCameraComponent>(TEXT("Character Camera"));
	
}

// Called when the game starts or when spawned
void AVrCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVrCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AVrCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

