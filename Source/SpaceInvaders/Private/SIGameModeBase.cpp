// Fill out your copyright notice in the Description page of Project Settings.


#include "SIGameModeBase.h"
#include "SIPawn.h"
#include "SIPlayerController.h"
#include "InvaderSquad.h"
#include "Kismet/GameplayStatics.h"


ASIGameModeBase::ASIGameModeBase()
	: spawnLocation{ }
	, nInvaderRows{ 5 }
	, nInvaderCols{ 7 }
	, spawnedInvaderSquad{}

{

	DefaultPawnClass = ASIPawn::StaticClass();
	PlayerControllerClass = ASIPlayerController::StaticClass();
	InvaderSquadClass = AInvaderSquad::StaticClass();

}

void ASIGameModeBase::BeginPlay() {

	Super::BeginPlay();

	//Spawn a squad of invaders

	RegenerateSquad();

	// Delegate bindings:
	this->NewSquad.AddUObject(this, &ASIGameModeBase::OnNewSquad);
	this->PlayerZeroLifes.BindUObject(this, &ASIGameModeBase::OnPlayerZeroLifes);

}

void ASIGameModeBase::RegenerateSquad() {

	if (this->spawnedInvaderSquad != nullptr)
		this->spawnedInvaderSquad->Destroy();
	if (InvaderSquadClass) {
		
		AInvaderSquad* squad = Cast<AInvaderSquad>(InvaderSquadClass->GetDefaultObject());
		if (squad) {
			this->spawnedInvaderSquad = Cast<AInvaderSquad>(GetWorld()->SpawnActor(InvaderSquadClass, &spawnLocation));
		}
	}
}

void ASIGameModeBase::OnNewSquad(int32 lifes) {
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("Nueva escuadra")));
	RegenerateSquad();
}


void ASIGameModeBase::EndGame() {
	if (this->spawnedInvaderSquad != nullptr)
		this->spawnedInvaderSquad->Destroy();
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("Nuevo juego")));
	// Close level and open menu level
	UGameplayStatics::OpenLevel(this, FName("Menu"));

}



void ASIGameModeBase::OnPlayerZeroLifes() {
	EndGame();
}



