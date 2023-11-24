// Fill out your copyright notice in the Description page of Project Settings.

#include "SIGameModeBase.h"
#include "SIPawn.h"
#include "SIPlayerController.h"
#include "InvaderSquad.h"
#include "Kismet/GameplayStatics.h"

ASIGameModeBase::ASIGameModeBase()
	: SpawnedInvaderSquad{}

{
	// GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("Nueva escuadra")));
	DefaultPawnClass = ASIPawn::StaticClass();
	PlayerControllerClass = ASIPlayerController::StaticClass();
}

void ASIGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// Delegate bindings:
	SquadDestroyed.AddUObject(this, &ASIGameModeBase::OnNewLevel);
	PlayerZeroLives.BindUObject(this, &ASIGameModeBase::OnPlayerZeroLifes);
}

void ASIGameModeBase::OnNewLevel()
{
	IncreaseDifficulty();
}

void ASIGameModeBase::IncreaseDifficulty() {
	// TODO: modify the template Squad
}

void ASIGameModeBase::EndGame()
{
	if (this->SpawnedInvaderSquad != nullptr)
	{
		this->SpawnedInvaderSquad->Destroy();
	}
	// Close level and open menu level
	UGameplayStatics::OpenLevel(this, FName("Menu"));
}

void ASIGameModeBase::OnPlayerZeroLifes()
{
	EndGame();
}
