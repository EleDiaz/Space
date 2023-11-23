// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LocationVolume.h"
#include "GameFramework/Actor.h"
#include "InvaderSquad.generated.h"

DECLARE_DELEGATE(FSquadCommandDelegate)
DECLARE_DELEGATE_OneParam(FSquadMessageDelegate, int32)

enum class InvaderMovementType : uint8;

// TODO: The idea is to have a list possible invaders that we can use
//       With this list
UCLASS()
class SPACEINVADERS_API AInvaderSquad : public AActor	
{
	GENERATED_BODY()

public:
	AInvaderSquad();

	// Squad Command 
	FSquadCommandDelegate SquadOnLeftSide;
	FSquadCommandDelegate SquadOnRightSide;
	FSquadCommandDelegate SquadFinishesDown;
	FSquadMessageDelegate SquadDestroyed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void UpdateSquadState(float Delta);

	UFUNCTION(BlueprintCallable)
	int32 GetNumberOfMembers();

protected:
	// Squad movement
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Squad movement")
	float HorizontalVelocity;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Squad movement")
	float VerticalVelocity;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Squad movement")
	InvaderMovementType State;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Squad movement")
	InvaderMovementType PreviousState;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Squad movement")
	float FreeJumpRate;


	// Spawner Options
	// Classes of Invader varying the difficulty
	// Each one has a difficulty number associate, that will be used to generate the enemy row
	// the idea is to use the algorithm of perfect coin change given an amount(difficulty)
	// we have unlimited kind of of ships(coins)
	// 1 coin invader (five rows)
	// 5 coin invader
	// .... We could always fill with 1 coin invader
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Squad Spawner")
	TArray<TSubclassOf<class AInvader>> InvaderClasses;
	
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Squad Spawner")
	TSubclassOf<class AInvader> InvaderClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Squad Spawner")
	class AInvader* InvaderTemplate;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Squad Spawner")
	class ALocationVolume* ReenterVolume;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Squad Spawner")
	class ALocationVolume* LocationVolume;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Squad Spawner")
	float ExtraSeparation;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class ASIGameModeBase* MyGameMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<class AInvader*> SquadMembers;

	
private:
	// Private Attributes
	UPROPERTY()
	class UBillboardComponent* BillboardComponent;
	
	int32 numberOfMembers;

	UPROPERTY(VisibleAnywhere)
	float timeFromLastFreeJump;

	void NextActionSquadSquadOnLeftSide();

	void NextActionSquadOnRightSide();

	void NextActionSquadFinishesDown();

	void RemoveInvader(int32 ind);

	// Values for initializing defaults
	static constexpr float defaultHorizontalVelocity = 1000.0f;
	static constexpr float defaultVerticalVelocity = 1000.0f;
	static constexpr float defaultExtraSeparation = 0.0f;
	static constexpr float defaultPercentVolumeUsage = 0.9;
};
