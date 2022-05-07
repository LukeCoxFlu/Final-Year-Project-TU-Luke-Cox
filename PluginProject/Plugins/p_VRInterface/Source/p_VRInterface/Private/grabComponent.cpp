// Fill out your copyright notice in the Description page of Project Settings.
#include "grabComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UgrabComponent::UgrabComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UgrabComponent::getGrabbed(USceneComponent* attachedTo)
{
	//Checks if the reference too the static mesh componet is there
	if (reference == nullptr)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("The Grab object Must have a static mesh component"));
	}

	//Find the socket closest to the hand to use as the grab point
	FName closestCurrentSocket;
	if (useSockets)
	{
		FVector componentLocation = attachedTo->GetComponentLocation();
		closestCurrentSocket = listOfSockets[0];
		float closestCurrentDistance = FVector::Distance(reference->GetSocketLocation(listOfSockets[0]), componentLocation);
		for (FName socket : listOfSockets)
		{
			float Distance = FVector::Distance(reference->GetSocketLocation(socket), componentLocation);
			if (Distance < closestCurrentDistance)
			{
				closestCurrentSocket = socket;
				closestCurrentDistance = Distance;
			}
		}
	}

	//Physics objects to get picked up
	if (gravityEnabled)
	{
		reference->SetSimulatePhysics(false);
	}
	reference->SetCollisionProfileName("CharacterMesh");



	FAttachmentTransformRules rules(EAttachmentRule::KeepWorld,EAttachmentRule::KeepWorld,EAttachmentRule::KeepWorld,true);
	FAttachmentTransformRules rulesSocket(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);


	//Multiple grabs
	if (multipleGrabPoints)
	{
		//Assigning dominant hand
		if (NumberOfControllersCurrentlyGrabbing == 0)
		{
			if (!gravityEnabled)
			{
				if (useSockets) GetOwner()->AttachToComponent(attachedTo, rulesSocket, closestCurrentSocket);
				else GetOwner()->AttachToComponent(attachedTo, rules);
			}
			else
			{
				if (useSockets) reference->AttachToComponent(attachedTo, rulesSocket, closestCurrentSocket);
				else reference->AttachToComponent(attachedTo, rules);
			}
			
			dominant = attachedTo;
			NumberOfControllersCurrentlyGrabbing++;
			dominantRotator = GetOwner()->GetActorRotation();
		}
		//Assing secondary hand for dominant had to look at
		else if (NumberOfControllersCurrentlyGrabbing == 1)
		{
			secondary = attachedTo;
			NumberOfControllersCurrentlyGrabbing++;
		}
		else
		{
			if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("To many controllers?????"));
		}
	}
	else
	{
		if (!gravityEnabled)
		{
			if (useSockets) GetOwner()->AttachToComponent(attachedTo, rulesSocket, closestCurrentSocket);
			else GetOwner()->AttachToComponent(attachedTo, rules);
		}
		else
		{
			if (useSockets) reference->AttachToComponent(attachedTo, rulesSocket, closestCurrentSocket);
			else reference->AttachToComponent(attachedTo, rules);
		}
	}
}

void UgrabComponent::releaseComponent(USceneComponent* attachedTo)
{
	FDetachmentTransformRules rules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, true);

	if (multipleGrabPoints)
	{
		if (attachedTo == dominant)
		{
			NumberOfControllersCurrentlyGrabbing--;
			if (gravityEnabled)
			{
				GetOwner()->SetActorLocation(reference->GetComponentLocation());
				reference->DetachFromComponent(rules);
				reference->SetCollisionProfileName("BlockAllDynamic");
				reference->SetSimulatePhysics(true);
			}
			else
			{
				GetOwner()->DetachFromActor(rules);
				reference->SetCollisionProfileName("BlockAllDynamic");
			}
			
		}
		else if (attachedTo == secondary)
		{
			secondary = nullptr;
			NumberOfControllersCurrentlyGrabbing--;
			GetOwner()->SetActorRotation(dominantRotator);
		}
	}
	else
	{
		if (gravityEnabled)
		{
			GetOwner()->SetActorLocation(reference->GetComponentLocation());
			reference->DetachFromComponent(rules);
			reference->SetCollisionProfileName("BlockAllDynamic");
			reference->SetSimulatePhysics(true);
		}
		else
		{
			GetOwner()->DetachFromActor(rules);
			reference->SetCollisionProfileName("BlockAllDynamic");
		}
	}
}

bool UgrabComponent::thisObjectIsClimbable()
{
	return isClimbable;
}

// Called when the game starts
void UgrabComponent::BeginPlay()
{
	Super::BeginPlay();
	reference = Cast<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (reference == nullptr)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("The Grab object Must have a static mesh component"));
	}
	else
	{
		gravityEnabled = reference->IsSimulatingPhysics();
	}
	// ...
	
}

void UgrabComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (NumberOfControllersCurrentlyGrabbing == 2)
	{
		//Code for two handed items, doesnt work very well :(
		FRotator rotation = UKismetMathLibrary::FindLookAtRotation(dominant->GetComponentLocation(), secondary->GetComponentLocation());
		FRotator B(0, -90, 0);

		rotation = rotation + B;

		FRotator finalRot(dominant->GetComponentRotation().Roll, rotation.Yaw, (rotation.Pitch * -1));

		GetOwner()->SetActorRotation(finalRot);
	}
}


