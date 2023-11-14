// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LocationVolume.h"

#include "SIGameModeBase.generated.h"


// Delegates of this game:
DECLARE_DELEGATE(FStandardDelegateSignature)
DECLARE_MULTICAST_DELEGATE_OneParam(FOneParamMulticastDelegateSignature, int32);
DECLARE_MULTICAST_DELEGATE(FMulticastDelegateSignature);
DECLARE_DELEGATE_OneParam(FOneParamDelegateSignature, int32)

/**
 *
 */
UCLASS()
class SPACEINVADERS_API ASIGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	FStandardDelegateSignature SquadOnLeftSide; // Invader-> Squad
	FStandardDelegateSignature SquadOnRightSide; // Invader -> Squad
	FStandardDelegateSignature SquadFinishesDown; // Invader -> Squad

	//
	FStandardDelegateSignature SquadSuccessful; // Invader -> GameMode, Player

	FOneParamMulticastDelegateSignature InvaderDestroyed; // Invader -> Squad Invader->Player

	// Squad -> GameMode, Player
	// Squad Destroyed
	FMulticastDelegateSignature SquadDestroyed;

	// Player Dies
	FStandardDelegateSignature PlayerZeroLifes; // Player -> Game Mode

	//------------------------------------------------
	//Spawned squad
	//------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Level Layout")
	TSubclassOf<class AInvaderSquad> InvaderSquadClass;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Level Layout")
	ALocationVolume* SpawnableLocation;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Level Layout")
	ALocationVolume* TopReenterSpawn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Level Layout")
	class AInvaderSquad* SquadTemplate;

	ASIGameModeBase();

protected:

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Level Layout")
	AInvaderSquad* SpawnedInvaderSquad;

	virtual void BeginPlay() override;


	UFUNCTION(BlueprintCallable)
	void CreateSquad();

	// Delegate bindings
	UFUNCTION(BlueprintCallable)
	void OnNewLevel();

	//
	UFUNCTION(BlueprintCallable)
	void IncreaseDifficulty();


	void EndGame();


	UFUNCTION(BlueprintCallable)
	void OnPlayerZeroLifes();
};
