#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGeneratorv2.generated.h"

UENUM(BlueprintType)
enum class EBiomeType : uint8
{
    Biome1, // Study
    Biome2, // Dungeon
    Biome3  // Courtyard
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

    // The actor that moves everything; usually the player or a dedicated mover
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Setup")
    AActor* MovementParent;

    // --- Level Block Configuration ---

    // The very first block spawned at the start of the run
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Level Blocks")
    TSubclassOf<AActor> StartBlockTemplate;

    // Type 1: Blocks containing props suitable for Courtyard and Dungeon
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Level Blocks")
    TArray<TSubclassOf<AActor>> CourtyardDungeonBlocks;

    // Type 2: Blocks containing props suitable for Study and Dungeon
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Level Blocks")
    TArray<TSubclassOf<AActor>> StudyDungeonBlocks;

    // ------------------------------

    // Pools of biome floor assets for each environment type
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Biomes")
    TArray<TSubclassOf<AActor>> Biome1Pool; // Study

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Biomes")
    TArray<TSubclassOf<AActor>> Biome2Pool; // Dungeon

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Biomes")
    TArray<TSubclassOf<AActor>> Biome3Pool; // Courtyard

    // How fast the run progresses towards the final biome
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Progress")
    float ProgressIncrement = 0.00025f;

    // Progress value (0.0 to 1.0) required to trigger the Courtyard biome
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Progress")
    float Biome3Threshold = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Settings")
    float LevelBlockLength = 3600.0f;

    // Number of blocks to keep active in front of the player
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Settings")
    int32 MaxBlocksAhead = 4;

    // X-coordinate threshold after which blocks are destroyed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Settings")
    float DeletionXThreshold = -3000.0f;

private:
    UPROPERTY()
    TArray<AActor*> ActiveBlocks;

    UPROPERTY()
    AActor* LastSpawnedBlock;

    EBiomeType CurrentBiome;
    int32 BlocksInCurrentChunk; // Tracking how many blocks we've spawned in the current biome
    int32 MaxBlocksInChunk;     // Duration of the current biome chunk in blocks
    float InternalProgress;

    void SpawnBlock(FVector Location);
    TSubclassOf<AActor> GetNextBlockClass(EBiomeType ForBiome);
    TSubclassOf<AActor> GetRandomBiomeAsset(EBiomeType ForBiome);
    void ReplaceFloorsWithBiomes(AActor* ParentBlock, EBiomeType ForBiome);
    void UpdateBiomeLogic();
    void CleanupBlocks();
};