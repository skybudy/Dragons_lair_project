#include "LevelGeneratorv2.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

ALevelGeneratorv2::ALevelGeneratorv2()
{
    PrimaryActorTick.bCanEverTick = true;
    CurrentBiome = EBiomeType::Biome1;
    InternalProgress = 0.0f;
    BlocksInCurrentChunk = 0;
    MaxBlocksInChunk = 2; // Default to a 2-block chunk (e.g., 8 floors total)
}

void ALevelGeneratorv2::BeginPlay()
{
    Super::BeginPlay();

    // 1. Initialize the run with the designated starting block (always Study biome)
    if (StartBlockTemplate)
    {
        AActor* NewBlock = GetWorld()->SpawnActor<AActor>(StartBlockTemplate, FVector::ZeroVector, FRotator::ZeroRotator);
        if (NewBlock)
        {
            if (MovementParent) NewBlock->AttachToActor(MovementParent, FAttachmentTransformRules::KeepWorldTransform);
            ReplaceFloorsWithBiomes(NewBlock, EBiomeType::Biome1);
            ActiveBlocks.Add(NewBlock);
            LastSpawnedBlock = NewBlock;
        }
    }

    // 2. Pre-generate the initial set of blocks ahead of the player
    for (int32 i = 1; i < MaxBlocksAhead; i++)
    {
        SpawnBlock(FVector(i * LevelBlockLength, 0.0f, 0.0f));
    }
}

void ALevelGeneratorv2::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Advance the game progress
    InternalProgress = FMath::Clamp(InternalProgress + ProgressIncrement, 0.0f, 1.0f);

    if (!LastSpawnedBlock || !MovementParent) return;

    // Check if we need to spawn a new block as the world moves
    float LastBlockX = LastSpawnedBlock->GetActorLocation().X;
    float SpawnThreshold = (MaxBlocksAhead - 1) * LevelBlockLength;

    if (LastBlockX < SpawnThreshold)
    {
        FVector NewPos = LastSpawnedBlock->GetActorLocation() + FVector(LevelBlockLength, 0.0f, 0.0f);
        SpawnBlock(NewPos);
    }

    CleanupBlocks();
}

void ALevelGeneratorv2::SpawnBlock(FVector Location)
{
    // Update biome state first so the block and its floors stay in sync
    UpdateBiomeLogic();

    // Pick a block template that matches the current biome's prop requirements
    TSubclassOf<AActor> BlockClass = GetNextBlockClass(CurrentBiome);

    if (!BlockClass) return;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    AActor* NewBlock = GetWorld()->SpawnActor<AActor>(BlockClass, Location, FRotator::ZeroRotator, Params);

    if (IsValid(NewBlock))
    {
        if (MovementParent) NewBlock->AttachToActor(MovementParent, FAttachmentTransformRules::KeepWorldTransform);

        // Populate the block's floor slots using assets from the chosen biome
        ReplaceFloorsWithBiomes(NewBlock, CurrentBiome);

        ActiveBlocks.Add(NewBlock);
        LastSpawnedBlock = NewBlock;
    }
}

void ALevelGeneratorv2::UpdateBiomeLogic()
{
    // Determine if it's time to switch to a new biome chunk
    if (BlocksInCurrentChunk >= MaxBlocksInChunk)
    {
        BlocksInCurrentChunk = 0;
        MaxBlocksInChunk = FMath::RandRange(2, 3); // 2-3 blocks results in 8-12 floor segments

        // Once progress hits the threshold, we transition to the final Courtyard biome
        if (InternalProgress >= Biome3Threshold)
        {
            CurrentBiome = EBiomeType::Biome3;
        }
        else
        {
            // Otherwise, keep alternating between Study and Dungeon
            CurrentBiome = (CurrentBiome == EBiomeType::Biome1) ? EBiomeType::Biome2 : EBiomeType::Biome1;
        }
    }

    BlocksInCurrentChunk++;
}

void ALevelGeneratorv2::ReplaceFloorsWithBiomes(AActor* ParentBlock, EBiomeType ForBiome)
{
    TArray<UStaticMeshComponent*> MeshComps;
    ParentBlock->GetComponents<UStaticMeshComponent>(MeshComps);

    // Sort floor meshes by X position to ensure they spawn in the correct order
    MeshComps.Sort([](const UStaticMeshComponent& A, const UStaticMeshComponent& B) {
        return A.GetRelativeLocation().X < B.GetRelativeLocation().X;
        });

    for (UStaticMeshComponent* Mesh : MeshComps)
    {
        if (Mesh->GetName().Contains(TEXT("floor")))
        {
            FVector FinalWorldLoc = ParentBlock->GetActorLocation() + FVector(Mesh->GetRelativeLocation().X, 0.0f, 0.0f);

            // Get a random floor asset belonging to the target biome
            TSubclassOf<AActor> BiomeClass = GetRandomBiomeAsset(ForBiome);

            if (BiomeClass)
            {
                FActorSpawnParameters BParams;
                BParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
                AActor* BiomeActor = GetWorld()->SpawnActor<AActor>(BiomeClass, FinalWorldLoc, FRotator::ZeroRotator, BParams);

                if (BiomeActor)
                {
                    BiomeActor->AttachToActor(ParentBlock, FAttachmentTransformRules::KeepWorldTransform);

                    // Hide and disable the placeholder mesh
                    Mesh->SetVisibility(false);
                    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
                }
            }
        }
    }
}

TSubclassOf<AActor> ALevelGeneratorv2::GetRandomBiomeAsset(EBiomeType ForBiome)
{
    TArray<TSubclassOf<AActor>>* Pool = nullptr;
    if (ForBiome == EBiomeType::Biome1) Pool = &Biome1Pool;
    else if (ForBiome == EBiomeType::Biome2) Pool = &Biome2Pool;
    else if (ForBiome == EBiomeType::Biome3) Pool = &Biome3Pool;

    return (Pool && Pool->Num() > 0) ? (*Pool)[FMath::RandRange(0, Pool->Num() - 1)] : nullptr;
}

TSubclassOf<AActor> ALevelGeneratorv2::GetNextBlockClass(EBiomeType ForBiome)
{
    TArray<TSubclassOf<AActor>>* SelectedPool = nullptr;

    if (ForBiome == EBiomeType::Biome1) // Study
    {
        SelectedPool = &StudyDungeonBlocks; // Uses Type 2 (Study-safe props)
    }
    else if (ForBiome == EBiomeType::Biome3) // Courtyard
    {
        SelectedPool = &CourtyardDungeonBlocks; // Uses Type 1 (Courtyard-safe props)
    }
    else // Dungeon (Biome2)
    {
        // Dungeon can safely use blocks from either pool
        bool bUseType1 = FMath::RandBool();
        if (bUseType1 && CourtyardDungeonBlocks.Num() > 0) SelectedPool = &CourtyardDungeonBlocks;
        else SelectedPool = &StudyDungeonBlocks;
    }

    if (SelectedPool && SelectedPool->Num() > 0)
    {
        return (*SelectedPool)[FMath::RandRange(0, SelectedPool->Num() - 1)];
    }
    return nullptr;
}

void ALevelGeneratorv2::CleanupBlocks()
{
    // Remove blocks that have moved far enough behind the camera/player
    for (int32 i = ActiveBlocks.Num() - 1; i >= 0; i--)
    {
        if (IsValid(ActiveBlocks[i]) && ActiveBlocks[i]->GetActorLocation().X < DeletionXThreshold)
        {
            AActor* ToDestroy = ActiveBlocks[i];
            ActiveBlocks.RemoveAt(i);
            ToDestroy->Destroy();
        }
    }
}