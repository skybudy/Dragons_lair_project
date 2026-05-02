// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuCharacter.h"

// Sets default values
AMenuCharacter::AMenuCharacter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AMenuCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (MenuWidgetClass)
	{
		MenuWidget = CreateWidget <UUserWidget>(GetWorld(), MenuWidgetClass);
		if (MenuWidget)
		{
			MenuWidget->AddToViewport();
		}
	}
}

// Called every frame
void AMenuCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMenuCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}