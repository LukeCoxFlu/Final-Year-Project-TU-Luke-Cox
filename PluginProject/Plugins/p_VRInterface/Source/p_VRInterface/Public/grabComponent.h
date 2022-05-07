// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MotionControllerComponent.h"
#include "Engine/Engine.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ActorComponent.h"
#include "grabComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Category = "VR_Plugin")
class P_VRINTERFACE_API UgrabComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab", meta = (AllowPrivateAccess = "true"), meta = (ToolTip = "Does this grabable object use sockets to grab on"))
	bool useSockets = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab", meta = (AllowPrivateAccess = "true"), meta = (ToolTip = "Name of each of the applicable sockets"))
	TArray<FName> listOfSockets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab", meta = (AllowPrivateAccess = "true"), meta = (ToolTip = "Is this object a climbable wall"))
	bool isClimbable;


	bool gravityEnabled = false;

	UStaticMeshComponent* reference = nullptr;

	USceneComponent* dominant = nullptr;
	USceneComponent* secondary = nullptr;

	FRotator dominantRotator;



	int NumberOfControllersCurrentlyGrabbing = 0;

public:	
	// Sets default values for this component's properties

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grab (UNFINISHED)")
	bool multipleGrabPoints = false;

	UgrabComponent();

	UFUNCTION(BlueprintCallable, Category = "Grabbing")
	void getGrabbed(USceneComponent* attachedTo);
	UFUNCTION(BlueprintCallable, Category = "Grabbing")
	void releaseComponent(USceneComponent* attachedTo);

	bool thisObjectIsClimbable();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
