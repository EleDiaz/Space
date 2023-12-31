// Fill out your copyright notice in the Description page of Project Settings.


#include "Invader.h"
#include "Bullet.h"
#include "InvaderMovementComponent.h"
#include "SIGameModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

// TODO: Need different types of invaders

AInvader::AInvader()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Default values
	BulletClass = ABullet::StaticClass();

	// TODO: remove
	// Inmutable FNames for limits
	LeftSideTag = FName(AInvader::leftSideTagString);
	RightSideTag = FName(AInvader::rightSideTagString);
	DownSideTag = FName(AInvader::downSideTagString);

	// Create Components in actor
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMeshComponent");
	MovementComponent = CreateDefaultSubobject<UInvaderMovementComponent>("InvaderMoveComponent");
	RootComponent = Mesh; // We need a RootComponent to have a base transform

	SetInvaderMesh();

	// Component hierarchy
	AddOwnedComponent(MovementComponent);
	// Because UInvaderMovementComponent is only an Actor Component and not a Scene Component can't Attach To.

	// Audio component
	JetAudioComponent = CreateDefaultSubobject<UAudioComponent>("JetAudio");
	JetAudioComponent->SetupAttachment(RootComponent);
	ShotAudioComponent = CreateDefaultSubobject<UAudioComponent>("ShotAudio");
	ShotAudioComponent->SetupAttachment(RootComponent);

	FireRate = 0.0001f;
	bFrozen = false;
}

void AInvader::SetFrozen(bool frozen)
{
	this->bFrozen = frozen;
}

bool AInvader::GetFrozen() const
{
	return this->bFrozen;
}

// Called when the game starts or when spawned
void AInvader::BeginPlay()
{
	Super::BeginPlay();

	// Generate a Bullet Template of the correct class
	BulletTemplate = NewObject<ABullet>(this, BulletClass);
	BulletTemplate->bulletType = BulletType::INVADER;
}

void AInvader::SetPositionInSquad(int32 index)
{
	PositionInSquad = index;
}

int32 AInvader::GetPositionInSquad()
{
	return PositionInSquad;
}

// Called every frame
void AInvader::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bFrozen)
	{
		// Freezing the invader when is shot down
		return;
	}
	TimeFromLastShot += DeltaTime;

	// Fire, and as the time goes the change to start firing increase exponentially
	float val = FMath::RandRange(0.0f, 1.0f);
	if (val < 1.0 - FMath::Exp(-TimeFromLastShot * DeltaTime / FireRate))
	{
		Fire();
	}

	//Jet sound
	if (JetAudioComponent != nullptr && AudioJet != nullptr && MovementComponent != nullptr)
	{
		if (MovementComponent->bFreeJump && !JetAudioComponent->IsPlaying())
		{
			JetAudioComponent->SetSound(AudioJet);
			JetAudioComponent->Play();
		}
	}
}

void AInvader::Fire()
{
	if (bFrozen)
	{
		return;
	}
	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();
	if (BulletTemplate != nullptr)
	{
		BulletTemplate->velocity = BulletVelocity;
		BulletTemplate->dir = GetActorForwardVector();
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParameters.Template = BulletTemplate;
		const auto SpawnedBullet = GetWorld()->SpawnActor<ABullet>(BulletClass, SpawnLocation, SpawnRotation, SpawnParameters);
		// SpawnedBullet->Shot();
		ShotAudio();

		TimeFromLastShot = 0.0f;
	}
}

void AInvader::SetInvaderSquad(AInvaderSquad* Invader)
{
	this->InvaderSquad = Invader;
}

void AInvader::NotifyActorBeginOverlap(AActor* OtherActor)
{
	// If it is already a zombie invader nothing happens.
	if (bFrozen)
	{
		return;
	}

	UWorld* TheWorld = GetWorld();
	if (TheWorld != nullptr)
	{
		AGameModeBase* GameMode = UGameplayStatics::GetGameMode(TheWorld);
		ASIGameModeBase* MyGameMode = Cast<ASIGameModeBase>(GameMode);

		//First, bullet cases
		if (OtherActor->IsA(ABullet::StaticClass()))
		{
			ABullet* Bullet = Cast<ABullet>(OtherActor);
			if (Bullet->bulletType == BulletType::PLAYER)
			{
				OtherActor->Destroy();
				auto _ = InvaderSquad->InvaderDestroyed.ExecuteIfBound(PositionInSquad);
				InvaderDestroyed();
				return;
			}
			return;
			//It's an invader bullet, so it has to be ignored
		}

		// Overlap with other Invader is ignored
		if (OtherActor->IsA(AInvader::StaticClass()))
			return;

		// Reappear from top
		if (MovementComponent->bFreeJump)
		{
			InvaderSquad->ReenterFromTop(this);
			return;
		}

		// Belong to a squad
		if (InvaderSquad != nullptr && !MovementComponent->bFreeJump)
		{
			if (OtherActor->ActorHasTag(LeftSideTag))
				auto _ = InvaderSquad->SquadOnLeftSide.ExecuteIfBound();
			else if (OtherActor->ActorHasTag(RightSideTag))
				auto _ = InvaderSquad->SquadOnRightSide.ExecuteIfBound();
			else if (OtherActor->ActorHasTag(DownSideTag))
				auto _ = MyGameMode->SquadSuccessful.ExecuteIfBound(); // Squad wins!
		}
	}
}


void AInvader::InvaderDestroyed()
{
	if (UWorld* TheWorld = GetWorld())
	{
		bFrozen = true; // Invader can't move or fire while being destroyed

		// Hide Static Mesh Component
		if (Mesh != nullptr)
		{
			Mesh->SetVisibility(false);
		}
		if (IsValid(NFXExplosion))
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(NFXExplosion, RootComponent, NAME_None, FVector(0.f), FRotator(0.f), EAttachLocation::KeepRelativeOffset, true);
		}
		//Audio
		if (JetAudioComponent != nullptr && AudioExplosion != nullptr)
		{
			JetAudioComponent->SetSound(AudioExplosion);
			JetAudioComponent->Play();
		}
		// Wait:
		TheWorld->GetTimerManager().SetTimer(TimerHandle, this, &AInvader::PostInvaderDestroyed, 2.0f, false);
	}
}

void AInvader::PostInvaderDestroyed()
{
	Destroy();
}


void AInvader::SetInvaderMesh(UStaticMesh* newStaticMesh, const FString path, FVector scale)
{
	const TCHAR* tpath;
	tpath = AInvader::defaultStaticMeshName; // default route
	if (!Mesh) // No Mesh component
		return;

	if (!newStaticMesh)
	{
		if (!path.IsEmpty())
			tpath = *path;
		auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(tpath);
		newStaticMesh = MeshAsset.Object;
	}
	if (newStaticMesh)
	{
		Mesh->SetStaticMesh(newStaticMesh);
		Mesh->SetRelativeScale3D(scale);
		// FBoxSphereBounds meshBounds = Mesh->Bounds;
		// BoundOrigin = meshBounds.Origin;
		// BoundRadius = meshBounds.SphereRadius;
	}
}

void AInvader::ShotAudio() const
{
	if (ShotAudioComponent != nullptr && AudioShoot != nullptr)
	{
		ShotAudioComponent->SetSound(AudioShoot);
		ShotAudioComponent->Play();
	}
}
