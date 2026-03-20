#include "ProceduralGenerator.h"
#include "Engine/World.h"
#include "TimerManager.h"

AProceduralGenerator::AProceduralGenerator()
{
    // Enable ticking every frame
    PrimaryActorTick.bCanEverTick = true;
}

void AProceduralGenerator::BeginPlay()
{
    Super::BeginPlay();

    // Default lane offsets if none were set
    if (LaneOffsetsY.Num() == 0)
        LaneOffsetsY = { -200.f, 0.f, 200.f };

    // Starting X position for spawning tiles
    NextSpawnX = 0.f;

    // Spawn a few tiles at the beginning
    SpawnInitialTiles();

    // Repeatedly spawn tiles using a timer
    GetWorld()->GetTimerManager().SetTimer(
        SpawnTimerHandle,
        this,
        &AProceduralGenerator::SpawnTile,
        SpawnInterval,
        true
    );
}

void AProceduralGenerator::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Remove old actors behind the player
    CleanupBehind();
}

void AProceduralGenerator::SpawnInitialTiles()
{
    // Spawn several tiles at game start
    for (int i = 0; i < 3; i++)
        SpawnTile();
}

void AProceduralGenerator::SpawnTile()
{
    // Determine X position for this tile
    float TileX = NextSpawnX;
    NextSpawnX += TileLength;

    // Randomly choose a pattern type
    int32 PatternIndex = FMath::RandRange(0, 5);
    EPatternType Pattern = static_cast<EPatternType>(PatternIndex);

    // Execute the selected pattern
    switch (Pattern)
    {
    case EPatternType::CoinsLine:        SpawnPattern_CoinsLine(TileX); break;
    case EPatternType::CoinsWave:        SpawnPattern_CoinsWave(TileX); break;
    case EPatternType::CoinsSnake:       SpawnPattern_CoinsSnake(TileX); break;
    case EPatternType::ObstacleWall:     SpawnPattern_ObstacleWall(TileX); break;
    case EPatternType::DoubleObstacles:  SpawnPattern_DoubleObstacles(TileX); break;
    case EPatternType::RandomObstacles:  SpawnPattern_RandomObstacles(TileX); break;
    }

    // Guarantee: spawn at least a minimum number of obstacles
    if (MinObstaclesPerTile > 0)
    {
        int Count = FMath::RandRange(MinObstaclesPerTile, MaxObstaclesPerTile);

        for (int i = 0; i < Count; i++)
        {
            int Lane = FMath::RandRange(0, LaneOffsetsY.Num() - 1);
            float X = TileX + FMath::FRandRange(200.f, TileLength - 200.f);

            SpawnObstacle({ X, LaneOffsetsY[Lane], FloorZ + ObstacleHeightOffset });
        }
    }
}

void AProceduralGenerator::CleanupBehind()
{
    // Iterate backwards to safely remove elements
    for (int32 i = SpawnedActors.Num() - 1; i >= 0; --i)
    {
        AActor* A = SpawnedActors[i];

        // Remove invalid actors
        if (!IsValid(A))
        {
            SpawnedActors.RemoveAt(i);
            continue;
        }

        // Destroy actors that moved behind the despawn threshold
        if (A->GetActorLocation().X < DespawnBehindX)
        {
            A->Destroy();
            SpawnedActors.RemoveAt(i);
        }
    }
}

// === PATTERNS ===

void AProceduralGenerator::SpawnPattern_CoinsLine(float TileX)
{
    if (!CoinClass) return;

    // Choose a random lane
    int Lane = FMath::RandRange(0, LaneOffsetsY.Num() - 1);
    float Y = LaneOffsetsY[Lane];

    // Spawn coins evenly spaced in a straight line
    for (int i = 0; i < MaxCoinsPerTile; i++)
    {
        float X = TileX + i * (TileLength / MaxCoinsPerTile);
        SpawnCoin({ X, Y, FloorZ + CoinHeight });
    }
}

