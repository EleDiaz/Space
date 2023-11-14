// Fill out your copyright notice in the Description page of Project Settings.


#include "InvaderSquad.h"
#include "InvaderMovementComponent.h"
#include "Invader.h"
#include "SIGameModeBase.h"

#include "Math/BoxSphereBounds.h"
#include "LocationVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"


AInvaderSquad::AInvaderSquad()
	: HorizontalVelocity{AInvaderSquad::defaultHorizontalVelocity}
	  , VerticalVelocity{AInvaderSquad::defaultVerticalVelocity}
	  , State{InvaderMovementType::STOP}
	  , PreviousState{InvaderMovementType::STOP}
	  , FreeJumpRate{0.0001}
	  , Rows{AInvaderSquad::defaultNRows}
	  , Cols{AInvaderSquad::defaultNCols}
	  , ExtraSeparation(AInvaderSquad::defaultExtraSeparation)
	  , numberOfMembers{Rows * Cols}
	  , timeFromLastFreeJump{0.0}
{
	// Create Components in actor

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root; // We need a RootComponent to have a base transform
	PrimaryActorTick.bCanEverTick = true;
}


void AInvaderSquad::SetRows(int32 nrows)
{
	this->Rows = nrows;
	this->numberOfMembers = this->Rows * this->Cols;
}

void AInvaderSquad::SetCols(int32 ncols)
{
	this->Cols = ncols;
	this->numberOfMembers = this->Rows * this->Cols;
}

int32 AInvaderSquad::GetRows()
{
	return this->Rows;
}

int32 AInvaderSquad::GetCols()
{
	return this->Cols;
}

int32 AInvaderSquad::GetNumberOfMembers()
{
	return this->numberOfMembers;
}


// Called when the game starts or when spawned
void AInvaderSquad::BeginPlay()
{
	Super::BeginPlay();

	UWorld* TheWorld = GetWorld();

	// Bind to delegates
	if (TheWorld != nullptr)
	{
		AGameModeBase* GameMode = UGameplayStatics::GetGameMode(TheWorld);
		MyGameMode = Cast<ASIGameModeBase>(GameMode);
		if (MyGameMode != nullptr)
		{
			MyGameMode->SquadOnRightSide.BindUObject(this, &AInvaderSquad::SquadOnRightSide);
			MyGameMode->SquadOnLeftSide.BindUObject(this, &AInvaderSquad::SquadOnLeftSide);
			MyGameMode->SquadFinishesDown.BindUObject(this, &AInvaderSquad::SquadFinishesDown);
			MyGameMode->InvaderDestroyed.AddUObject(this, &AInvaderSquad::RemoveInvader);
		}
	}

	// Set Invader Template with Default Value for invaderClass
	if (InvaderClass->IsChildOf<AInvader>())
	{
		InvaderTemplate = NewObject<AInvader>(this, InvaderClass->GetFName(), RF_NoFlags,
		                                      InvaderClass.GetDefaultObject());
	}
	else
	{
		InvaderTemplate = NewObject<AInvader>();
	}

	//Spawn Invaders
	FBoxSphereBounds spawnableBounds = LocationVolume->GetBounds();

	// bounds.Origin;
	// bounds.BoxExtent;

	FBoxSphereBounds invaderBounds = InvaderTemplate->Mesh->Bounds;

	auto x = FGenericPlatformMath::FloorToInt(
		spawnableBounds.BoxExtent.X / (invaderBounds.BoxExtent.X + ExtraSeparation));

	

	FVector actorLocation = GetActorLocation(); // TODO: Remove
	FVector spawnLocation = actorLocation; // TODO: Remove
	FRotator spawnRotation = FRotator(0.0f, 180.0f, 0.0f);
	// Invader Forward is opposite to Player Forward (Yaw rotation)
	FActorSpawnParameters spawnParameters;
	int32 count = 0;
	AInvader* spawnedInvader;
	float radiusX = 0.0f;
	float radiusY = 0.0f;
	for (int i = 0; i < this->Cols; i++)
	{
		for (int j = 0; j < this->Rows; j++)
		{
			//invaderTemplate->SetPositionInSquad(count);

			spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			spawnParameters.Template = InvaderTemplate;
			spawnedInvader = GetWorld()->SpawnActor<AInvader>(spawnLocation, spawnRotation, spawnParameters);
			spawnedInvader->SetPositionInSquad(count);
			++count;
			SquadMembers.Add(spawnedInvader);
			float r = spawnedInvader->GetBoundRadius();
			if (r > radiusX)
				radiusX = r;
			if (r > radiusY)
				radiusY = r;
			spawnLocation.X += radiusX * 2 + this->ExtraSeparation;
		}
		spawnLocation.X = actorLocation.X;

		spawnLocation.Y += radiusY * 2 + this->ExtraSeparation;
	}

	this->numberOfMembers = count;

	this->State = InvaderMovementType::RIGHT;
}

