// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ScoreData.generated.h"

USTRUCT(BlueprintType)
struct FPlayerScore
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    int32 Score;

    UPROPERTY(BlueprintReadWrite)
	FString Name;

    FPlayerScore()
    {
        Score = 0;
        Name = TEXT("Unamed hero");
    }
	
    FPlayerScore(int32 score, FString name)
    {
        Score = score;
        Name = name;
    }
};


UCLASS(Blueprintable, BlueprintType)
class SPACEINVADERS_API UScoreData : public UObject
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FPlayerScore> scores;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxSize;

	UScoreData();

	// Keep the scores sorted on each insertion
	UFUNCTION(BlueprintCallable)
	void AddItem(UPARAM() FPlayerScore PlayerScore);

	// Checks that the score will reach the top score
	UFUNCTION(BlueprintCallable)
	bool IsMemorable(UPARAM() int Score);
};
