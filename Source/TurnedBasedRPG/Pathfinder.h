#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pathfinder.generated.h"

USTRUCT(BlueprintType)
struct FGridCell
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ID; //cell id

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector GridPosition; //world pos of cel

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Cost = 1; //movement cost

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> LinkedCells; //neighboring cells

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Occupier = nullptr; //pointer to actor actively standing on cell
};

UCLASS()
class TURNEDBASEDRPG_API APathfinder : public AActor
{
	GENERATED_BODY()

public:
	APathfinder();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FGridCell> Grid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxSearchSteps = 150;

	UFUNCTION(BlueprintCallable)
	TArray<int32> ComputePath(int32 StartID, int32 GoalID, int32 Limit = -1);

	UFUNCTION(BlueprintCallable)
	TArray<int32> GetReachableCells(int32 OriginID, int32 Range);

	UFUNCTION(BlueprintCallable)
	TArray<int32> GetAdjacentCells(int32 FromID);

	UFUNCTION(BlueprintCallable)
	void InitializeGrid();
};