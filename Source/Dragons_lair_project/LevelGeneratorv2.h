#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGeneratorv2.generated.h"

UENUM(BlueprintType)
enum class EBiomeType : uint8
{
    Biome1, Biome2, Biome3
};

UCLASS()
class DRAGONS_LAIR_PROJECT_API ALevelGeneratorv2 : public AActor
{
    GENERATED_BODY()

public:
    ALevelGeneratorv2();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Setup")
    AActor* MovementParent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Level Blocks")
    TArray<TSubclassOf<AActor>> BlockTemplates;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Biomes")
    TArray<TSubclassOf<AActor>> Biome1Pool;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Biomes")
    TArray<TSubclassOf<AActor>> Biome2Pool;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Biomes")
    TArray<TSubclassOf<AActor>> Biome3Pool;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Progress")
    float ProgressIncrement = 0.00025f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Progress")
    float Biome3Threshold = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Settings")
    float LevelBlockLength = 3600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Settings")
    int32 MaxBlocksAhead = 4;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Settings")
    float DeletionXThreshold = -3000.0f;

private:
    TArray<int32> ShuffleBag;

    UPROPERTY()
    TArray<AActor*> ActiveBlocks;

    UPROPERTY()
    AActor* LastSpawnedBlock;

    EBiomeType CurrentBiome;
    int32 FloorsInCurrentChunk;
    int32 MaxFloorsInChunk;
    float InternalProgress;

    void SpawnBlock(TSubclassOf<AActor> BlockClass, FVector Location);
    int32 GetNextBlockIndex();
    TSubclassOf<AActor> GetNextBiomeClass();
    void ReplaceFloorsWithBiomes(AActor* ParentBlock);
    void CleanupBlocks();
};