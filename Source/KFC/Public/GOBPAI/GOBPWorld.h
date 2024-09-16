// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KFC/Public/GOBPAI/WorldStates.h"

/**
 * 
 */
//
	class KFC_API GOBPWorld
	{
	public:
		GOBPWorld()
		{
			World = FWorldStates();
		}
		~GOBPWorld();

		static GOBPWorld* GetInstance()
		{
			if (!Instance)
			{
				Instance = new GOBPWorld();
			}
			return Instance;
		}

		static GOBPWorld *Instance;

		static FWorldStates GetWorld()
		{
			return World;
		}
		
	private:
		static FWorldStates World;
	};


	

