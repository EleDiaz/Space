// Fill out your copyright notice in the Description page of Project Settings.

#include "SIGameModeBase.h"
#include "SIPawn.h"
#include "SIPlayerController.h"
#include "InvaderSquad.h"
#include "SIGameInstance.h"
#include "Kismet/GameplayStatics.h"

ASIGameModeBase::ASIGameModeBase()
	: PointsPerInvader(100), PointsPerSquad(1000), PlayerPoints(0), SpawnedInvaderSquad{}

{
	// GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("Nueva escuadra")));
	DefaultPawnClass = ASIPawn::StaticClass();
	PlayerControllerClass = ASIPlayerController::StaticClass();
}

void ASIGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SpawnedInvaderSquad = Cast<AInvaderSquad>(UGameplayStatics::GetActorOfClass(GetWorld(), AInvaderSquad::StaticClass()));

	// Delegate bindings:
	InvaderDestroyed.BindUObject(this, &ASIGameModeBase::IncreaseScore);
	SquadDestroyed.AddUObject(this, &ASIGameModeBase::OnNewLevel);
	PlayerZeroLives.BindUObject(this, &ASIGameModeBase::OnPlayerZeroLifes);
}

void ASIGameModeBase::OnNewLevel()
{
	PlayerPoints += PointsPerSquad;
	if (IsValid(SpawnedInvaderSquad))
	{
		SpawnedInvaderSquad->IncreaseLevel();
		SpawnedInvaderSquad->BuildSquad();
	}
}

void ASIGameModeBase::IncreaseScore()
{
	PlayerPoints += PointsPerInvader;
}

void ASIGameModeBase::EndGame()
{
	auto GameInstance = Cast<USIGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (IsValid(GameInstance)) {
		GameInstance->Score->AddItem(FPlayerScore(PlayerPoints, TEXT("New Score")));
	}
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
