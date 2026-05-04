// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CCPlayerBase.generated.h"

class UInputAction;
class UInputMappingContext;

//Enum for player state
UENUM(BlueprintType) //0 is running, 1 is jumping, 2 is sliding
enum class EPlayerState: uint8
{
	Running UMETA(DisplayName = "Running State"),
	Jumping UMETA(DisplayName = "Jumping State"),
	Sliding UMETA(DisplayName = "Sliding State")
};

UCLASS()
class DRAGONS_LAIR_PROJECT_API ACCPlayerBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACCPlayerBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Lane Switching Functions
	void MoveTowardsTargetLane(int MoveDirection);
	void WhenPlayerLaneHasBeenReached(float NewLanePosition);
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	

	/*
	 * Controller
	 */

	UPROPERTY(EditAnywhere)
	UInputMappingContext* MappingContext;

	UPROPERTY(EditAnywhere)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere)
	UInputAction* SlideAction;

	UPROPERTY(EditAnywhere)
	UInputAction* MoveAction;

	void PlayerJumpTriggered();
	void PlayerJumpCompleted();
	
	void PlayerSlideTriggered();
	void PlayerSlideCompleted();
	
	void PlayerMoveTriggered(const FInputActionValue& ActionValue);

	//Variables
	UPROPERTY(VisibleAnywhere)
	bool bIsPlayerJumping = false;

	UPROPERTY(VisibleAnywhere)
	bool bIsPlayerCrouched = false;

	UPROPERTY(VisibleAnywhere)
	bool bIsSwitchingLanes = false;

	UPROPERTY(EditAnywhere)
	int LaneSwitchingSpeed = 1000; //See what value is good
	
	UPROPERTY(VisibleAnywhere)
	int LaneSwitchingDirection = 0;

	UPROPERTY(EditAnywhere)
	int StartingLane = 2;
	
	UPROPERTY(VisibleAnywhere)
	int CurrentLane = StartingLane;

	UPROPERTY(VisibleAnywhere)
	int TargetLane = CurrentLane;
	
	UPROPERTY(EditAnywhere)
	TArray<int32> LanePositions = {-260,-130,0,130,260};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EPlayerState CurrentPlayerState = EPlayerState::Running;
};
