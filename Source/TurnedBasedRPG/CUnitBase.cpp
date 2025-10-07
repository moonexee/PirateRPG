// Fill out your copyright notice in the Description page of Project Settings.
#include "CUnitBase.h"

// Sets default values
ACUnitBase::ACUnitBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACUnitBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACUnitBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
   
    if (bIsMoving && CurrentPath.IsValidIndex(PathIndex))
    {
        const FVector TargetLocation = Pathfinder->Grid[CurrentPath[PathIndex]].GridPosition + FVector(0, 0, 70.f);
        FVector CurrentLocation = GetActorLocation();
        FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
        
        FRotator NewRotation = Direction.Rotation();
        SetActorRotation(NewRotation);

        FVector NewLocation = CurrentLocation + Direction * MoveSpeed * DeltaTime;

        //snap to target if we're close enough
        if (FVector::Dist(NewLocation, TargetLocation) < 5.0f)
        {
            SetActorLocation(TargetLocation);
            ++PathIndex;

            if (!CurrentPath.IsValidIndex(PathIndex))
            {
                //done moving
                bIsMoving = false;
                
                //update the pathfinder grid: clear old cell
                int32 OldID = CurrentID;
                if (Pathfinder->Grid.IsValidIndex(OldID))
                    Pathfinder->Grid[OldID].Occupier = nullptr;

                CurrentID = CurrentPath.Last();
                Pathfinder->Grid[CurrentID].Occupier = this;

                OnMovementFinished();
            }
        }
        else
        {
            SetActorLocation(NewLocation);
        }
    }
}

int32 ACUnitBase::MoveAlongPath(const TArray<int32>& Path, APathfinder* PathfinderRef)
{
    if (Path.Num() == 0 || !PathfinderRef)
        return -1;

    Pathfinder = PathfinderRef;
    CurrentPath = Path;
    PathIndex = 0;
    bIsMoving = true;

    return CurrentPath.Last(); //final destination cell ID
}


void ACUnitBase::MakeDecision_Implementation()
{

}


void ACUnitBase::PerformAttack(EAttackType AttackType, int TileClicked)
{
    OnAttackTriggered(CurrentAttackType, TileClicked);
}

void ACUnitBase::TakeDamage(int32 DamageAmount)
{
    UnitStats.Health -= DamageAmount;
    UnitStats.Health += UnitStats.Defense;
}


void ACUnitBase::GainXP()
{
    UnitStats.XP += 15;

    if (UnitStats.XP >= 100)
    {
        LevelUp();
        UnitStats.XP = 0;
    }
}

void ACUnitBase::LevelUp()
{
    UnitStats.Attack += 5;
    UnitStats.Defense += 2;
    UnitStats.Health += 10;
    UnitStats.Mana += 2;
    UnitStats.Level++;
}

// Called to bind functionality to input
void ACUnitBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

