// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class DRAGONS_LAIR_PROJECT_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();
	//TO DO, add Reference to WBP QTE, BP Patrol and PlayerCharacter	
	float FGameTime;
	float FPatrolSpeed;
	bool bSuccess;
	bool bSeesPlayer;
	bool bPlayerInAttackRange;
	bool bQTEComplete;
	
	UPROPERTY(VisibleAnywhere, Category = "Collision Capsule")
	class UCapsuleComponent* EnemyAttackRange;
	
	UPROPERTY(VisibleAnywhere, Category = "Collision Sphere")
	class USphereComponent* EnemyVisionRange;
	
	// UPROPERTY(VisibleAnywhere, Category = "State Tree")
	// class UStateTreeComponent* StateTreeComponent;
	
	UFUNCTION()
	void OnVisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnAttackRangeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
