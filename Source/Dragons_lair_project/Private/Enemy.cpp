// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"


// Sets default values
AEnemy::AEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	EnemyVisionRange = CreateDefaultSubobject<USphereComponent>(TEXT("EnemyVisionRange"));
	EnemyAttackRange = CreateDefaultSubobject<UCapsuleComponent>(TEXT("EnemyAttackRange"));
	
	//Create root component and attach collisions
	RootComponent = SkeletalMesh;
	EnemyVisionRange ->SetupAttachment(SkeletalMesh);
	EnemyAttackRange ->SetupAttachment(SkeletalMesh);
	
	//Binds to overlap function
	EnemyVisionRange ->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnVisionSphereBeginOverlap);
	EnemyAttackRange ->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnAttackRangeBeginOverlap);
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
	UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//This is the cast function
	ACharacter* OverlappingCharacter = Cast<ACharacter>(OtherActor);
	if (OverlappingCharacter)
	{
		bSeesPlayer = true;
	}
}

void AEnemy::OnAttackRangeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	//This is the cast function
	ACharacter* OverlappingCharacter = Cast<ACharacter>(OtherActor);
	if (OverlappingCharacter)
	{
		bPlayerInAttackRange = true;
	}
}
