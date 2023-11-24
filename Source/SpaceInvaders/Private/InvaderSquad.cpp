// Fill out your copyright notice in the Description page of Project Settings.


#include "InvaderSquad.h"
#include "InvaderMovementComponent.h"
#include "Invader.h"
#include "SIGameModeBase.h"

#include "Math/BoxSphereBounds.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BillboardComponent.h"

AInvaderSquad::AInvaderSquad()
	: HorizontalVelocity{1000}
	  , VerticalVelocity{1000}
      , DescendingAmount{100}
	  , State{InvaderMovementType::STOP}
	  , PreviousState{InvaderMovementType::STOP}
	  , FreeJumpRate{10}
	  , ExtraSeparation{5.0}
	  , NumberOfMembers{0}
	  , TimeFromLastFreeJump{0.0}
{
	// Create Components in actor
	PrimaryActorTick.bCanEverTick = true;

	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>("Billboard");
	AActor::SetActorHiddenInGame(true);
}

int32 AInvaderSquad::GetNumberOfMembers()
{
	return this->NumberOfMembers;
}

void AInvaderSquad::ReenterFromTop(AInvader* Invader)
{
	FVector InvaderLocation = Invader->GetActorLocation();
	// We could use a new random location inside that reenter volume
	float XAxis = ReenterVolume->GetActorLocation().X;
	Invader->SetActorLocation(FVector(XAxis, InvaderLocation.Y, InvaderLocation.Z));
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
			InvaderDestroyed.BindUObject(this, &AInvaderSquad::RemoveInvader);
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
	BuildSquad();
}

void AInvaderSquad::BuildSquad()
{
	// Clean a possible squad
	if (!SquadMembers.IsEmpty())
	{
		for (auto SquadMember : SquadMembers)
		{
			if (SquadMember != nullptr)
				SquadMember->Destroy();
		}
	}
	
	// Spawn Invaders
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
			// We could attach the invaders to the squad to move only one transform
			// SpawnedInvader->AttachToActor(this, ... , "");
			++InvadersCount;
			SquadMembers.Add(SpawnedInvader);
		}
	}
	NumberOfMembers = InvadersCount;
	State = InvaderMovementType::RIGHT;
}

void AInvaderSquad::Destroyed()
{
	Super::Destroyed();
	for (AInvader* Invader : SquadMembers)
	{
		if (Invader != nullptr)
			Invader->Destroy();
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
	TArray<AInvader*> Survivors;

	// Get all available invaders
	for (auto Invader : SquadMembers)
	{
		if (Invader)
		{
			if (const auto InvaderMovement = Invader->GetComponentByClass<UInvaderMovementComponent>())
			{
				if (!InvaderMovement->bFreeJump)
				{
					Survivors.Emplace(Invader);
				}
			}
		}
	}
	
	TimeFromLastFreeJump += delta;
	float Val = FMath::RandRange(0.0f, 1.0f);
	int32 CountSurvivors = Survivors.Num();
	if (CountSurvivors > 0 && Val < (1.0 - FMath::Exp(-FreeJumpRate * TimeFromLastFreeJump)))
	{
		int32 Ind = FMath::RandRange(0, CountSurvivors - 1); // Randomly select one of the living invaders
		
		if (const auto InvaderMovement = Survivors[Ind]->GetComponentByClass<UInvaderMovementComponent>())
		{
			// Survivors[ind]->FireRate *= 100;
			InvaderMovement->bFreeJump = true;
		}
	}
}

void AInvaderSquad::NextActionSquadOnRightSide()
{
	PreviousState = InvaderMovementType::RIGHT;
	State = InvaderMovementType::DOWN;
}

void AInvaderSquad::NextActionSquadSquadOnLeftSide()
{
	PreviousState = InvaderMovementType::LEFT;
	State = InvaderMovementType::DOWN;
}

void AInvaderSquad::NextActionSquadFinishesDown()
{
	static int32 CountActions = 0;
	++CountActions;
	if (CountActions >= NumberOfMembers)
	{
		CountActions = 0;
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
	--this->NumberOfMembers;
	if (this->NumberOfMembers == 0)
	{
		if (MyGameMode != nullptr)
		{
			MyGameMode->SquadDestroyed.Broadcast();
		}
	}
}
