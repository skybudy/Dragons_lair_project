#include "LevelGeneratorv2.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

ALevelGeneratorv2::ALevelGeneratorv2()
{
    PrimaryActorTick.bCanEverTick = true;
    CurrentBiome = EBiomeType::Biome1;
    InternalProgress = 0.0f;
    FloorsInCurrentChunk = 0;
    MaxFloorsInChunk = 8;
}

void ALevelGeneratorv2::BeginPlay()
{
    Super::BeginPlay();
    if (BlockTemplates.Num() < 2) return;

    SpawnBlock(BlockTemplates[0], FVector::ZeroVector);
    for (int32 i = 1; i < MaxBlocksAhead; i++)
    {
        SpawnBlock(BlockTemplates[GetNextBlockIndex()], FVector(i * LevelBlockLength, 0.0f, 0.0f));
    }
}

void ALevelGeneratorv2::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    InternalProgress = FMath::Clamp(InternalProgress + ProgressIncrement, 0.0f, 1.0f);

    if (!LastSpawnedBlock || !MovementParent) return;

    float LastBlockX = LastSpawnedBlock->GetActorLocation().X;
    float SpawnThreshold = (MaxBlocksAhead - 1) * LevelBlockLength;

    if (LastBlockX < SpawnThreshold)
    {
        FVector NewPos = LastSpawnedBlock->GetActorLocation() + FVector(LevelBlockLength, 0.0f, 0.0f);
        SpawnBlock(BlockTemplates[GetNextBlockIndex()], NewPos);
    }
    CleanupBlocks();
}

void ALevelGeneratorv2::SpawnBlock(TSubclassOf<AActor> BlockClass, FVector Location)
{
    if (!BlockClass) return;
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    AActor* NewBlock = GetWorld()->SpawnActor<AActor>(BlockClass, Location, FRotator::ZeroRotator, Params);

    if (IsValid(NewBlock))
    {
        if (MovementParent) NewBlock->AttachToActor(MovementParent, FAttachmentTransformRules::KeepWorldTransform);
        ReplaceFloorsWithBiomes(NewBlock);
        ActiveBlocks.Add(NewBlock);
        LastSpawnedBlock = NewBlock;
    }
}

void ALevelGeneratorv2::ReplaceFloorsWithBiomes(AActor* ParentBlock)
{
    TArray<UStaticMeshComponent*> MeshComps;
    ParentBlock->GetComponents<UStaticMeshComponent>(MeshComps);
    MeshComps.Sort([](const UStaticMeshComponent& A, const UStaticMeshComponent& B) {
        return A.GetRelativeLocation().X < B.GetRelativeLocation().X;
        });

    for (UStaticMeshComponent* Mesh : MeshComps)
    {
        if (Mesh->GetName().Contains(TEXT("floor")))
        {
            FVector FinalWorldLoc = ParentBlock->GetActorLocation() + FVector(Mesh->GetRelativeLocation().X, 0.0f, 0.0f);
            TSubclassOf<AActor> BiomeClass = GetNextBiomeClass();
            if (BiomeClass)
            {
                FActorSpawnParameters BParams;
                BParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
                AActor* BiomeActor = GetWorld()->SpawnActor<AActor>(BiomeClass, FinalWorldLoc, FRotator::ZeroRotator, BParams);
                if (BiomeActor)
                {
                    BiomeActor->AttachToActor(ParentBlock, FAttachmentTransformRules::KeepWorldTransform);
                    Mesh->SetVisibility(false);
                    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
                }
            }
        }
    }
}

TSubclassOf<AActor> ALevelGeneratorv2::GetNextBiomeClass()
{
    if (InternalProgress >= Biome3Threshold) CurrentBiome = EBiomeType::Biome3;
    else
    {
        if (FloorsInCurrentChunk >= MaxFloorsInChunk)
        {
            CurrentBiome = (CurrentBiome == EBiomeType::Biome1) ? EBiomeType::Biome2 : EBiomeType::Biome1;
            FloorsInCurrentChunk = 0;
            MaxFloorsInChunk = FMath::RandRange(6, 10);
        }
        FloorsInCurrentChunk++;
    }

    TArray<TSubclassOf<AActor>>* Pool = &Biome1Pool;
    if (CurrentBiome == EBiomeType::Biome2) Pool = &Biome2Pool;
    else if (CurrentBiome == EBiomeType::Biome3) Pool = &Biome3Pool;

    return (Pool && Pool->Num() > 0) ? (*Pool)[FMath::RandRange(0, Pool->Num() - 1)] : nullptr;
}

int32 ALevelGeneratorv2::GetNextBlockIndex()
{
    if (BlockTemplates.Num() < 2) return 0;
    if (ShuffleBag.Num() == 0)
    {
        for (int32 i = 1; i < BlockTemplates.Num(); i++) ShuffleBag.Add(i);
        for (int32 i = ShuffleBag.Num() - 1; i > 0; i--) ShuffleBag.Swap(i, FMath::RandRange(0, i));
    }
    return ShuffleBag.Pop();
}

void ALevelGeneratorv2::CleanupBlocks()
{
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