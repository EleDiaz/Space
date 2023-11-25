// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreData.h"


UScoreData::UScoreData(): scores{}, MaxSize(10) {
    scores.Reserve(MaxSize);
}


void UScoreData::AddItem(UPARAM() FPlayerScore PlayerScore) {
    scores.Add(PlayerScore);

    // TODO: Sorting the value
    // auto swappingTerm = PlayerScore;
    // for (size_t i = 0; i < scores.Num(); i++)
    // {
    //     if (scores[i].Score < swappingTerm.Score) {
    //         auto aux = scores[i];
    //         scores[i] = swappingTerm;
    //         swappingTerm = aux;
    //     }
    // }

    if (scores.Num() > MaxSize) {
        scores.Pop();
    }
}

bool UScoreData::IsMemorable(UPARAM() int Score) {
    return scores.Num() > 0 && scores.Last(0).Score < Score
        || scores.Num() <= MaxSize;
}