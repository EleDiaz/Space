// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InvaderSquad.generated.h"

enum class InvaderMovementType : uint8;

// TODO: The idea is to have a list possible invaders that we can use
//       With this list
UCLASS()
class SPACEINVADERS_API AInvaderSquad : public AActor
{
	GENERATED_BODY()

public:
	AInvaderSquad();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

public:
	//--------------------------------------------------------
	// Root Scene Component to modify location, rotation,...
	//--------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USceneComponent* Root;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void UpdateSquadState(float Delta);

	UFUNCTION(BlueprintCallable)
	void SetRows(int32 Rows);

	UFUNCTION(BlueprintCallable)
	void SetCols(int32 Cols);

	UFUNCTION(BlueprintCallable)
	int32 GetRows();

	UFUNCTION(BlueprintCallable)
	int32 GetCols();

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
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Squad Spawner")
	TSubclassOf<class AInvader> InvaderClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Squad Spawner")
	class AInvader* InvaderTemplate;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Squad Spawner")
	class ALocationVolume* ReenterVolume;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Squad Spawner")
	class ALocationVolume* LocationVolume;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Squad Spawner")
	int32 Rows;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Squad Spawner")
	int32 Cols;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Squad Spawner")
	float ExtraSeparation;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class ASIGameModeBase* MyGameMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<class AInvader*> SquadMembers;

private:
	int32 numberOfMembers;

	UPROPERTY(VisibleAnywhere)
	float timeFromLastFreeJump;

	void SquadOnLeftSide();

	void SquadOnRightSide();

	void SquadFinishesDown();

	void RemoveInvader(int32 ind);

	// Values for initializing defaults
	static const int32 defaultNRows = 1;
	static const int32 defaultNCols = 1;
	static constexpr float defaultHorizontalVelocity = 1000.0f;
	static constexpr float defaultVerticalVelocity = 1000.0f;
	static constexpr float defaultExtraSeparation = 0.0f;
	static constexpr float defaultPercentVolumeUsage = 0.9;
};
