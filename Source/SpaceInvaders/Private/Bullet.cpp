// Fill out your copyright notice in the Description page of Project Settings.
#include "Bullet.h"

#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Sound/SoundCue.h"
#include "UObject/ConstructorHelpers.h"


// Sets default values
ABullet::ABullet()
	: bulletType{BulletType::PLAYER},
	  dir{},
	  velocity{}
{
	PrimaryActorTick.bCanEverTick = true;
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMeshComponent");

	RootComponent = Mesh; // We need a RootComponent to have a base transform
	SetBulletMesh();

	// Bullets will overlap, blocking behaviour is not desirable
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
}


void ABullet::BeginPlay()
{
	Super::BeginPlay();
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector location = GetActorLocation();
	location += DeltaTime * velocity * dir;
	SetActorLocation(location);
}

void ABullet::SetBulletMesh(UStaticMesh* staticMesh, FString path, FVector scale)
{
	const TCHAR* tpath;
	tpath = ABullet::defaultStaticMeshPath; // default route
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
		Mesh->SetRelativeScale3D(scale);
	}
}

void ABullet::Shot()
{
	if (AudioComponent != nullptr && AudioShoot != nullptr)
	{
		AudioComponent->SetSound(AudioShoot);
		AudioComponent->Play();
	}
}


void ABullet::NotifyActorBeginOverlap(AActor* OtherActor)
{
	// Debug
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red,
	                                 FString::Printf(TEXT("%s overlaped me"), *(OtherActor->GetName())));
	for (FName tag : autoDestroyTags)
		if (OtherActor->ActorHasTag(tag))
			Destroy();
}
