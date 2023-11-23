// Fill out your copyright notice in the Description page of Project Settings.


#include "InvaderSquad.h"
#include "InvaderMovementComponent.h"
#include "Invader.h"
#include "SIGameModeBase.h"

#include "Math/BoxSphereBounds.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BillboardComponent.h"

AInvaderSquad::AInvaderSquad()
	: HorizontalVelocity{AInvaderSquad::defaultHorizontalVelocity}
	  , VerticalVelocity{AInvaderSquad::defaultVerticalVelocity}
	  , State{InvaderMovementType::STOP}
	  , PreviousState{InvaderMovementType::STOP}
	  , FreeJumpRate{0.0001}
	  , ExtraSeparation(AInvaderSquad::defaultExtraSeparation)
	  , numberOfMembers{0}
	  , timeFromLastFreeJump{0.0}
{
	// Create Components in actor
	PrimaryActorTick.bCanEverTick = true;

	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>("Billboard");
	AActor::SetActorHiddenInGame(true);
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
			SquadOnRightSide.BindUObject(this, &AInvaderSquad::NextActionSquadOnRightSide);
			SquadOnLeftSide.BindUObject(this, &AInvaderSquad::NextActionSquadSquadOnLeftSide);
			SquadFinishesDown.BindUObject(this, &AInvaderSquad::NextActionSquadFinishesDown);
			SquadDestroyed.BindUObject(this, &AInvaderSquad::RemoveInvader);
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
	InvaderTemplate->SetInvaderSquad(this);

	//Spawn Invaders
	FBoxSphereBounds SpawnableBounds = LocationVolume->GetBounds();
	FBoxSphereBounds InvaderBounds = InvaderTemplate->Mesh->Bounds;
	auto Spacing_X = InvaderBounds.BoxExtent.X + ExtraSeparation;
	auto Spacing_Y = InvaderBounds.BoxExtent.Y + ExtraSeparation;
	auto min = SpawnableBounds.Origin;
	auto max = min + SpawnableBounds.BoxExtent;

	auto centerX = Spacing_X / 2 * (FGenericPlatformMath::Fmod(FMath::Abs(max.X - min.X), Spacing_X) /
		ExtraSeparation);
	auto centerY = Spacing_Y / 2 * (FGenericPlatformMath::Fmod(FMath::Abs(max.Y - min.Y), Spacing_Y) /
		Spacing_Y);

	int32 InvadersCount = 0;
	FVector SpawnLocation;
	// Invader Forward is opposite to Player Forward (Yaw rotation)
	FRotator SpawnRotation = FRotator(0.0f, 180.0f, 0.0f);
	FActorSpawnParameters SpawnParameters;
	AInvader* SpawnedInvader;
	
	for (float i = min.X; i < max.X; i += Spacing_X)
	{
		for (float j = min.Y; j < max.Y; j += Spacing_Y)
		{
			SpawnLocation = FVector(centerX + i, centerY + j, SpawnableBounds.Origin.Z);
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParameters.Template = InvaderTemplate;
			SpawnedInvader = GetWorld()->SpawnActor<AInvader>(SpawnLocation, SpawnRotation, SpawnParameters);
			SpawnedInvader->SetPositionInSquad(InvadersCount);
			++InvadersCount;
			SquadMembers.Add(SpawnedInvader);
		}
	}

	/*
	auto x = FGenericPlatformMath::FloorToInt(
		spawnableBounds.BoxExtent.X / (invaderBounds.BoxExtent.X + ExtraSeparation));

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
	*/
	this->numberOfMembers = InvadersCount;

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
			survivors[ind]->FireRate *= 100;
			imc->state = InvaderMovementType::FREEJUMP;
		}
	}
}


// Handling events

// La escuadra llega al lado derecho
void AInvaderSquad::NextActionSquadOnRightSide()
{
	PreviousState = InvaderMovementType::RIGHT;
	State = InvaderMovementType::DOWN;
}

// La escuadra llega al lado izquierdo
void AInvaderSquad::NextActionSquadSquadOnLeftSide()
{
	PreviousState = InvaderMovementType::LEFT;
	State = InvaderMovementType::DOWN;
}

// Cada vez que un invasor completa el movimiento de descenso
void AInvaderSquad::NextActionSquadFinishesDown()
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
