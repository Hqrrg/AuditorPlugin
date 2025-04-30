// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class AUDITOR_API Node
{
public:
	Node(TSharedPtr<Node> Parent, FString Name, FColor Colour =FColor::FromHex(TEXT("B68F55FF")));

private:
	TSharedPtr<Node> Parent;
	TArray<TSharedPtr<Node>> Children;
	FString Name;
	FColor Colour;

public:
	FORCEINLINE TSharedPtr<Node> GetParent() const { return Parent; }
	FORCEINLINE TArray<TSharedPtr<Node>>& GetChildren() { return Children; }
	FORCEINLINE FString GetName() const { return Name; }
	FORCEINLINE FColor GetColour() const { return Colour; }
	
	void AddChild(TSharedPtr<Node> InChild);
	void AddChildren(std::initializer_list<TSharedPtr<Node>> InChildren);
};
