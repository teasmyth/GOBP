// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	FString NodeName = "Node";
	EBT_NodeState State = Running;
	bool Started = false;
	AActor* Owner = nullptr;
	UGobpAction* Action = nullptr;
	

	virtual EBT_NodeState Update() final;
	void SetAction (UGobpAction* InAction);
	virtual void DeleteNode() = 0;
	virtual void ResetNode();


protected:

	//Technically, I am not planning to use these, as I am replacing them with Action's Start,Update and Exit functions
	//Keeping these for the sake of traditional structure of a Behaviour Tree
	virtual void OnStart();
	virtual void OnExit();
	virtual EBT_NodeState OnUpdate() = 0;
	
};
