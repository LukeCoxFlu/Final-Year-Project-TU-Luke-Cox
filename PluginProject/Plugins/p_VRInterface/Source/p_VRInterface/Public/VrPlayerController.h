// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VrCharacter.h"
#include "GameFramework/PlayerController.h"
#include "VrPlayerController.generated.h"

/**
 * 
 */
UCLASS(Category = "VR_Plugin")
class P_VRINTERFACE_API AVrPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	//FUNC
	//Constructor
	AVrPlayerController();


	//Variables

	/** This Is A Refereance to the current player pawn*/
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, Category = "VR_Plugin")
	AVrCharacter* currentPawn = nullptr;




protected:

	virtual void BeginPlay() override;

};
