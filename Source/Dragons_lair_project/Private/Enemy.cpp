// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"


// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	EnemyAttackRange = CreateDefaultSubobject<UCapsuleComponent>(TEXT("EnemyAttackRange"));
	EnemyVisionRange = CreateDefaultSubobject<USphereComponent>(TEXT("EnemyVisionRange"));

	EnemyAttackRange ->SetupAttachment(GetMesh());
	EnemyVisionRange ->SetupAttachment(GetMesh());

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	FGameTime = .4;
	bSeesPlayer = false;
	bPlayerInAttackRange = false;
	bQTEComplete = false;
	
}

void AEnemy::OnVisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//This is the cast function
	ACharacter* OverlappingCharacter = Cast<ACharacter>(OtherActor);
	if (OverlappingCharacter)
	{
		bSeesPlayer = true;
	}
}

void AEnemy::OnAttackRangeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//This is the cast function
	ACharacter* OverlappingCharacter = Cast<ACharacter>(OtherActor);
	if (OverlappingCharacter)
	{
		bPlayerInAttackRange = true;
	}
}


// // Called every frame
// void AEnemy::Tick(float DeltaTime)
// {
// 	Super::Tick(DeltaTime);
//
// }
//
// // Called to bind functionality to input
// void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
// {
// 	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
// }