void AInvaderSquad::Destroyed()
{
	Super::Destroyed();
	for (AInvader* invader : SquadMembers)
	{
		if (invader != nullptr)
			invader->Destroy();
	}
}

// Called every frame
void AInvaderSquad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateSquadState(DeltaTime);
}

void AInvaderSquad::UpdateSquadState(float delta)
{
	TArray<AInvader*> survivors;

	for (auto invader : SquadMembers)
	{
		//------------------------------------
		if (invader)
		{
			// very important, first nullptr is checked!.


			// First, we get de movement component
			UInvaderMovementComponent* imc = (UInvaderMovementComponent*)invader->GetComponentByClass(
				UInvaderMovementComponent::StaticClass());

			// Now, its state is updated
			if (imc)
			{
				if (imc->state != InvaderMovementType::FREEJUMP)
					survivors.Emplace(invader);
				imc->horizontalVelocity = HorizontalVelocity;
				imc->verticalVelocity = VerticalVelocity;
				//imc->isXHorizontal = isXHorizontal;
				if (imc->state != InvaderMovementType::FREEJUMP)
					// The state of the squad is copied to the invader state (except for those in FREEJUMP)
					imc->state = State;
			}
		}

		//------------------------------------
	}
	this->timeFromLastFreeJump += delta;
	float val = FMath::RandRange(0.0f, 1.0f);
	int32 countSurvivors = survivors.Num();
	if (countSurvivors > 0 && val < (1.0 - FMath::Exp(-FreeJumpRate * this->timeFromLastFreeJump)))
	{
		int32 ind = FMath::RandRange(0, countSurvivors - 1); // Randomly select one of the living invaders
		UInvaderMovementComponent* imc = (UInvaderMovementComponent*)survivors[ind]->GetComponentByClass(
			UInvaderMovementComponent::StaticClass());
		if (imc)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, FString::Printf(TEXT("%s on FreeJump"), *(imc->GetName())));
			survivors[ind]->fireRate *= 100;
			imc->state = InvaderMovementType::FREEJUMP;
		}
	}
}


// Handling events

// La escuadra llega al lado derecho

void AInvaderSquad::SquadOnRightSide()
{
	PreviousState = InvaderMovementType::RIGHT;
	State = InvaderMovementType::DOWN;
}

// La escuadra llega al lado izquierdo

void AInvaderSquad::SquadOnLeftSide()
{
	PreviousState = InvaderMovementType::LEFT;
	State = InvaderMovementType::DOWN;
}

// Cada vez que un invasor completa el movimiento de descenso

void AInvaderSquad::SquadFinishesDown()
{
	static int32 countActions = 0;
	++countActions;
	if (countActions >= numberOfMembers)
	{
		countActions = 0;
		switch (PreviousState)
		{
		case InvaderMovementType::RIGHT:
			State = InvaderMovementType::LEFT;
			break;
		case InvaderMovementType::LEFT:
			State = InvaderMovementType::RIGHT;
			break;
		default:
			State = InvaderMovementType::STOP;
		}
	}
}


void AInvaderSquad::RemoveInvader(int32 ind)
{
	SquadMembers[ind] = nullptr;
	--this->numberOfMembers;
	if (this->numberOfMembers == 0)
	{
		if (MyGameMode != nullptr)
		{
			MyGameMode->SquadDestroyed.Broadcast();
		}
	}
}
