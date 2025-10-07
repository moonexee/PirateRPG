#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UObject/ObjectMacros.h"
#include "Pathfinder.h"
#include "CUnitBase.generated.h"


USTRUCT(BlueprintType)
struct FUnitStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Health = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 XP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Attack = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 AttackRange = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Defense = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Mana = 5;

};

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	Melee 	UMETA(DisplayName = "Melee"),
	Ranged 	UMETA(DisplayName = "Gun"),
	Special UMETA(DisplayName = "Special")
};

UCLASS()
class TURNEDBASEDRPG_API ACUnitBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACUnitBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn))
	int32 CurrentID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUnitStats UnitStats;

	UFUNCTION(BlueprintCallable)
	int32 MoveAlongPath(const TArray<int32>& Path, APathfinder* PathfinderRef);

	UFUNCTION(BlueprintImplementableEvent)
	void OnMovementFinished();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void MakeDecision(); //called by turnManager in BP for all enemy units

	virtual void MakeDecision_Implementation();

	UFUNCTION(BlueprintCallable)
	virtual void PerformAttack(EAttackType AttackType, int TileClicked);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttackType CurrentAttackType;

	UFUNCTION(BlueprintImplementableEvent)
	void OnAttackTriggered(EAttackType Type, int TileClicked); //called by performattack() which is used in the unit's bp

	UFUNCTION(BlueprintCallable)
	virtual void TakeDamage(int32 DamageAmount);

	UFUNCTION(BlueprintCallable)
	void GainXP();

	UFUNCTION(BlueprintCallable)
	void LevelUp();


protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	TArray<int32> CurrentPath;
	APathfinder* Pathfinder; //pointer to pathfinder
	int32 PathIndex = 0;
	bool bIsMoving = false;
	float MoveSpeed = 200.f; //units per second
};
