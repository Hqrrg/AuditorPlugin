// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class AUDITOR_API Node
{
public:
	Node(Node* Parent, FString Name, FColor Colour =FColor::FromHex(TEXT("B68F55FF")));

private:
	Node* Parent;
	TArray<Node*> Children;
	FString Name;
	FColor Colour;

public:
	FORCEINLINE Node* GetParent() const { return Parent; }
	FORCEINLINE TArray<Node*>& GetChildren() { return Children; }
	FORCEINLINE FString GetName() const { return Name; }
	FORCEINLINE FColor GetColour() const { return Colour; }
	
	void AddChild(Node* InChild);
	void AddChildren(std::initializer_list<Node*> InChildren);
};
