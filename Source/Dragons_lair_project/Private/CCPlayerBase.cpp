// Fill out your copyright notice in the Description page of Project Settings.


#include "CCPlayerBase.h"

#include <rapidjson/document.h>

#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ACCPlayerBase::ACCPlayerBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Locks the player on the x-axis
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(1.f,0.f,0.f));

	//Enables crouching and sets crouching settings
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	
}

// Called when the game starts or when spawned
void ACCPlayerBase::BeginPlay()
{
	Super::BeginPlay();

	//This is the Enhanced Input System from Blueprints
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) //Determines if the player exist
	{
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer()) //Determines if the local player exists
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer)) //Creates the subsystem
			{
				Subsystem->AddMappingContext(MappingContext,0);
			}
		}
	}
}

// Called every frame
void ACCPlayerBase::Tick(float DeltaTime)
{ 
	Super::Tick(DeltaTime);
	
	//Determines the current player state (Delete this later and just tie it to the input actions)
	if (GetCharacterMovement()->IsFalling()) //0 is running, 1 is jumping, 2 is sliding
	{
		CurrentPlayerState = EPlayerState::Jumping; //If jumping it sets the current state to jumping
	}
	else if (bIsPlayerCrouched)
	{
		CurrentPlayerState = EPlayerState::Sliding; //If sliding it sets current state to slide state
	}
	else
	{
		CurrentPlayerState = EPlayerState::Running; //If running it sets current state to run state
	}

	FString EnumStr = UEnum::GetValueAsString(CurrentPlayerState);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Current State: %s"), *EnumStr));
	
	if (bIsSwitchingLanes)
	{
			float TargetLanePosition = LanePositions[TargetLane]; //Gets the positon that the player want to go to
			float PlayerYPosition = GetActorLocation().Y;
			float ErrorRange = 10;
		
			if (PlayerYPosition < TargetLanePosition - ErrorRange) //Checks if target lane is right of player position
			{
				MoveTowardsTargetLane(1);
			}
			else if (PlayerYPosition > TargetLanePosition + ErrorRange) //Checks if target lane is left of player position
			{
				MoveTowardsTargetLane(-1);
			}
			else //Player Must Have Reached The Target
			{
				WhenPlayerLaneHasBeenReached(TargetLanePosition);
			}
	}
}

void ACCPlayerBase::MoveTowardsTargetLane(int MoveDirection)//Moves towards target lane position
{
	GetCharacterMovement()->Velocity.Y = MoveDirection * LaneSwitchingSpeed;
}

void ACCPlayerBase::WhenPlayerLaneHasBeenReached(float NewLanePosition)
{
	bIsSwitchingLanes = false; //Stops switching lanes

	//Sets Y Velocity to 0, so it stops immediatlyd
	GetCharacterMovement()->Velocity.Y = 0.0f;
	
	//Sets current lane to new current lane
	CurrentLane = TargetLane;

	//Sets player position to new lane position for increased accuracy
	FVector NewPlayerPosition = GetActorLocation();
	NewPlayerPosition.Y = NewLanePosition;
	SetActorLocation(NewPlayerPosition);
}




// Called to bind functionality to input
void ACCPlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction,ETriggerEvent::Triggered,this,&ACCPlayerBase::PlayerJumpTriggered);
		EnhancedInputComponent->BindAction(JumpAction,ETriggerEvent::Completed,this,&ACCPlayerBase::PlayerJumpCompleted);

		EnhancedInputComponent->BindAction(SlideAction,ETriggerEvent::Triggered,this,&ACCPlayerBase::PlayerSlideTriggered);
		EnhancedInputComponent->BindAction(SlideAction,ETriggerEvent::Completed,this,&ACCPlayerBase::PlayerSlideCompleted);
		
		EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Started,this,&ACCPlayerBase::PlayerMoveTriggered);
	}
}

void ACCPlayerBase::PlayerJumpTriggered() //Uncrouches and jumps when the Jump Action is triggered
{
	UnCrouch();
	Jump();
	CurrentPlayerState = EPlayerState::Jumping; //If jumping it sets the current state to jumping
	bIsPlayerJumping = true;
	bIsPlayerCrouched = false;
}

void ACCPlayerBase::PlayerJumpCompleted() //Stops jumping when Jump Action is no longer triggered
{
	StopJumping();
	bIsPlayerJumping = false;
}

void ACCPlayerBase::PlayerSlideTriggered() //Crouches (Slides) the player if they mean several checks
{
	if (!bIsPlayerJumping) //Get the action value bool from the JumpAction
	{
		if (GetCharacterMovement()->IsFalling()) //Checks if the character is falling
		{
			UnCrouch();
			bIsPlayerCrouched = false;
		}
		else //If they are on the ground and not jumping, then crouch
		{
			Crouch();
			CurrentPlayerState = EPlayerState::Sliding; //If sliding it sets current state to slide state
			bIsPlayerCrouched = true;
		}
	}
}

void ACCPlayerBase::PlayerSlideCompleted() //Uncrouches (Unslides) when player stops pressing slide
{
	UnCrouch();
	bIsPlayerCrouched = false;
}

void ACCPlayerBase::PlayerMoveTriggered(const FInputActionValue& ActionValue) //Lane switching system
{
	if (!bIsPlayerCrouched) //Doesn't changing target lane when sliding
	{
		bIsSwitchingLanes = true;

		FVector2D MovementInput = ActionValue.Get<FVector2D>(); //Gets the input direction

		if (MovementInput.X > 0) //If right input pressed
		{
			if (TargetLane + 1 < LanePositions.Num()) //If target lane is not the right most lane  
			{
				TargetLane += 1; //Move the target lane one to the right
			}
		}
		else //If left input pressed
		{
			if (TargetLane > 0) //If target lane is not the left most lane
			{
				TargetLane -= 1; //Move the target lane one to the left
			}
		}

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%d"),TargetLane));
	}
}

