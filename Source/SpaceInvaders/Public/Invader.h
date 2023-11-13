// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Invader.generated.h"

UCLASS()
class SPACEINVADERS_API AInvader : public AActor
{
	GENERATED_BODY()
	
public:	

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UInvaderMovementComponent* Movement;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float fireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float bulletVelocity = 3000.0f;

	//Audio
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundCue* AudioShoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundCue* AudioExplosion;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundCue* AudioJet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class ABullet> bulletClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class ABullet* bulletTemplate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector boundOrigin;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float  boundRadius;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* PFXExplosion;

	// Public Methods
	
	// Sets default values for this actor's properties
	AInvader();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UFUNCTION(BlueprintCallable)
	float GetBoundRadius();

	UFUNCTION(BlueprintCallable)
	FVector GetBoundOrigin();


	UFUNCTION(BlueprintCallable)
	void SetPositionInSquad(int32 index);

	UFUNCTION(BlueprintCallable)
	int32 GetPositionInSquad();

	UFUNCTION(BlueprintCallable)
	void SetInvaderMesh(class UStaticMesh* staticMesh = nullptr, const FString path = TEXT(""), FVector scale = FVector(1.0f, 1.0f, 1.0f));

	UFUNCTION(BlueprintCallable)
	void Fire();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

private:

	// Private Attributes
	UPROPERTY()
	class UAudioComponent* AudioComponent;

	UPROPERTY()
	class UAudioComponent* AudioComponentJet;

	UPROPERTY()
	class UInvaderMovementComponent* InvaderMovementComponent;

	UPROPERTY(VisibleInstanceOnly)
	int32 positionInSquad = 0;

	UPROPERTY(VisibleInstanceOnly)
	float timeFromLastShot = 0.0f;

	UPROPERTY(VisibleAnywhere)
	FName leftSideTag;

	UPROPERTY(VisibleAnywhere)
	FName rightSideTag;

	UPROPERTY(VisibleAnywhere)
	FName downSideTag;

	bool bFrozen;

	// Timer to control waiting after destruction
	FTimerHandle timerHandle;


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