void AProceduralGenerator::SpawnPattern_CoinsWave(float TileX)
{
    if (!CoinClass) return;

    // Spawn coins in a sinusoidal wave pattern
    for (int i = 0; i < MaxCoinsPerTile; i++)
    {
        float X = TileX + i * (TileLength / MaxCoinsPerTile);
        float Y = FMath::Sin(i * 0.5f) * 200.f;
        SpawnCoin({ X, Y, FloorZ + CoinHeight });
    }
}

void AProceduralGenerator::SpawnPattern_CoinsSnake(float TileX)
{
    if (!CoinClass) return;

    // Start in a random lane
    int Lane = FMath::RandRange(0, LaneOffsetsY.Num() - 1);

    // Every few coins, switch to a new random lane
    for (int i = 0; i < MaxCoinsPerTile; i++)
    {
        if (i % 3 == 0)
            Lane = FMath::RandRange(0, LaneOffsetsY.Num() - 1);

        float X = TileX + i * (TileLength / MaxCoinsPerTile);
        float Y = LaneOffsetsY[Lane];

        SpawnCoin({ X, Y, FloorZ + CoinHeight });
    }
}

void AProceduralGenerator::SpawnPattern_ObstacleWall(float TileX)
{
    if (ObstacleClasses.Num() == 0) return;

    // Choose one lane to leave open
    int HoleLane = FMath::RandRange(0, LaneOffsetsY.Num() - 1);

    // Spawn obstacles in all lanes except the hole
    for (int Lane = 0; Lane < LaneOffsetsY.Num(); Lane++)
    {
        if (Lane == HoleLane) continue;

        float X = TileX + TileLength * 0.5f;
        float Y = LaneOffsetsY[Lane];

        SpawnObstacle({ X, Y, FloorZ + ObstacleHeightOffset });
    }
}

void AProceduralGenerator::SpawnPattern_DoubleObstacles(float TileX)
{
    if (ObstacleClasses.Num() == 0) return;

    // Choose two random lanes
    int Lane1 = FMath::RandRange(0, LaneOffsetsY.Num() - 1);
    int Lane2 = FMath::RandRange(0, LaneOffsetsY.Num() - 1);

    // Spawn two obstacles at different X positions
    float X1 = TileX + TileLength * 0.3f;
    float X2 = TileX + TileLength * 0.7f;

    SpawnObstacle({ X1, LaneOffsetsY[Lane1], FloorZ + ObstacleHeightOffset });
    SpawnObstacle({ X2, LaneOffsetsY[Lane2], FloorZ + ObstacleHeightOffset });
}

void AProceduralGenerator::SpawnPattern_RandomObstacles(float TileX)
{
    if (ObstacleClasses.Num() == 0) return;

    // Random number of obstacles
    int Count = FMath::RandRange(1, MaxObstaclesPerTile);

    for (int i = 0; i < Count; i++)
    {
        int Lane = FMath::RandRange(0, LaneOffsetsY.Num() - 1);
        float X = TileX + FMath::FRandRange(200.f, TileLength - 200.f);

        SpawnObstacle({ X, LaneOffsetsY[Lane], FloorZ + ObstacleHeightOffset });
    }
}

// === SPAWNING ===

void AProceduralGenerator::SpawnCoin(FVector Location)
{
    if (!CoinClass || !MovementRoot) return;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // Spawn coin actor
    AActor* Coin = GetWorld()->SpawnActor<AActor>(CoinClass, Location, FRotator::ZeroRotator, Params);
    if (Coin)
    {
        // Attach to movement root so everything moves together
        Coin->AttachToActor(MovementRoot, FAttachmentTransformRules::KeepWorldTransform);
        SpawnedActors.Add(Coin);
    }
}

void AProceduralGenerator::SpawnObstacle(FVector Location)
{
    if (ObstacleClasses.Num() == 0 || !MovementRoot) return;

    // Pick a random obstacle class
    int Index = FMath::RandRange(0, ObstacleClasses.Num() - 1);
    TSubclassOf<AActor> Class = ObstacleClasses[Index];

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // Spawn obstacle actor
    AActor* Obs = GetWorld()->SpawnActor<AActor>(Class, Location, FRotator::ZeroRotator, Params);
    if (Obs)
    {
        Obs->AttachToActor(MovementRoot, FAttachmentTransformRules::KeepWorldTransform);
        SpawnedActors.Add(Obs);
    }
}
