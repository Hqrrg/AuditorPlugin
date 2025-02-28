// Fill out your copyright notice in the Description page of Project Settings.


#include "Node.h"

Node::Node(Node* Parent, FString Name, FColor Colour)
{
	this->Parent = Parent;
	this->Name = Name;
	this->Colour = Colour;
}

void Node::AddChild(Node* InChild)
{
	if (InChild == nullptr) return;
	Children.Add(InChild);
}

void Node::AddChildren(std::initializer_list<Node*> InChildren)
{
	for (Node* Child : InChildren)
	{
		if (Child == nullptr) continue;
		Children.Add(Child);
	}
}
