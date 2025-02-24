// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class AUDITOR_API Node
{
public:
	Node(Node* Parent, FString Name);

private:
	Node* Parent;
	TArray<Node*> Children;
	FString Name;

public:
	FORCEINLINE Node* GetParent() const { return Parent; }
	FORCEINLINE TArray<Node*>& GetChildren() { return Children; }
	FORCEINLINE FString GetName() const { return Name; }

	void AddChild(Node* InChild);
	void AddChildren(std::initializer_list<Node*> InChildren);
};
