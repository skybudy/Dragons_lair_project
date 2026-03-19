#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralGenerator.generated.h"

// Types of generation patterns used when spawning tiles
UENUM(BlueprintType)
enum class EPatternType : uint8
{
    CoinsLine,         // Straight line of coins
    CoinsWave,         // Sinusoidal wave of coins
    CoinsSnake,        // Random lane-switching coin pattern
    ObstacleWall,      // Wall of obstacles with one open lane
    DoubleObstacles,   // Two obstacles placed at different X positions
    RandomObstacles    // Random number of obstacles in random lanes
};

UCLASS()
class DRAGONS_LAIR_PROJECT_API AProceduralGenerator : public AActor
{
    GENERATED_BODY()

public:
    AProceduralGenerator();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // === SETTINGS ===

    // List of obstacle types that can be spawned
    UPROPERTY(EditAnywhere, Category = "Procedural")
    TArray<TSubclassOf<AActor>> ObstacleClasses;

    // Coin actor class to spawn
    UPROPERTY(EditAnywhere, Category = "Procedural")
    TSubclassOf<AActor> CoinClass;

    // Parent actor to which all spawned objects will be attached (moves the whole world)
    UPROPERTY(EditAnywhere, Category = "Procedural")
    AActor* MovementRoot;

    // Length of one tile segment along the X axis
    UPROPERTY(EditAnywhere, Category = "Procedural")
    float TileLength = 1000.f;

    // Time interval between spawning new tiles
    UPROPERTY(EditAnywhere, Category = "Procedural")
    float SpawnInterval = 5.f;

    // X threshold behind the player where objects get destroyed
    UPROPERTY(EditAnywhere, Category = "Procedural")
    float DespawnBehindX = -2000.f;

    // Base Z height for coins and obstacles
    UPROPERTY(EditAnywhere, Category = "Procedural")
    float FloorZ = 0.f;

    // Vertical offset for coins
    UPROPERTY(EditAnywhere, Category = "Procedural")
    float CoinHeight = 50.f;

    // Vertical offset for obstacles (helps align them visually)
    UPROPERTY(EditAnywhere, Category = "Procedural")
    float ObstacleHeightOffset = 50.f;

    // Y positions for lanes (e.g., left, center, right)
    UPROPERTY(EditAnywhere, Category = "Procedural")
    TArray<float> LaneOffsetsY;

    // Minimum number of obstacles guaranteed per tile
    UPROPERTY(EditAnywhere, Category = "Procedural")
    int32 MinObstaclesPerTile = 1;

    // Maximum number of obstacles allowed per tile
    UPROPERTY(EditAnywhere, Category = "Procedural")
    int32 MaxObstaclesPerTile = 3;

    // Maximum number of coins spawned in coin patterns
    UPROPERTY(EditAnywhere, Category = "Procedural")
    int32 MaxCoinsPerTile = 10;

private:
    // X position where the next tile will be spawned
    float NextSpawnX;

    // List of all spawned actors for cleanup
    TArray<AActor*> SpawnedActors;

    // Timer handle for repeated tile spawning
    FTimerHandle SpawnTimerHandle;

    // Initial batch of tiles at game start
    void SpawnInitialTiles();

    // Spawn a single tile with a random pattern
    void SpawnTile();

    // Destroy actors that moved behind the despawn threshold
    void CleanupBehind();

    // === PATTERN FUNCTIONS ===
    void SpawnPattern_CoinsLine(float TileX);
    void SpawnPattern_CoinsWave(float TileX);
    void SpawnPattern_CoinsSnake(float TileX);
    void SpawnPattern_ObstacleWall(float TileX);
    void SpawnPattern_DoubleObstacles(float TileX);
    void SpawnPattern_RandomObstacles(float TileX);

    // === SPAWN HELPERS ===
    void SpawnCoin(FVector Location);
    void SpawnObstacle(FVector Location);
};
