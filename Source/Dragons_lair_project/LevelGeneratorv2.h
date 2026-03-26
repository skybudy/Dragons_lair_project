#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGeneratorv2.generated.h"

UCLASS()
class DRAGONS_LAIR_PROJECT_API ALevelGeneratorv2 : public AActor
{
    GENERATED_BODY()

public:
    ALevelGeneratorv2();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    /** Movement Actor that drives the world towards the player */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Setup")
    AActor* MovementParent;

    /** * Block Blueprints:
     * Index 0: STARTER BLOCK (Spawns only once at the start)
     * Index 1 to N: VARIATION BLOCKS (Spawned randomly in a shuffled sequence)
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Setup")
    TArray<TSubclassOf<AActor>> BlockTemplates;

    /** Fixed length of a single block in Unreal Units (e.g., 1500 for 15 meters) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Settings")
    float BlockLength = 1500.0f;

    /** Number of blocks maintained ahead of the player */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Settings")
    int32 MaxBlocksAhead = 8;

    /** X Coordinate behind the player where blocks get destroyed */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runner | Settings")
    float DeletionXThreshold = -3000.0f;

private:
    /** List of indices for Variation Blocks to ensure unique random order */
    TArray<int32> ShuffleBag;

    /** Current list of spawned blocks */
    UPROPERTY()
    TArray<AActor*> ActiveBlocks;

    /** Reference to the last spawned block to calculate next spawn position */
    UPROPERTY()
    AActor* LastSpawnedBlock;

    /** Core function to spawn a block at a specific world location */
    void SpawnBlock(TSubclassOf<AActor> BlockClass, FVector Location);

    /** Returns a random index from BlockTemplates (excluding index 0) with no repeats until full cycle is done */
    int32 GetNextBlockIndex();

    /** Removes blocks that have moved past the player */
    void CleanupBlocks();
};