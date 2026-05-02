// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/UserWidget.h"
#include "MenuCharacter.generated.h"

UCLASS()
class DRAGONS_LAIR_PROJECT_API AMenuCharacter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMenuCharacter();
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> MenuWidgetClass;
	
	UPROPERTY()
	UUserWidget* MenuWidget;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
