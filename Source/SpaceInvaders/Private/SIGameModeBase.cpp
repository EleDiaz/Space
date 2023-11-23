// Fill out your copyright notice in the Description page of Project Settings.

#include "SIGameModeBase.h"
#include "SIPawn.h"
#include "SIPlayerController.h"
#include "InvaderSquad.h"
#include "Kismet/GameplayStatics.h"

ASIGameModeBase::ASIGameModeBase()
	: SpawnableLocation{}, TopReenterSpawn{}, SquadTemplate{}, SpawnedInvaderSquad{}

{
	// GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("Nueva escuadra")));
	DefaultPawnClass = ASIPawn::StaticClass();
	PlayerControllerClass = ASIPlayerController::StaticClass();
	InvaderSquadClass = AInvaderSquad::StaticClass();
}

void ASIGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	// Set a default Squad Template (base difficulty ideally)
	// TODO: Find template in scene
	if (InvaderSquadClass->IsChildOf<AInvaderSquad>()) {
		SquadTemplate = NewObject<AInvaderSquad>(this, InvaderSquadClass->GetFName(), RF_NoFlags, InvaderSquadClass.GetDefaultObject());
	}
	else {
		SquadTemplate = NewObject<AInvaderSquad>();
	}

	// TODO: Use the spawnableLocation to set the template

	// Spawn a squad of invaders
	CreateSquad();

	// Delegate bindings:
	SquadDestroyed.AddUObject(this, &ASIGameModeBase::OnNewLevel);
	PlayerZeroLifes.BindUObject(this, &ASIGameModeBase::OnPlayerZeroLifes);
}

void ASIGameModeBase::CreateSquad()
{
	if (SpawnedInvaderSquad != nullptr)
	{
		SpawnedInvaderSquad->Destroy();
	}

	if (InvaderSquadClass)
	{
        FActorSpawnParameters spawnParameters;
		spawnParameters.Template = SquadTemplate;

		SpawnedInvaderSquad = GetWorld()->SpawnActor<AInvaderSquad>(spawnParameters);
	}
}

void ASIGameModeBase::OnNewLevel()
{
	IncreaseDifficulty();
	CreateSquad();
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
