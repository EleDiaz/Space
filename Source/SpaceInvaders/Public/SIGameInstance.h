// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ScoreData.h"
#include "Engine/GameInstance.h"
#include "SIGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class SPACEINVADERS_API USIGameInstance : public UGameInstance
{
public:
	virtual void Init() override;

private:
	GENERATED_BODY()

public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	UScoreData* Score;

};
