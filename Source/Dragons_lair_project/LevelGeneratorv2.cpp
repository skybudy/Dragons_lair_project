#include "LevelGeneratorv2.h"

ALevelGeneratorv2::ALevelGeneratorv2()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ALevelGeneratorv2::BeginPlay()
{
    Super::BeginPlay();

    if (BlockTemplates.Num() < 2)
    {
        UE_LOG(LogTemp, Error, TEXT("LevelGeneratorv2: Need at least 2 templates (1 Starter, 1 Variation)!"));
        return;
    }

    // 1. Spawn the unique Starter Block (Index 0)
    SpawnBlock(BlockTemplates[0], FVector::ZeroVector);

    // 2. Fill the rest of the initial track with variation blocks
    for (int32 i = 1; i < MaxBlocksAhead; i++)
    {
        FVector NextLocation = FVector(i * BlockLength, 0.0f, 0.0f);
        SpawnBlock(BlockTemplates[GetNextBlockIndex()], NextLocation);
    }
}

void ALevelGeneratorv2::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!LastSpawnedBlock || !MovementParent) return;

    // Check if world movement has moved the last block enough to spawn a new one
    float LastBlockX = LastSpawnedBlock->GetActorLocation().X;
    float SpawnThreshold = (MaxBlocksAhead - 1) * BlockLength;

    if (LastBlockX < SpawnThreshold)
    {
        FVector NewSpawnPos = LastSpawnedBlock->GetActorLocation() + FVector(BlockLength, 0.0f, 0.0f);
        SpawnBlock(BlockTemplates[GetNextBlockIndex()], NewSpawnPos);
    }

    CleanupBlocks();
}

int32 ALevelGeneratorv2::GetNextBlockIndex()
{
    // Safety check: ensure we have variation blocks (index 1 and above)
    if (BlockTemplates.Num() < 2) return 0;

    if (ShuffleBag.Num() == 0)
    {
        // Refill the bag with indices starting from 1
        for (int32 i = 1; i < BlockTemplates.Num(); i++)
        {
            // Only add if the class actually exists in the array slot
            if (BlockTemplates[i])
            {
                ShuffleBag.Add(i);
            }
        }

        // shuffle
        for (int32 i = ShuffleBag.Num() - 1; i > 0; i--)
        {
            int32 RandomIdx = FMath::RandRange(0, i);
            ShuffleBag.Swap(i, RandomIdx);
        }
    }

    // Double check if we actually have anything in the bag now
    if (ShuffleBag.Num() > 0)
    {
        return ShuffleBag.Pop();
    }

    return 1; // Fallback to first variation
}

void ALevelGeneratorv2::SpawnBlock(TSubclassOf<AActor> BlockClass, FVector Location)
{
    // 1. null check
    if (!BlockClass || !GetWorld()) return;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AActor* NewBlock = GetWorld()->SpawnActor<AActor>(BlockClass, Location, FRotator::ZeroRotator, Params);

	if (IsValid(NewBlock)) // 2. Use IsValid() instead of just if(NewBlock) for safety
    {
        if (IsValid(MovementParent))
        {
            NewBlock->AttachToActor(MovementParent, FAttachmentTransformRules::KeepWorldTransform);
        }

        ActiveBlocks.Add(NewBlock);
        LastSpawnedBlock = NewBlock;
    }
}

void ALevelGeneratorv2::CleanupBlocks()
{
    for (int32 i = ActiveBlocks.Num() - 1; i >= 0; i--)
    {
        AActor* Block = ActiveBlocks[i];

        // 3. Ensure we are not trying to access or destroy something already gones
        if (IsValid(Block))
        {
            if (Block->GetActorLocation().X < DeletionXThreshold)
            {
                ActiveBlocks.RemoveAt(i);
                Block->Destroy();
            }
        }
        else
        {
            // Remove null entries if any
            ActiveBlocks.RemoveAt(i);
        }
    }
}