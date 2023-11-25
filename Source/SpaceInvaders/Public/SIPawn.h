// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "SIPawn.generated.h"

UCLASS()
class SPACEINVADERS_API ASIPawn : public ADefaultPawn
{
	GENERATED_BODY()

public:
	// Velocity of the pawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Defender config")
	float velocity;

	// Bullets
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Defender config")
	TSubclassOf<class ABullet> bulletClass;

	// Velocity of the player bullets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defender config")
	float bulletVelocity;

	//Lifes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defender config")
	int32 playerLives;

	//Audios
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defender config")
	class USoundCue* AudioShoot;

	//Audios: Explossion
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defender config")
	class USoundCue* AudioExplosion;


	// Sets default values for this pawn's properties
	ASIPawn();

	// It could be possible to change the static mesh component during run time.
	UFUNCTION(BlueprintCallable)
	void SetStaticMesh(class UStaticMesh* staticMesh = nullptr, FString path = TEXT(""),
	                   FVector scale = FVector(1.0f, 1.0f, 1.0f));

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Overlaps
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	//Getters and Setters

	UFUNCTION(BlueprintCallable)
	int64 GetPoints();

	UFUNCTION(BlueprintCallable)
	int32 GetLives();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Bindings for inputs
	void OnMove(float value);

	void ShotAudio();
	void OnFire();

	void OnPause();

	UFUNCTION(BlueprintCallable)
	void DestroyPlayer();

	UFUNCTION(BlueprintCallable)
	void PostPlayerDestroyed();

private:
	// To set a frozen state (no moving and firing capabilities)
	bool bFrozen;

	//To pause the Game
	bool bPause;

	FTimerHandle timerHandle;

	UPROPERTY()
	class ASIGameModeBase* MyGameMode;

	//Points
	int64 playerPoints;


	UPROPERTY() //This is necessary to control the reference counter of the ABullet and avoid garbage collector action
	class ABullet* bulletTemplate;


	// Audio component
	UPROPERTY()
	class UAudioComponent* ExplosionAudioComponent;
	UPROPERTY()
	class UAudioComponent* ShotAudioComponent;

	static constexpr const TCHAR* defaultStaticMeshPath = TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'");
};
