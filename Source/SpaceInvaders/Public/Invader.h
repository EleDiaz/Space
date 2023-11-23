// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InvaderSquad.h"
#include "GameFramework/Actor.h"
#include "Invader.generated.h"

UCLASS()
class SPACEINVADERS_API AInvader : public AActor
{
	GENERATED_BODY()

public:
	//~ Begin Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UInvaderMovementComponent* MovementComponent;
	
	// Audio components are made to be use with a single track sound
	// In case of need simultaneous sounds playing just create those
	// components
	UPROPERTY()
	class UAudioComponent* AudioComponent;
	//~ End Components

	//~ Begin Invader attributes
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Invader Attributes")
	TSubclassOf<class ABullet> BulletClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Invader Attributes")
	class ABullet* BulletTemplate;
	
	/**
	 * A random value that get increase with time passed since las shot. Measure in seconds
	 * Example: A value of 20 implies a 0.9 of probability of shot again after 20 seconds.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Invader Attributes")
	float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Invader Attributes")
	float BulletVelocity = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Invader Attributes")
	int Lives = 1;
	//~ End Invader attributes

	//
	//~ Begin Audio
	// TODO: move the bullet instance, (this allow us to not necessarily change the audio clip but its audio effects)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio Invader")
	class USoundCue* AudioShoot;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio Invader")
	class USoundCue* AudioExplosion;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio Invader")
	class USoundCue* AudioJet;
	//~ End Audio

	//~ Begin Particles
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* PFXExplosion;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* PFXShot;
	//~ End Particles

	// Sets default values for this actor's properties
	AInvader();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UFUNCTION(BlueprintCallable)
	void SetPositionInSquad(int32 index);

	UFUNCTION(BlueprintCallable)
	int32 GetPositionInSquad();

	UFUNCTION(BlueprintCallable)
	void SetInvaderMesh(class UStaticMesh* staticMesh = nullptr, const FString path = TEXT(""),
	                    FVector scale = FVector(1.0f, 1.0f, 1.0f));

	UFUNCTION(BlueprintCallable)
	void Fire();
	void SetInvaderSquad(AInvaderSquad* InvaderSquad);

	UPROPERTY()
	class AInvaderSquad* InvaderSquad;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	
	bool bFrozen;
	
	UPROPERTY(VisibleInstanceOnly)
	int32 PositionInSquad = 0;

	UPROPERTY(VisibleInstanceOnly)
	float TimeFromLastShot = 0.0f;

	// TODO: Remove
	UPROPERTY(VisibleAnywhere)
	FName LeftSideTag;

	// TODO: Remove
	UPROPERTY(VisibleAnywhere)
	FName RightSideTag;

	// TODO: Remove
	UPROPERTY(VisibleAnywhere)
	FName DownSideTag;

	// Timer to control waiting after destruction
	// TODO: not used
	FTimerHandle TimerHandle;

	UFUNCTION()
	void InvaderDestroyed();

	UFUNCTION()
	void PostInvaderDestroyed();
	
	// Static literals of the class
	static constexpr const TCHAR* defaultStaticMeshName = TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'");
	static constexpr const TCHAR* leftSideTagString = TEXT("LeftLimit");
	static constexpr const TCHAR* rightSideTagString = TEXT("RightLimit");
	static constexpr const TCHAR* downSideTagString = TEXT("BottomLimit");
};
