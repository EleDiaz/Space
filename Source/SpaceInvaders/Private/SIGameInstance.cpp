// Fill out your copyright notice in the Description page of Project Settings.


#include "SIGameInstance.h"

void USIGameInstance::Init()
{
	Super::Init();

	Score = NewObject<UScoreData>();
}
