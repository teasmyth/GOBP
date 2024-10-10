// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
//Limitation: due to dependencies, whenever we update this list, it will require a recompile of the entire project that is affected by AI.
//todo decoupling.
UENUM(BlueprintType)
enum class EConditions : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Divider = 1 UMETA(DisplayName = "----------"),
	HasBall = 2 UMETA(DisplayName = "Has Ball"),
	ClearShot = 3 UMETA(DisplayName = "Clear Shot"),
	ClearPass = 4 UMETA(DisplayName = "Clear Pass"),
	Shoot = 5 UMETA(DisplayName = "Shoot"),
	Corners = 6 UMETA(DisplayName = "Corners"),
	Pass = 7 UMETA(DisplayName = "Pass"),
	ObstructedShot = 8 UMETA(DisplayName = "Obstructed Shot"),
	ObstructedPass = 9 UMETA(DisplayName = "Obstructed Pass"),
	Defend = 10 UMETA(DisplayName = "Defend"),
	Intercept = 11 UMETA(DisplayName = "Intercept"),
	Mark = 12 UMETA(DisplayName = "Mark"),
	Pressure = 13 UMETA(DisplayName = "Pressure"),
	BallAction = 14 UMETA(DisplayName = "Ball Action"),
	NoBall = 15 UMETA(DisplayName = "No Ball"),
	ObstructedDribble = 16 UMETA(DisplayName = "Obstructed Dribble"),
	CanOvercomeOpponent = 17 UMETA(DisplayName = "Can Overcome Opponent"),
	OvercomeOpponentSuccess = 18 UMETA(DisplayName = "Overcome Opponent Success"),
	OvercomeOpponentFailed = 19 UMETA(DisplayName = "Overcome Opponent Failed"),
	Dribble = 20 UMETA(DisplayName = "Dribble"),
};

FORCEINLINE uint32 GetTypeHash(const EConditions& Condition)
{
	return static_cast<uint32>(Condition);
}
