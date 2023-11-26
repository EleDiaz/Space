// Fill out your copyright notice in the Description page of Project Settings.
#include "InvaderMovementComponent.h"

#include "Invader.h"
#include "Kismet/KismetMathLibrary.h"

UInvaderMovementComponent::UInvaderMovementComponent():
	InvaderSquad(nullptr), Invader(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;
	// Defaults:
	freeJumpRadius = 300.0f;
	freeJumpVelocity = 1000.0f;
}

// Called when the game starts
// Set pointer to GameMode to call delegates
void UInvaderMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	Invader = Cast<AInvader>(GetOwner());
	if (Invader)
	{
		InvaderSquad = Invader->InvaderSquad;
	}
}

void UInvaderMovementComponent::MoveInvader(float DeltaX, float DeltaY)
{
	auto InvaderLocation = Invader->GetActorLocation();
	InvaderLocation.X += DeltaX;
	InvaderLocation.Y += DeltaY;
	Invader->SetActorLocation(InvaderLocation);
}


// Called every frame
void UInvaderMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (Invader->GetFrozen())
	{
		return;
	}

	if (bFreeJump)
	{
		//TODO: Do somthing
		return;
	}

	float DeltaHorizontal = InvaderSquad->HorizontalVelocity * DeltaTime;
	float DeltaVertical = InvaderSquad->VerticalVelocity * DeltaTime;

	switch (InvaderSquad->State)
	{
	case InvaderMovementType::STOP:
		// MoveInvader(0,0);
		break;

	case InvaderMovementType::RIGHT:
		MoveInvader(0, DeltaHorizontal);
		break;

	case InvaderMovementType::LEFT:
		MoveInvader(0, -DeltaHorizontal);
		break;

	// Down movement: this is an automatic movement that has to finish automatically
	// It is based on an internal variable, descendingProgress, that is updated.
	case InvaderMovementType::DOWN:
		// TODO: This looks quite bad, in terms of performance if we had 1 million ship, all of them will send this
		//       All ships share same speed, and probably the engine could give the same delta time to each actor.
		if (descendingProgress > InvaderSquad->DescendingAmount)
		{
			DeltaVertical = 0.0f;

			// TODO: Review this
			if (auto World = GetWorld())
			{
				World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
				{
					auto _ = InvaderSquad->SquadFinishesDown.ExecuteIfBound();
					descendingProgress = 0.0f;
				}));
			}

		}
		MoveInvader(-DeltaVertical, 0);
		descendingProgress += DeltaVertical;
		break;
	}
}

void UInvaderMovementComponent::GetCurve(float delta)
{
	// auto Location = Invader->GetActorLocation();
	// auto rotation = FRotator::MakeFromEuler(UE::Math::TVector<double>(0.0f, 10.0f, 0));
	// rotation.RotateVector()
	// FRotator rotation = FRotator(0.0f, -(theta * 180.0f / PI) - 90, 0.0f);
	//
	//
	
// 				FQuat newQuaternion = rotation.Quaternion() * initialQuaternion;
	
	
}

// TODO: The idea would be to generate points through a SplineComponent
// void UInvaderMovementComponent::GenerateTargetPoints()
// {
// 	AActor* Parent = GetOwner();
// 	FTransform initialTransform;
// 	FVector initialLocation;
//
// 	FQuat initialQuaternion;
// 	if (!Parent)
// 	{
// 		numberOfTargetPoints = 0;
// 		return;
// 	}
//
// 	initialTransform = Parent->GetActorTransform();
// 	initialLocation = initialTransform.GetLocation();
// 	initialQuaternion = initialTransform.GetRotation();
//
// 	// The first stage movement is a circle
// 	// Calculate center of the circle from actor location
// 	float radio = freeJumpRadius;
// 	FVector center = initialLocation;
//
// 	center.X += radio;
//
// 	if (numberOfTargetPoints > 0)
// 	{
// 		float theta = 0.0f;
// 		float deltaTheta = 2 * PI / numberOfTargetPoints;
//
// 		//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, FString::Printf(TEXT("X is %g Y is %g"), initialLocation.X, initialLocation.Y));
//
// 		FVector newLocation = initialLocation;
// 		FTransform newTransform = initialTransform;
// 		for (int32 i = 0; i < numberOfTargetPoints; i++)
// 		{
// 			newLocation.X = center.X - radio * FMath::Cos(theta);
// 			newLocation.Y = center.Y + radio * FMath::Sin(theta);
// 			//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, FString::Printf(TEXT("X is %g Y is %g"), newLocation.X, newLocation.Y));
// 			newTransform.SetLocation(newLocation);
// 			// Change the rotation of the actor to follow the tangent of the circle
// 			if (i != (numberOfTargetPoints - 1))
// 			{
// 				// FRotator requires angles in degrees!
// 				FRotator rotation = FRotator(0.0f, -(theta * 180.0f / PI) - 90, 0.0f);
// 				FQuat newQuaternion = rotation.Quaternion() * initialQuaternion;
// 				newTransform.SetRotation(newQuaternion);
// 			}
// 			else
// 				newTransform.SetRotation(initialQuaternion); //Last transformation 
// 			targetPoints.Add(newTransform);
// 			theta += deltaTheta;
// 		}
// 	}
// }