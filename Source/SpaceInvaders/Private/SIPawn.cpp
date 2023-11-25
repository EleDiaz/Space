// Fill out your copyright notice in the Description page of Project Settings.


#include "SIPawn.h"
#include "SIGameModeBase.h"
#include "Bullet.h"
#include "Invader.h"

#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ASIPawn::ASIPawn()
	: velocity{1000},
	  bulletVelocity{3000},
	  playerLives{3}, //nullptr if(AudioShoot)
	  AudioShoot{},
	  AudioExplosion{},
	  bFrozen{false},
	  bPause{false},
	  MyGameMode{},
	  playerPoints{0}
{
	PrimaryActorTick.bCanEverTick = true;

	SetStaticMesh(); // Default mesh (SetStaticMesh with no arguments)

	// Audio component
	ExplosionAudioComponent = CreateDefaultSubobject<UAudioComponent>("ExplosionAudio");
	ExplosionAudioComponent->SetupAttachment(RootComponent);
	ShotAudioComponent = CreateDefaultSubobject<UAudioComponent>("ShotAudioComponent");
	ShotAudioComponent->SetupAttachment(RootComponent);
}

// Set a static mesh.
void ASIPawn::SetStaticMesh(UStaticMesh* staticMesh, FString path, FVector scale)
{
	UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
	const TCHAR* tpath;
	tpath = ASIPawn::defaultStaticMeshPath; // default route
	if (!Mesh) // No Mesh component
		return;

	if (!staticMesh)
	{
		if (!path.IsEmpty())
			tpath = *path;
		auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(tpath);
		staticMesh = MeshAsset.Object;
	}
	if (staticMesh)
	{
		Mesh->SetStaticMesh(staticMesh);

		Mesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	}
}


// Called when the game starts or when spawned
void ASIPawn::BeginPlay()
{
	Super::BeginPlay();

	// Generate a Bullet Template of the correct class
	if (bulletClass->IsChildOf<ABullet>())
		bulletTemplate = NewObject<ABullet>(this, bulletClass->GetFName(), RF_NoFlags, bulletClass.GetDefaultObject());
	else
		bulletTemplate = NewObject<ABullet>();

	bulletTemplate->bulletType = BulletType::PLAYER;

	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld());
	MyGameMode = Cast<ASIGameModeBase>(GameMode);
}

// Called every frame
void ASIPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASIPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis(TEXT("SIRight"), this, &ASIPawn::OnMove);
	PlayerInputComponent->BindAction(TEXT("SIFire"), IE_Pressed, this, &ASIPawn::OnFire);
	PlayerInputComponent->BindAction(TEXT("SIPause"), IE_Pressed, this, &ASIPawn::OnPause);
}


void ASIPawn::OnMove(float value)
{
	if (bFrozen)
		return;

	float deltaTime = GetWorld()->GetDeltaSeconds();

	float delta = velocity * value * deltaTime;
	FVector dir = FVector(0.0f, 1.0f, 0.0f);

	AddMovementInput(dir, delta);
}

void ASIPawn::ShotAudio()
{
	if (ShotAudioComponent != nullptr && AudioShoot != nullptr)
	{
		ShotAudioComponent->SetSound(AudioShoot);
		ShotAudioComponent->Play();
	}
}

void ASIPawn::OnFire()
{
	if (bFrozen)
		return;

	FVector SpawnLocation = GetActorLocation();
	FRotator SpawnRotation = GetActorRotation();
	bulletTemplate->velocity = bulletVelocity;
	bulletTemplate->dir = GetActorForwardVector();
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.Template = bulletTemplate;
	ABullet* Bullet = GetWorld()->SpawnActor<ABullet>(SpawnLocation, SpawnRotation, SpawnParameters);
	ShotAudio();
}

void ASIPawn::OnPause()
{
	bPause = !bPause;
}


int64 ASIPawn::GetPoints()
{
	return this->playerPoints;
}

int32 ASIPawn::GetLives()
{
	return this->playerLives;
}

void ASIPawn::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (!bFrozen)
	{
		// Collision with an enemy
		if (OtherActor->IsA(ABullet::StaticClass()))
		{
			// ABullet::StaticClass() obtengo un puntero a la UCLASS en memoria de Abullet
			ABullet* bullet = Cast<ABullet>(OtherActor);
			if (bullet->bulletType == BulletType::INVADER)
			{
				OtherActor->Destroy();
				DestroyPlayer();
			}
		}
		// Collision with an invader
		if (OtherActor->IsA(AInvader::StaticClass()))
		{
			OtherActor->Destroy();
			DestroyPlayer();
		}
	}
}

void ASIPawn::DestroyPlayer()
{
	UWorld* TheWorld;
	TheWorld = GetWorld();

	if (TheWorld)
	{
		bFrozen = true; // Pawn can'tmove or fire while being destroyed
		--this->playerLives;
		UStaticMeshComponent* LocalMeshComponent = Cast<UStaticMeshComponent>(
			GetComponentByClass(UStaticMeshComponent::StaticClass()));
		// Hide Static Mesh Component
		if (LocalMeshComponent != nullptr)
		{
			LocalMeshComponent->SetVisibility(false);
		}
		//Audio
		if (ExplosionAudioComponent != nullptr && AudioExplosion != nullptr)
		{
			ExplosionAudioComponent->SetSound(AudioExplosion);
			ExplosionAudioComponent->Play();
		}
		// Wait:
		TheWorld->GetTimerManager().SetTimer(timerHandle, this, &ASIPawn::PostPlayerDestroyed, 3.0f, false);
	}
}

void ASIPawn::PostPlayerDestroyed()
{
	// End game
	if (this->playerLives == 0)
	{
		if (MyGameMode)
			MyGameMode->PlayerZeroLives.ExecuteIfBound();
		return;
	}

	// Regenerate and continue
	UStaticMeshComponent* LocalMeshComponent = Cast<UStaticMeshComponent>(
		GetComponentByClass(UStaticMeshComponent::StaticClass()));
	// Show Static Mesh Component
	if (LocalMeshComponent != nullptr)
	{
		LocalMeshComponent->SetVisibility(true);
	}
	// Unfrozing
	bFrozen = false;
}