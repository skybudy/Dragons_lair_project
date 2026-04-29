// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "PickupableObject.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPickedUp);

UCLASS()
class DRAGONS_LAIR_PROJECT_API APickupableObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupableObject();
	
	//Static Mesh for visibility
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UStaticMeshComponent* PickUpMesh;
	
	//Collision Sphere created for functionality
	UPROPERTY(VisibleAnywhere, Category = "SphereCollision")
	class USphereComponent* PickUpCollisionSphere;
	
	//On picked up condition
	UPROPERTY(BlueprintAssignable, Category = "EventDispatcher")
	FOnPickedUp OnPickedUp;
	
	//Overlap Function for the collision sphere
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	//PickedUp function
	UFUNCTION(BlueprintCallable, Category = "Function")
	void PickedUp();
};