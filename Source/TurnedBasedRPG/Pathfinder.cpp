#include "Pathfinder.h"
#include "Kismet/GameplayStatics.h"

APathfinder::APathfinder()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APathfinder::BeginPlay()
{
	Super::BeginPlay();
}

struct FPathNode
{
	int32 ID; //cell id
	TSharedPtr<FPathNode> Parent; //previous node path
	float G; //cost so far (distance traveled)
	float H; //estimated cost to goal cell

	float F() const { return G + H; } //total cost
};

TArray<int32> APathfinder::ComputePath(int32 StartID, int32 GoalID, int32 Limit)
{
	TArray<TSharedPtr<FPathNode>> OpenSet; //nodes left to explore
	TArray<TSharedPtr<FPathNode>> ClosedSet; //nodes already explored

	auto StartNode = MakeShared<FPathNode>();
	StartNode->ID = StartID;
	StartNode->G = 0.0f;
	StartNode->H = 0.0f;

	OpenSet.Add(StartNode);

	int32 Steps = 0;

	while (OpenSet.Num() > 0 && Steps++ < MaxSearchSteps)
	{
		//sort by lowest f-cost
		OpenSet.Sort([](const TSharedPtr<FPathNode>& A, const TSharedPtr<FPathNode>& B) 
			{
			return A->F() < B->F();
			});

		//take current best node
		TSharedPtr<FPathNode> Current = OpenSet[0];
		OpenSet.RemoveAt(0);
		ClosedSet.Add(Current);

		//goal reached -> reconstruct path by walking back through parents
		if (Current->ID == GoalID)
		{
			TArray<int32> Result;
			while (Current->Parent.IsValid())
			{
				Result.Insert(Current->ID, 0); //insert path step at front
				Current = Current->Parent;
			}
			//optional length limit
			if (Limit > 0 && Result.Num() > Limit)
				Result.SetNum(Limit);
			return Result;
		}

		//explore neighbors
		const FGridCell& Cell = Grid[Current->ID];

		for (int32 NeighborID : Cell.LinkedCells)
		{
			if (Grid[NeighborID].Occupier) continue;

			if (ClosedSet.ContainsByPredicate([&](const TSharedPtr<FPathNode>& N) { return N->ID == NeighborID; })) //skip if already explored
				continue;

			float MovementCost = Current->G + FVector::Dist(Cell.GridPosition, Grid[NeighborID].GridPosition) * Grid[NeighborID].Cost; //cost to move into this neighbor

			TSharedPtr<FPathNode> ExistingNode = OpenSet.FindByPredicate([&](const TSharedPtr<FPathNode>& N) //check if node already exists in OpenSet
				{
				return N->ID == NeighborID;
				}) ? *OpenSet.FindByPredicate([&](const TSharedPtr<FPathNode>& N) 
					{
					return N->ID == NeighborID;
					}) : nullptr;

				if (!ExistingNode.IsValid()) //if not already in OpenSet -> add it
				{
					auto NewNode = MakeShared<FPathNode>();
					NewNode->ID = NeighborID;
					NewNode->Parent = Current;
					NewNode->G = MovementCost;
					NewNode->H = FVector::Dist(Grid[NeighborID].GridPosition, Grid[GoalID].GridPosition);
					OpenSet.Add(NewNode);
				}
				else if (MovementCost < ExistingNode->G) //if already exists but cheaper path found -> update it
				{
					ExistingNode->G = MovementCost;
					ExistingNode->Parent = Current;
				}
		}
	}

	return {}; //no path
}

TArray<int32> APathfinder::GetReachableCells(int32 OriginID, int32 Range)
{
	TArray<int32> Reachable;

	for (int32 i = 0; i < Grid.Num(); i++)
	{
		if (i == OriginID)
			continue;

		//if cell is occupied (player/enemy)
		if (Grid[i].Occupier)
		{
			//checks if at least 1 neighboring cell is available
			const FGridCell& OccupiedCell = Grid[i];
			for (int32 NeighborID : OccupiedCell.LinkedCells)
			{
				TArray<int32> Path = ComputePath(OriginID, NeighborID);
				if (Path.Num() > 0 && Path.Num() <= Range)
				{
					Reachable.Add(i); //set the occupied cell as reachable
					break;
				}
			}
		}
		else
		{
			//free cell -> normal testing
			TArray<int32> Path = ComputePath(OriginID, i);
			if (Path.Num() > 0 && Path.Num() <= Range)
			{
				Reachable.Add(i);
			}
		}
	}

	return Reachable;
}

TArray<int32> APathfinder::GetAdjacentCells(int32 FromID)
{
	if (!Grid.IsValidIndex(FromID)) return {}; //returns direct neighbors of given cell
	return Grid[FromID].LinkedCells;
}


void APathfinder::InitializeGrid()
{
	Grid.Sort([](const FGridCell& A, const FGridCell& B) //sort by ID so Grid[i].ID == i
		{
		return A.ID < B.ID;
		});

	for (FGridCell& Cell : Grid) //compare with all other cells to find valid neighbors
	{

		for (const FGridCell& Other : Grid)
		{
			for (const FVector& Offset : TArray<FVector>{ FVector(110,0,0), FVector(-110,0,0), FVector(0,110,0), FVector(0,-110,0) }) //only direct connections (up, down, left, right)
			{
				if ((Cell.GridPosition + Offset).Equals(Other.GridPosition, 1.0f))
				{
					Cell.LinkedCells.AddUnique(Other.ID);
				}
			}
		}
	}
}