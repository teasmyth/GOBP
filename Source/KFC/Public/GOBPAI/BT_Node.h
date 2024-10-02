// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KFC/Public/Football/PlayerStats.h"
#include "KFC/Public/GOBPAI/GOBPAction.h"
#include "KFC/Public/GOBPAI/EBT_NodeState.h"

/**
 * 
 */


class KFC_API BT_Node
{
public:
	BT_Node();
	virtual ~BT_Node() = default;

	FString NodeName;
	EBT_NodeState State = Running;
	bool Started = false;
	UGobpAction* Action = nullptr;
	bool bIsRootNode = false;
	

	virtual EBT_NodeState Update(UPlayerStats* Player) final;
	virtual void ResetNode();


protected:

	//On OnStart, for single frame actions, we can return Success or Failure
	virtual EBT_NodeState OnStart(UPlayerStats* Player) = 0;
	virtual void OnExit(UPlayerStats* Player) = 0;
	virtual EBT_NodeState OnUpdate(UPlayerStats* Player) = 0;
	
};
