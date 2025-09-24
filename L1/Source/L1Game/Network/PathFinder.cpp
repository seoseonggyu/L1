#include "PathFinder.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

APathFinder::APathFinder()
{
    PrimaryActorTick.bCanEverTick = true;

}

void APathFinder::BeginPlay()
{
    Super::BeginPlay();

    NodeArr = GenerateNodes(GetWorld(), _GridSize);
    GenerateEdges(NodeArr, _GridSize);
    SaveEdgesMapToTxt();
}

void APathFinder::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void APathFinder::SaveEdgesMapToTxt()
{
    FString SaveText;

    for (const TPair<FVector, TArray<FVector>>& Pair : EdgesMap)
    {
        const FVector& Key = Pair.Key;
        const TArray<FVector>& Values = Pair.Value;

        SaveText += FString::Printf(TEXT("Key: (%.2f, %.2f, %.2f)\n"), Key.X, Key.Y, Key.Z);

        for (const FVector& Val : Values)
        {
            SaveText += FString::Printf(TEXT("   Value: (%.2f, %.2f, %.2f)\n"), Val.X, Val.Y, Val.Z);
        }
        SaveText += TEXT("\n");
    }

    FString SavePath = FPaths::ProjectDir() / TEXT("EdgesMapOutput.txt");

    if (FFileHelper::SaveStringToFile(SaveText, *SavePath))
    {
        UE_LOG(LogTemp, Log, TEXT("EdgesMap saved to %s"), *SavePath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save EdgesMap"));
    }
}

TArray<FVector> APathFinder::GenerateNodes(UWorld* World, float GridSize)
{
    TArray<FVector> Nodes;

    for (float x = -LevelBoundary; x <= LevelBoundary; x += GridSize)
    {
        for (float y = -LevelBoundary; y <= LevelBoundary; y += GridSize)
        {
            FVector Location(x, y, 0);
            if (IsLocationNavigable(World, Location))
            {
                Nodes.Add(Location);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Obstacle : %f,%f,%f"), Location.X, Location.Y, Location.Z);
            }
        }
    }

    return Nodes;
}

bool APathFinder::IsLocationNavigable(UWorld* World, FVector Location)
{
    FHitResult Hit;
    FVector Start = Location + FVector(0, 0, 100);
    FVector End = Location - FVector(0, 0, 100);
    FCollisionQueryParams Params;

    bool Result = !World->LineTraceSingleByChannel(Hit, Start, End, ECC_GameTraceChannel5, Params);

    if (Result)
        DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 30.f);
    else
        DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 30.f);

    return Result;
}

TMap<FVector, TArray<FVector>> APathFinder::GenerateEdges(const TArray<FVector>& Nodes, float GridSize)
{
    TMap<FVector, TArray<FVector>> Edges;

    for (const FVector& Node : Nodes)
    {
        TArray<FVector> Neighbors;
        for (const FVector& Offset : NeighborOffsets)
        {
            FVector Neighbor = Node + Offset * GridSize;
            if (Nodes.Contains(Neighbor))
            {
                FHitResult Hit;
                FCollisionQueryParams Params;
                bool Result = GetWorld()->LineTraceSingleByChannel(Hit, Node, Neighbor, ECC_GameTraceChannel5, Params);
                if (Result) continue;

                FVector Start = Node;
                FVector End = Neighbor;
                Start.Z += 10;
                End.Z += 10;
                DrawDebugLine(GetWorld(), Start, End, FColor::Black, false, 30.f);
                Neighbors.Add(Neighbor);
            }
        }
        Edges.Add(Node, Neighbors);
    }

    EdgesMap = Edges;

    return Edges;
}