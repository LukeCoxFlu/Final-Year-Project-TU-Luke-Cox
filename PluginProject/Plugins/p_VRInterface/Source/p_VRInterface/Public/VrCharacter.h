// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "VrCharacter.generated.h"

UCLASS()
class P_VRINTERFACE_API AVrCharacter : public APawn
{
	GENERATED_BODY()

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VrDefault", meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* characterCapsule = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VrDefault", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* characterCamera = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VrDefault", meta = (AllowPrivateAccess = "true"))
	float characterHeight = 100.0f;

	//Movemnt Functionality
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VrMovement", meta = (AllowPrivateAccess = "true"))
	float movementSpeed = 400.0f;

	void MoveForward(float value);
	void MoveRight(float value);

	FVector movementDirectionY;
	FVector movementDirectionX;


public:
	// Sets default values for this pawn's properties
	AVrCharacter();

	UFUNCTION(BlueprintCallable, Category = "Player Height")
	void setHeight();

	UFUNCTION(BlueprintCallable, Category = "Player Height")
	void setNewHeight(float height);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void VrCharacterMovement(float xAxis, float yAxis, float DeltaTime);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;




};
