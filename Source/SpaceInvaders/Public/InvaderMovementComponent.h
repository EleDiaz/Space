// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "InvaderMovementComponent.generated.h"

UENUM(BlueprintType)
enum class InvaderMovementType : uint8
{
	STOP = 0 UMETA(DisplayName = "Stopped"),
	RIGHT = 1 UMETA(DisplayName = "Right"),
	LEFT = 2 UMETA(DisplayName = "Left"),
	DOWN = 3 UMETA(DisplayName = "Down"),
};

// TODO: This class does more than what it should do. The squadron movement should be associate to the Invader Squad
//       And delegate to this, once we set it to freeJump
UCLASS()
class SPACEINVADERS_API UInvaderMovementComponent : public UMovementComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInvaderMovementComponent();

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Invader Movement")
	bool bFreeJump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Invader Movement")
	float freeJumpRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Invader Movement")
	float freeJumpVelocity; // Velocity in the first stage of the free jump

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void MoveInvader(float DeltaX, float DeltaY);

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	void GetCurve(float delta);

private:
	// Down movement state variables:
	float descendingProgress = 0.0f; // Store progress in the Down state

	UPROPERTY()
	class AInvaderSquad* InvaderSquad; // to call delegates
	UPROPERTY()
	class AInvader* Invader; // to call delegates
};
