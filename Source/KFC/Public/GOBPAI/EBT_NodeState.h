// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum EBT_NodeState: uint8
{
	Running = 0,
	Success = 1,
	Failure = 2
};

