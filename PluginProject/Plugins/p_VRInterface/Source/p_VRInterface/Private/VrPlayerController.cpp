// Fill out your copyright notice in the Description page of Project Settings.


#include "VrPlayerController.h"



AVrPlayerController::AVrPlayerController()
{
	//Default tick enable
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

}

void AVrPlayerController::BeginPlay()
{
	Super::BeginPlay();

	currentPawn = Cast<AVrCharacter>(GetPawn());

	if (currentPawn == nullptr)
	{
		if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Something went wrong, VRController, BeginPlay")); 
	}


}
