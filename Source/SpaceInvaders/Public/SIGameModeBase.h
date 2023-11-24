// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

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
	FStandardDelegateSignature SquadSuccessful; // Invader -> GameMode, Player
	FMulticastDelegateSignature SquadDestroyed;
	// Player Dies
	FStandardDelegateSignature PlayerZeroLives; // Player -> Game Mode

	ASIGameModeBase();

protected:
	// Level InvaderSquad
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Level Layout")
	class AInvaderSquad* SpawnedInvaderSquad;

	virtual void BeginPlay() override;

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
