// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupableObject.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"

// Sets default values
APickupableObject::APickupableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// Disable tick as it is not used
	PrimaryActorTick.bCanEverTick = false;
	
	//Creating the Components in the Blueprintclass
	PickUpMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickUpMesh"));
	PickUpCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickUpCollisionSphere"));
	
	//Sets mesh as the root and attaches collision sphere to it
	RootComponent = PickUpMesh;	
	PickUpCollisionSphere->SetupAttachment(PickUpMesh);
	
	//Binds to the Overlap function
	PickUpCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &APickupableObject::OnSphereBeginOverlap);
}

//OnActorBeginOverlap
void APickupableObject::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//This is the cast function
	//TO DO, Change ACharacter to the proper player and add scoring.
	ACharacter* OverlappingCharacter = Cast<ACharacter>(OtherActor);
	if (OverlappingCharacter)
	{
		PickedUp();
	}
}

//Notifies listeners and destroys itself
void APickupableObject::PickedUp()
{
	OnPickedUp.Broadcast();
	Destroy();
}