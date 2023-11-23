// Fill out your copyright notice in the Description page of Project Settings.


#include "Invader.h"
#include "Bullet.h"
#include "InvaderMovementComponent.h"
#include "SIGameModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"

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
	AudioComponent = CreateDefaultSubobject<UAudioComponent>("Audio");
	RootComponent = Mesh; // We need a RootComponent to have a base transform

	SetInvaderMesh();

	// Component hierarchy
	AddOwnedComponent(MovementComponent);
	// Because UInvaderMovementComponent is only an Actor Component and not a Scene Component can't Attach To.

	// Audio component
	AudioComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

	FireRate = 0.0001f;
	bFrozen = false;
}

// Called when the game starts or when spawned
void AInvader::BeginPlay()
{
	Super::BeginPlay();

	// Generate a Bullet Template of the correct class
	if (BulletClass->IsChildOf<ABullet>())
		BulletTemplate = NewObject<ABullet>(this, BulletClass->GetFName(), RF_NoFlags, BulletClass.GetDefaultObject());
	else
		BulletTemplate = NewObject<ABullet>();

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
		MovementComponent->state = InvaderMovementType::STOP;
		return;
	}
	TimeFromLastShot += DeltaTime;

	// Fire, and as the time goes the change to start firing increase exponentially
	float val = FMath::RandRange(0.0f, 1.0f);
	if (val < 1.0 - FMath::Exp(-(2.72 * TimeFromLastShot) / FireRate))
	{
		Fire();
	}

	//Jet sound
	if (AudioComponent != nullptr && AudioJet != nullptr && MovementComponent != nullptr)
	{
		bool bFreeJump = MovementComponent->state == InvaderMovementType::FREEJUMP;
		if (bFreeJump && !AudioComponent->IsPlaying())
		{
			AudioComponent->SetSound(AudioJet);
			AudioComponent->Play();
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
		const auto SpawnedBullet = GetWorld()->SpawnActor<ABullet>(SpawnLocation, SpawnRotation, SpawnParameters);
		SpawnedBullet->Shot();

		TimeFromLastShot = 0.0f;
	}
}

void AInvader::SetInvaderSquad(AInvaderSquad* Invader)
{
	this->InvaderSquad = Invader;
}

void AInvader::NotifyActorBeginOverlap(AActor* OtherActor)
{
	// Debug
	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, FString::Printf(TEXT("%s entered me"), *(OtherActor->GetName())));
	FName actorTag;
	// If it is already a zombie invader nothing happens.
	if (bFrozen)
	{
		return;
	}

	UWorld* TheWorld = GetWorld();
	if (TheWorld != nullptr)
	{
		bool bFreeJump = MovementComponent->state == InvaderMovementType::FREEJUMP;
		AGameModeBase* GameMode = UGameplayStatics::GetGameMode(TheWorld);
		ASIGameModeBase* MyGameMode = Cast<ASIGameModeBase>(GameMode);

		//First, bullet cases
		if (OtherActor->IsA(ABullet::StaticClass()))
		{
			ABullet* Bullet = Cast<ABullet>(OtherActor);
			if (Bullet->bulletType == BulletType::PLAYER)
			{
				OtherActor->Destroy();
				MyGameMode->InvaderDestroyed.Broadcast(this->PositionInSquad);
				InvaderDestroyed();
				return;
			}
			return;
			//It's an invader bullet, so it has to be ignored
		}

		// OVerlap with other Invader is ignored
		if (OtherActor->IsA(AInvader::StaticClass()))
			return;

		// Overlap with anything in freejump (except invaders and their own bullets) is a silent Destroy.
		// TODO: Reappear on the top do not destroy
		if (bFreeJump)
		{
			
			MyGameMode->InvaderDestroyed.Broadcast(this->PositionInSquad);
			Destroy();
			return;
		}

		// Belong to a squad
		if (InvaderSquad != nullptr)
		{
			if (OtherActor->ActorHasTag(LeftSideTag) && !bFreeJump)
				auto _ = InvaderSquad->SquadOnLeftSide.ExecuteIfBound();
			else if (OtherActor->ActorHasTag(RightSideTag) && !bFreeJump)
				auto _ = InvaderSquad->SquadOnRightSide.ExecuteIfBound();
			else if (OtherActor->ActorHasTag(DownSideTag) && !bFreeJump)
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
		if (PFXExplosion != nullptr)
		{
			UGameplayStatics::SpawnEmitterAtLocation(TheWorld, PFXExplosion, this->GetActorTransform(), true);
		}
		//Audio
		if (AudioComponent != nullptr && AudioExplosion != nullptr)
		{
			AudioComponent->SetSound(AudioExplosion);
			AudioComponent->Play();
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
